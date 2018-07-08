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
phyIV0.hardwaregain.setString('18'); // rxgain

const phyOV0 = dev.phy.chan.output.voltage0.attr;

phyOV0.hardwaregain.setString('-18'); // tx gain

const txOAV0 = dev.tx.chan.output.altvoltage0.attr;

txOAV0.raw.setString(1);
txOAV0.frequency.setString(10000);
txOAV0.scale.setString(1);
