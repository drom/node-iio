#!/usr/bin/env node
'use strict';

const iio = require('../index.js');

const rango = len => Array.apply(null, Array(len));

const perChannel = cha => ({
    id: iio.channel_get_id(cha),
    name: iio.channel_get_name(cha),
    type: iio.channel_get_type(cha),
    isOutput: iio.channel_is_output(cha) || undefined,
    isEnabled: iio.channel_is_enabled(cha) || undefined,
    attributes: rango(iio.channel_get_attrs_count(cha)).map((_, k) => {
        const key = iio.channel_get_attr(cha, k);
        const dst = Buffer.alloc(1024);
        const len = iio.channel_attr_read(
            cha,
            Buffer.from(key + '\u0000'),
            dst,
            1024
        );
        const value = ((len |0)> 0)
        ? dst.slice(0, len).toString()
        : undefined;
        return {
            key: key,
            len: len |0,
            value: value
        };
    })
});

const genPerDeviceAttribute = dev => i => {
    const key = iio.device_get_attr(dev, i);
    const dst = Buffer.alloc(1024);
    const len = iio.device_attr_read(
        dev,
        Buffer.from(key + '\u0000'),
        dst,
        1024
    );
    const value = ((len |0) > 0)
    ? dst.slice(0, len).toString()
    : undefined;

    return {
        key: key,
        len: len |0,
        value: value
    };
}

const perDevice = dev => {
    const channelCount = iio.device_get_channels_count(dev);
    return {
        id: iio.device_get_id(dev),
        name: iio.device_get_name(dev),
        attributes: rango(iio.device_get_attrs_count(dev))
            .map((_, i) => i)
            .map(genPerDeviceAttribute),
        channels: rango(channelCount)
            .map((_, i) => iio.device_get_channel(dev, i))
            .map(perChannel)
    };
};

const perContext = cxt => rango(iio.context_get_devices_count(cxt))
    .map((_, i) => iio.context_get_device(cxt, i))
    .map(perDevice);

const backends = rango(iio.get_backends_count())
    .map((_, i) => iio.get_backend(i));

const scanCxt = iio.create_scan_context(null, 0);
const info = iio.scan_context_get_info_list(scanCxt);
const infoCount = iio.context_info_count(info);

const devices = rango(infoCount)
    .map((_, i) => iio.context_info_get_uri_index(info, i))
    .map(uri => iio.create_context_from_uri(Buffer.from(uri + '\u0000')))
    .map(perContext);

console.log(JSON.stringify({
    backends: backends,
    devices: devices
}, null, 4));
