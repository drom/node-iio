#!/usr/bin/env node
'use strict';

// const iio = require('bindings')('iio.node');
const iio = require('bindings')('iioc.node');

const len = iio.get_backends_count();

console.log(len);

let backends = Array(len);
for (let i = 0; i < len; i++) {
    backends[i] = iio.get_backend(i);
}

console.log(backends);

const scanContexts = backends.map(uri => iio.create_scan_context(uri, 0));

console.log(scanContexts);

const infos = scanContexts.map(cxt => iio.scan_context_get_info_list(cxt));

console.log(infos);

const uris = infos.map(info => {
    let res = [];
    for (let i = 0; i < info.length; i++) {
        const uri = iio.context_info_get_uri(info.ref);
        const cxt = iio.create_context_from_uri(uri);
        res[i] = {
            description: iio.context_info_get_description(info.ref),
            uri: uri,
            cxt: cxt
        };
    }
    return res;
});

console.log(uris);

// // const usbCxt = iio.create_context_from_uri(Buffer.from('usb:3.9.5\u0000'));
// const usbCxt = iio.create_context_from_uri('usb:3.11.5');
//
// console.log(usbCxt);
//
// const usbDeviceCount = iio.context_get_devices_count(usbCxt);
//
// console.log(usbDeviceCount);
