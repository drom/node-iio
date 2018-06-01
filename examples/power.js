#!/usr/bin/env node
'use strict';

const iio = require('../');

const get_ad9361_stream_dev = cxt => ({
    tx: 'cf-ad9361-dds-core-lpc',
    rx: 'cf-ad9361-lpc'
})

const scanCxt = iio.create_scan_context(null, 0);
const info = iio.scan_context_get_info_list(scanCxt);

const perChannel = dev => e => {
    const cha = iio.device_find_channel(dev, Buffer.from(e.name + '\u0000'), e.output || false);
    return {
        id: e.id,
        name: e.name,
        output: e.output || false,
        cha: cha
    };
};

const perDevice = cxt => e => {
    const dev = iio.context_find_device(cxt, Buffer.from(e.name + '\u0000'));
    return {
        id: e.id,
        name: e.name,
        dev: dev,
        channels: (e.channels || []).map(perChannel(dev))
    };
};

if (iio.context_info_count(info) > 0) {
    const uri = iio.context_info_get_uri_index(info, 0);
    const cxt = iio.create_context_from_uri(Buffer.from(uri + '\u0000'));

    const tree = [
        {id: 'tx',  name: 'cf-ad9361-dds-core-lpc', channels: []},
        {id: 'rx',  name: 'cf-ad9361-lpc'},
        {id: 'phy', name: 'ad9361-phy', channels: [
            {id: 'tx', name: 'voltage', output: true},
            {id: 'rx', name: 'voltage'},
            {id: 'txlo', name: 'altvoltage', output: true},
            {id: 'rxlo', name: 'altvoltage', output: true}
        ]}
    ].map(perDevice(cxt));

    console.log(tree);
}

// iio_device_find_channel
