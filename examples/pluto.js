#!/usr/bin/env node
'use strict';

const iio = require('../index.js');

const contexts = iio.discover('usb');

const pluto0 = contexts[0].devices;

const dev = {
    tx:  pluto0['cf-ad9361-dds-core-lpc'],
    rx:  pluto0['cf-ad9361-lpc'],
    phy: pluto0['ad9361-phy']
};

// set gain
const phyIV0 = dev.phy.chan.input.voltage0.attr;

phyIV0.gain_control_mode.setString('manual'); // AGC
phyIV0.hardwaregain.setString(18); // rxgain

const phyOV0 = dev.phy.chan.output.voltage0.attr;

phyOV0.hardwaregain.setString(-18); // tx gain

const txOAV0 = dev.tx.chan.output.altvoltage0.attr;

txOAV0.raw.setString(1);
txOAV0.frequency.setString(10000);
txOAV0.scale.setString(1);

const chans = [
    dev.rx.chan.input.voltage0,
    dev.rx.chan.input.voltage1,
    dev.tx.chan.output.voltage0,
    dev.tx.chan.output.voltage1
];

chans.map(cha => cha.enable());

// console.log(chans);

// iio_device_create_buffer(rx, 1024*1024, false);
const rxbuf = dev.rx.createBuffer(1 << 20, false);
const txbuf = dev.tx.createBuffer(1 << 20, false);

console.log(rxbuf, txbuf);

console.log(dev.rx.getSampleSize());
console.log(dev.tx.getSampleSize());

iio.buf_read(rxbuf, 1024, function (buf) { console.log(buf); });

// chans.map(cha => cha.disable());

// iio_channel_is_scan_element
// iio_buffer_foreach_sample (callback)
// iio_buffer_push
// iio_buffer_refill
// iio_buffer_step
// iio_buffer_end
// iio_buffer_first
// iio_device_get_sample_size
