#!/usr/bin/env node
'use strict';

const iio = require('../');

const ver = Buffer.alloc(16);
iio.library_get_version(ver);

console.log(
    'version:' + ver.readUIntLE(0, 4) + '.' + ver.readUIntLE(4, 4) +
    ' git:' + ver.slice(8, 15).toString()
);

for(let i = 0; i < iio.get_backends_count(); i++) {
    console.log(iio.get_backend(i));
}

const scanCxt = iio.create_scan_context(null, 0);
const info = iio.scan_context_get_info_list(scanCxt);
const infoCount = iio.context_info_count(info);

for(let i = 0; i < infoCount; i++) {
    console.log(
        iio.context_info_get_description_index(info, i),
        iio.context_info_get_uri_index(info, i)
    );
}
