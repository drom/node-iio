'use strict';

const chanType = require('./chan-type');

const rango = len => Array.apply(null, Array(len));

const keyValReductor = (prev, cur) => {
    prev[cur.key] = cur.getValue();
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
    return {
        key: key,
        // len: len |0,
        getValue : function () {
            const dst = Buffer.alloc(1024);
            const len = iio.channel_attr_read(
                cha, Buffer.from(key + '\u0000'), dst, 1024
            );
            return bufSlice(dst, len);
        },
        setValue: function (val) {
            console.log(val);
        }
    };
};

const genGenPerDeviceAttribute = iio => dev => i => {
    const key = iio.device_get_attr(dev, i);
    // const dst = Buffer.alloc(1024);
    // const len = iio.device_attr_read(
    //     dev, Buffer.from(key + '\u0000'), dst, 1024
    // );
    return {
        key: key,
        // len: len |0,
        // value: bufSlice(dst, len),
        getValue: function () {
            const dst = Buffer.alloc(1024);
            const len = iio.device_attr_read(
                dev, Buffer.from(key + '\u0000'), dst, 1024
            );
            return bufSlice(dst, len);
        },
        setValue: function (val) {
            console.log(val);
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
            isOutput: iio.channel_is_output(cha) || undefined,
            isEnabled: iio.channel_is_enabled(cha) || undefined,
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
