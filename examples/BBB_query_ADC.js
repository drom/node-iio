const iio = require('libiio');

console.log(iio.library_get_version());

for(let i = 0; i < iio.get_backends_count(); i++) {
    console.log(iio.get_backend(i));
}

var iio_context = iio.create_context_from_uri('local:');
var dev = iio.context_get_device(iio_context, 0);
console.log([iio.device_get_id(dev), iio.device_get_name(dev), 
    iio.device_get_channels_count(dev),
    iio.device_get_sample_size(dev)
]);

// BeagleBone Black has 7 ADC channels, we are using the 4-th here.
var channel = iio.device_get_channel(dev, 4);
iio.channel_enable(channel);
console.log([iio.channel_get_id(channel), iio.channel_is_enabled(channel), 
    iio.channel_get_type(channel),
    iio.channel_get_attrs_count(channel)
]);
for (let i = 0; i < iio.channel_get_attrs_count(channel); i++) {
    console.log([iio.channel_get_attr(channel, i),
        iio.channel_attr_read(channel, iio.channel_get_attr(channel, i))
    ]);
}

// Asking for 10 consecutive samples for the channels we enabled earlier. 
var buffer = iio.device_create_buffer(dev, 10, false);
iio.buf_read(buffer, function (chnl_id, sample) {
    console.log('Chnl=', chnl_id, ', sample=', sample);
    return 1;
});
