'use strict';

const chanType = require('./chan-type');

const rango = len => Array.apply(null, Array(len));

const keyValReductor = (prev, cur) => {
    prev[cur.key] = cur;
    cur.value = cur.getString()
    return prev;
};

const bufSlice = (buf, len) =>
    ((len |0) > 0)
        ? buf.slice(0, len - 1).toString()
        : undefined;

const genGenPerChannelAttribute = iio => cha => i => {
    const key = iio.channel_get_attr(cha, i);
    // const dst = Buffer.alloc(1024);
    // const len = iio.channel_attr_read(
    //     cha, Buffer.from(key + '\u0000'), dst, 1024
    // );
    const bufKey = Buffer.from(key + '\u0000');
    return {
        key: key,
        // len: len |0,
        getString: function () {
            const dst = Buffer.alloc(1024);
            const len = iio.channel_attr_read(cha, bufKey, dst, 1024);
            return bufSlice(dst, len);
        },
        setString: function (val) {
            const dst = Buffer.alloc(1024);
            let len = iio.channel_attr_read(cha, bufKey, dst, 1024);
            const orig = bufSlice(dst, len);
            const res = iio.channel_attr_write(cha, bufKey, Buffer.from(val + '\u0000'));
            len = iio.channel_attr_read(cha, bufKey, dst, 1024);
            const final = bufSlice(dst, len);
            console.log(orig + ' -> ' + final);
            return res;
        },
        setLongLong: function (val) {
            return iio.channel_attr_write_longlong(cha, bufKey, val);
        },
        setBuffer: function (val) {
            return iio.channel_attr_write_raw(cha, bufKey, val, val.length);
        },
    };
};

const genGenPerDeviceAttribute = iio => dev => i => {
    const key = iio.device_get_attr(dev, i);
    // const dst = Buffer.alloc(1024);
    // const len = iio.device_attr_read(
    //     dev, Buffer.from(key + '\u0000'), dst, 1024
    // );
    const bufKey = Buffer.from(key + '\u0000');
    return {
        key: key,
        getString: function () {
            const dst = Buffer.alloc(1024);
            const len = iio.device_attr_read(dev, bufKey, dst, 1024);
            return bufSlice(dst, len);
        },
        setString: function (val) {
            iio.device_attr_write(dev, bufKey, Buffer.from(val + '\u0000'));
        }
    };
};

const genPerChannel = iio => {
    const genPerChannelAttribute = genGenPerChannelAttribute(iio);
    return (cha, idx) => {
        const perChannelAttribute = genPerChannelAttribute(cha);
        return {
            id: iio.channel_get_id(cha),
            name: iio.channel_get_name(cha),
            obj: cha,
            type: chanType[iio.channel_get_type(cha)],
            isOutput: iio.channel_is_output(cha),
            isEnabled: iio.channel_is_enabled(cha),
            enable: () => iio.channel_enable(cha),
            disable: () => iio.channel_disable(cha),
            attr: rango(iio.channel_get_attrs_count(cha))
                .map((_, i) => i)
                .map(perChannelAttribute)
                .reduce(keyValReductor, {})
        };
    };
};

const genPerDevice = iio => {
    const genPerDeviceAttribute = genGenPerDeviceAttribute(iio);
    const perChannel = genPerChannel(iio);
    return (dev, idx) => {
        const perDeviceAttribute = genPerDeviceAttribute(dev);
        const channelCount = iio.device_get_channels_count(dev);
        return {
            idx: idx,
            id: iio.device_get_id(dev),
            name: iio.device_get_name(dev),
            obj: dev,
            getSampleSize: () => iio.device_get_sample_size(dev),
            attr: rango(iio.device_get_attrs_count(dev))
                .map((_, i) => i)
                .map(perDeviceAttribute)
                .reduce(keyValReductor, {}),
            chan: rango(channelCount)
                .map((_, i) => iio.device_get_channel(dev, i))
                .map(perChannel)
                .reduce((prev, cur) => {
                    prev[cur.isOutput ? 'output' : 'input'][cur.id] = cur;
                    return prev;
                }, {input: {}, output: {}})
        };
    };
};

const genPerContext = iio => {
    const perDevice = genPerDevice(iio);
    return (cxt, idx) => ({
        idx: idx,
        devices: rango(iio.context_get_devices_count(cxt))
            .map((_, i) => i)
            .map(i => iio.context_get_device(cxt, i))
            .map(perDevice)
            .reduce((prev, cur) => {
                prev[cur.name] = cur;
                return prev;
            }, {})
    });
};

const genDiscover = iio => {
    const perContext = genPerContext(iio);
    return uri => {
        const scanCxt = iio.create_scan_context(uri, 0);
        const info = iio.scan_context_get_info_list(scanCxt);
        const infoCount = iio.context_info_count(info);
        return rango(infoCount)
            .map((_, i) => i)
            .map(i => iio.context_info_get_uri_index(info, i))
            .map(uri => iio.create_context_from_uri(Buffer.from(uri + '\u0000')))
            .map(perContext)
    };
};

module.exports = genDiscover;
