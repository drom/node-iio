#!/usr/bin/env node
'use strict';

const iio = require('../index.js');

const rango = len => Array.apply(null, Array(len));

const backendCount = iio.get_backends_count();
const backends = rango(backendCount).map((_, i) => iio.get_backend(i));

const uri = process.argv[2];
const backend = uri.split(':')[0];

const scanCxt = iio.create_scan_context(Buffer.from(backend + '\u0000'), 0);

const cxt = iio.create_context_from_uri(Buffer.from(uri + '\u0000'));

const deviceCount = iio.context_get_devices_count(cxt);
const devices = rango(deviceCount).map((_, i) => {
    const dev = iio.context_get_device(cxt, i);
    const channelCount = iio.device_get_channels_count(dev);
    return {
        id: iio.device_get_id(dev),
        name: iio.device_get_name(dev),
        attributes: rango(iio.device_get_attrs_count(dev)).map((_, j) => {
            const key = iio.device_get_attr(dev, j);
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
        }),
        channels: rango(channelCount).map((_, j) => {
            const cha = iio.device_get_channel(dev, j);
            return {
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
            };
        })
    };
});

console.log(JSON.stringify({
    backends: backends,
    devices: devices
}, null, 4));
