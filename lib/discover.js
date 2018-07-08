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
    return {
        key: key,
        // len: len |0,
        getString: function () {
            const len = iio.channel_attr_read(cha, key);
            return bufSlice(dst, len);
        },
        setString: function (val) {
            let len = iio.channel_attr_read(cha, key);
            const orig = bufSlice(dst, len);
            const res = iio.channel_attr_write(cha, key, val);
            len = iio.channel_attr_read(cha, key);
            const final = bufSlice(dst, len);
            console.log(orig + ' -> ' + final);
            return res;
        },
        setLongLong: function (val) {
            return iio.channel_attr_write_longlong(cha, key, val);
        },
        setBuffer: function (val) {
            return iio.channel_attr_write_raw(cha, key, val, val.length);
        },
    };
};

const genGenPerDeviceAttribute = iio => dev => i => {
    const key = iio.device_get_attr(dev, i);
    console.log(dev, i, key, '<--devAttr');
    return {
        key: key,
        getString: () => iio.device_attr_read(dev, key),
        setString: function (key) {
            iio.device_attr_write(dev, key, val);
        }
    };
};

const genPerChannel = iio => {
    const genPerChannelAttribute = genGenPerChannelAttribute(iio);
    return (cha, idx) => {
        const perChannelAttribute = genPerChannelAttribute(cha);
        const id = iio.channel_get_id(cha);
        const name = iio.channel_get_name(cha);
        console.log(id, name);
        return {
            id: id, name: name, obj: cha,
            // type: chanType[iio.channel_get_type(cha)],
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
        const id = iio.device_get_id(dev);
        const name = iio.device_get_name(dev);
        const attrCount = iio.device_get_attrs_count(dev);
        const channelCount = iio.device_get_channels_count(dev);
        console.log(idx, dev, id, name, attrCount, channelCount, '<--chan');
        const perDeviceAttribute = genPerDeviceAttribute(dev);
        return {
            idx: idx,
            id: id,
            name: name,
            obj: dev,
            getSampleSize: () => iio.device_get_sample_size(dev),
            attr: rango(attrCount)
                .map((_, i) => i)
                .map(perDeviceAttribute)
                .reduce(keyValReductor, {}),
            chan: [] // rango(channelCount)
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
        console.log(scanCxt, info);
        return rango(info.length)
            .map((_, i) => i)
            .map(i => iio.context_info_get_uri(info.ref, i))
            .map(uri => iio.create_context_from_uri(uri))
            .map(perContext)
    };
};

module.exports = genDiscover;
