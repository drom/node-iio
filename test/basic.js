'use strict';

const iio = require('bindings')('iioc.node');

const len = iio.get_backends_count();

console.log(len);

let backends = Array(len);
for (let i = 0; i < len; i++) {
    backends[i] = iio.get_backend(i);
}

console.log(backends);

const scanContexts = backends.map(uri => iio.create_scan_context(uri, 0));

console.log(scanContexts.map(cxt => cxt.constructor));

const usbCxt = iio.create_context_from_uri('usb');

console.log(usbCxt);

const usbDeviceCount = iio.context_get_devices_count(usbCxt);


console.log(usbDeviceCount);
