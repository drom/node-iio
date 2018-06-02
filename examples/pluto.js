#!/usr/bin/env node
'use strict';

const iio = require('../index.js');

const contexts = iio.discover();

const pluto0 = contexts[0].devices;

const devices = {
    tx:  pluto0['cf-ad9361-dds-core-lpc'],
    rx:  pluto0['cf-ad9361-lpc'],
    phy: pluto0['ad9361-phy']
};

console.log(JSON.stringify(devices, null, 4));
// console.log(devices);
