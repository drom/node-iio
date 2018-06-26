[![Build Status](https://travis-ci.org/drom/node-iio.svg?branch=master)](https://travis-ci.org/drom/node-iio)

Linux Industrial Input/Output ([IIO](https://wiki.analog.com/software/linux/docs/iio/iio)) Subsystem library ([libiio](https://wiki.analog.com/resources/tools-software/linux-software/libiio)) bindings for Node.js

### Dependencies

This module depends on [libiio package](https://github.com/analogdevicesinc/libiio) being installed on your system.

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

| name                               | description                                                    |
|:-----------------------------------|:---------------------------------------------------------------|
| library_get_version                | Get the version of the libiio library                          |
| get_backends_count                 | Get the number of available backends                           |
| get_backend                        | Retrieve the name of a given backend                           |
| create_scan_context                | Create a scan context                                          |
| scan_context_get_info_list         | Enumerate available contexts                                   |
| context_info_count                 | number of elements in the context info list **NEW**            |
| create_context_from_uri            | Create a context from a URI description                        |
| context_info_get_uri_index         | Get a uri of a discovered context **+ index**                  |
| context_info_get_description_index | Get a description of a discovered context **+ index**          |
| context_get_devices_count          | Enumerate the devices found in the given context               |
| context_get_device                 | Get the device present at the given index                      |
|                                    |                                                                |
| device_get_id                      | Retrieve the device ID (e.g. iio:device0)                      |
| device_get_name                    | Retrieve the device name (e.g. xadc)                           |
| device_get_attrs_count             | Enumerate the device-specific attributes of the given device   |
| device_get_attr                    | Get the device-specific attribute present at the given index   |
| device_attr_read                   | Read the content of the given device-specific attribute        |
| device_get_channels_count          | Enumerate the channels of the given device                     |
| device_get_channel                 | Get the channel present at the given index                     |
|                                    |                                                                |
| channel_is_output                  | Return True if the given channel is an output channel          |
| channel_get_id                     | Retrieve the channel ID (e.g. voltage0)                        |
| channel_get_name                   | Retrieve the channel name (e.g. vccint)                        |
| channel_is_enabled                 | Returns True if the channel is enabled                         |
| channel_get_type                   | Get the type of the given channel                              |
| channel_disable                    | Disable the given channel                                      |
| channel_enable                     | Enable the given channel                                       |
| channel_get_attrs_count            | Enumerate the channel-specific attributes of the given channel |
| channel_get_attr                   | Get the channel-specific attribute present at the given index  |
| channel_attr_read                  | Read the content of the given channel-specific attribute       |
|                                    |                                                                |
| channel_attr_write_raw             | Set the value of the given channel-specific attribute          |
| channel_attr_write                 | Set the value of the given channel-specific attribute          |


### License

[MIT](LICENSE)
