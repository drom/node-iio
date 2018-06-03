#!/usr/bin/env node
'use strict';

const iio = require('../index.js');

const contexts = iio.discover();

const pluto0 = contexts[0].devices;

const dev = {
    tx:  pluto0['cf-ad9361-dds-core-lpc'],
    rx:  pluto0['cf-ad9361-lpc'],
    phy: pluto0['ad9361-phy']
};

const lo = 2400000000;
const fs = 2500000;
const SAMPLES = 1 << 20;

// RX
const phyIV0 = dev.phy.chan.input.voltage0.attr;
      phyIV0.rf_port_select.setString('A_BALANCED'); // port A (select for rf freq.)
      phyIV0.rf_bandwidth.setString(fs); // 2 MHz rf bandwidth
      phyIV0.sampling_frequency.setString(fs); // 2.5 MS/s rx sample rate

dev.phy.chan.output.altvoltage0.attr.frequency.setString(lo); // 2.5 GHz rf frequency

// TX
const phyOV0 = dev.phy.chan.output.voltage0.attr;
      phyOV0.rf_port_select.setString('A'); // port A (select for rf freq.)
      phyOV0.rf_bandwidth.setString(fs); // 1.5 MHz rf bandwidth
      phyOV0.sampling_frequency.setString(fs); // 2.5 MS/s rx sample rate

dev.phy.chan.output.altvoltage1.attr.frequency.setString(lo); // 2.5 GHz rf frequency

dev.phy.chan.input.voltage0.enable(); // RX I ??
dev.phy.chan.input.voltage2.enable(); // RX Q ??
dev.phy.chan.output.voltage0.enable(); // TX I ??
dev.phy.chan.output.voltage2.enable(); // TX Q ??

const rxbuf = iio.device_create_buffer(dev.rx.obj, SAMPLES, false);

phyIV0.gain_control_mode.setString('manual'); // AGC
phyIV0.hardwaregain.setString('18'); // rxgain
phyOV0.hardwaregain.setString('-18.25'); // tx gain

const txOAV0 = dev.tx.chan.output.altvoltage0.attr;

txOAV0.raw.setString(1);
txOAV0.frequency.setString(10000);
txOAV0.scale.setString(1);

console.log(dev.phy.getSampleSize());
// console.log(iio.buffer_refill(rxbuf));
// console.log(iio.buffer_step(rxbuf));
// console.log(iio.buffer_end(rxbuf));

// iio_buffer_first
// iio_device_get_sample_size
// iio_context_info_list_free
