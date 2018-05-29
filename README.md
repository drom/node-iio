[![Build Status](https://travis-ci.org/drom/node-iio.svg?branch=master)](https://travis-ci.org/drom/node-iio)

Linux Industrial Input/Output (IIO) Subsystem library ([libiio](http://analogdevicesinc.github.io/libiio/)) bindings for Node.js

### Dependencies

This module depends on [libiio](http://analogdevicesinc.github.io/libiio/) being installed on your system.

### Install

`npm i libiio`

### Usage

```js
const iio = require('libiio');

for(let i = 0; i < iio.get_backends_count(); i++) {
    console.log(iio.get_backend(i));
}
```

### API

Most of API calls are directly mapped to [libiio API](http://analogdevicesinc.github.io/libiio/) without `iio_` prefix.

### License

[MIT](LICENSE)
