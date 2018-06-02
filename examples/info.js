#!/usr/bin/env node
'use strict';

const iio = require('../index.js');

const pluto = iio.discover('usb');

console.log(JSON.stringify(pluto, null, 4));
