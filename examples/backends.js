#!/usr/bin/env node
'use strict';

const iio = require('../');

const ver = Buffer.alloc(16);
iio.library_get_version(ver);

console.log(
  ver.readUIntLE(0, 4),
  ver.readUIntLE(4, 4),
  ver.slice(8, 15).toString()
);

for(let i = 0; i < iio.get_backends_count(); i++) {
    console.log(iio.get_backend(i));
}
