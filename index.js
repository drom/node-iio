'use strict';

const iio = require('bindings')('iioc.node');
const chanType = require('./lib/chan-type');
const modifier = require('./lib/modifier');
const genDiscover = require('./lib/discover');

iio.chan_type = chanType;
iio.modifier = modifier;
iio.discover = genDiscover(iio);

module.exports = iio;
