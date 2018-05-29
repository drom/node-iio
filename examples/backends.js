'use strict';

const iio = require('../');

for(let i = 0; i < iio.get_backends_count(); i++) {
    console.log(iio.get_backend(i));
}
