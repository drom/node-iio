'use strict';

const iio = require('bindings')('iio.node');

const len = iio.get_backends_count();

console.log(len);

let backends = Array(len);
for (let i = 0; i < len; i++) {
    backends[i] = iio.get_backend(i);
}

console.log(backends);
