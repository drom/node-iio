#!/usr/bin/env node
'use strict';

const iio = require('./index.js');

const rango = len => Array.apply(null, Array(len));

const backendCount = iio.get_backends_count();

const backends = rango(backendCount).map((_, i) => iio.get_backend(i));

console.log('backends:', backends);

const backend = Buffer.from('usb\u0000');
const scanCxt = iio.create_scan_context(backend, 0);

console.log(scanCxt);

const cxt = iio.create_context_from_uri(Buffer.from('usb:1.5.5\u0000'));

console.log(cxt);

const deviceCount = iio.context_get_devices_count(cxt);
const devices = rango(deviceCount).map((_, i) => {
    const dev = iio.context_get_device(cxt, i);
    const channelCount = iio.device_get_channels_count(dev);
    return {
        id: iio.device_get_id(dev),
        name: iio.device_get_name(dev),
        channels: rango(channelCount).map((_, j) => {
            const cha = iio.device_get_channel(dev, j);
            return {
                id: iio.channel_get_id(cha),
                name: iio.channel_get_name(cha),
                type: iio.channel_get_type(cha),
                isOutput: iio.channel_is_output(cha) || undefined,
                isEnabled: iio.channel_is_enabled(cha) || undefined
            };
        })
    };
});

console.log(JSON.stringify(devices, null, 4));

// const main = () => {
//     const rx  = iio.context_get_device(cxt, 2); // cf-ad9361-lpc
//     const tx  = iio.context_get_device(cxt, 0); // cf-ad9361-dds-core-lpc
//     const phy = iio.context_get_device(cxt, 1); // ad9361-phy
//     const deviceChannelCount = iio.device_get_channels_count(rx);
// }
//
// main();
