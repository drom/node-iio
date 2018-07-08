#!/usr/bin/env node
'use strict';

const iio = require('../');

const ver = iio.library_get_version();

console.log(ver);

for(let i = 0; i < iio.get_backends_count(); i++) {
    console.log(iio.get_backend(i));
}

const scanCxt = iio.create_scan_context('usb', 0);

const info = iio.scan_context_get_info_list(scanCxt);

for(let i = 0; i < info.length; i++) {
    console.log(
        iio.context_info_get_description(info.ref, i),
        iio.context_info_get_uri(info.ref, i)
    );
}
