#!/usr/bin/env node
'use strict';

const iio = require('../index.js');

const morse = {
    'A': '.-',
    'C': '-.-.',
    'D': '-..',
    'E': '.',
    'I': '..',
    'P': '.--.',
    'Q': '--.-',
    'Y': '-.--',
    '6': '-....',
    ' ': ' '
};


const contexts = iio.discover('usb');

const pluto0 = contexts[0].devices;

const dev = {
    tx:  pluto0['cf-ad9361-dds-core-lpc'],
    rx:  pluto0['cf-ad9361-lpc'],
    phy: pluto0['ad9361-phy']
};

const phyOV0 = dev.phy.chan.output.voltage0.attr;

const freq = 144020000;
phyOV0.hardwaregain.setString(0); // tx gain
// const freq = 432000000;
// phyOV0.hardwaregain.setString(-5); // tx gain
// const freq = 1296000000;
// phyOV0.hardwaregain.setString(-30); // tx gain

dev.phy.chan.output.altvoltage1.attr.frequency.setString(freq); // TX LO

dev.tx.chan.output.altvoltage0.attr.scale.setString(1); // TX1_I_F1
dev.tx.chan.output.altvoltage1.attr.scale.setString(1); // TX1_I_F2

// dev.tx.chan.output.voltage0.attr.sampling_frequency.setString(31000000);

dev.phy.chan.output.voltage2.attr.sampling_frequency.setString(3500000);
dev.phy.chan.output.voltage3.attr.sampling_frequency.setString(3500000);
// dev.tx.chan.output.altvoltage0.attr.frequency.setString(freq); // TX1_I_F1
// dev.tx.chan.output.altvoltage1.attr.frequency.setString(freq); // TX1_I_F2

const sleep = time => new Promise((resolve) => setTimeout(resolve, time));

const dt = 80;
const cw = async str => {

    const arr = str.split('');
    dev.phy.chan.output.altvoltage1.attr.powerdown.setString(1);
    for (let i = 0; i < arr.length; i++) {
        const char = morse[arr[i]].split('');
        for (let j = 0; j < char.length; j++) {
            switch (char[j]) {
            case '.':
                dev.phy.chan.output.altvoltage1.attr.powerdown.setString(0);
                await sleep(dt);
                dev.phy.chan.output.altvoltage1.attr.powerdown.setString(1);
                break;
            case '-':
                dev.phy.chan.output.altvoltage1.attr.powerdown.setString(0);
                await sleep(3 * dt);
                dev.phy.chan.output.altvoltage1.attr.powerdown.setString(1);
                break;
            case ' ':
                await sleep(3 * dt);
                break;
            }
            await sleep(dt);
        }
        await sleep(3 * dt);
    }
};
// dev.phy.chan.output.altvoltage1.attr.powerdown.setString(0);
cw('CQ CQ CQ DE AI6YP AI6YP AI6YP  CQ CQ CQ DE AI6YP AI6YP AI6YP  CQ CQ CQ DE AI6YP AI6YP AI6YP');
// cw('II II II II II');


// set gain
// const phyIV0 = dev.phy.chan.input.voltage0.attr;
//
// phyIV0.gain_control_mode.setString('manual'); // AGC
// phyIV0.hardwaregain.setString(18); // rxgain
//
// const phyOV0 = dev.phy.chan.output.voltage0.attr;
//
// phyOV0.hardwaregain.setString(-40); // tx gain
//
// dev.phy.chan.output.altvoltage1.attr.frequency.setString('433000000'); // TX LO
// dev.phy.chan.output.altvoltage1.attr.powerdown.setString(1);


//
// const txOAV0 = dev.tx.chan.output.altvoltage0.attr;
//
// txOAV0.raw.setString(1);
// txOAV0.frequency.setString(10000);
// txOAV0.scale.setString(1);
//
// const chans = [
//     dev.rx.chan.input.voltage0,
//     dev.rx.chan.input.voltage1,
//     dev.tx.chan.output.voltage0,
//     dev.tx.chan.output.voltage1
// ];
//
// chans.map(cha => cha.enable());

// console.log(chans);

// iio_device_create_buffer(rx, 1024*1024, false);
// const rxbuf = dev.rx.createBuffer(1 << 20, false);
// const txbuf = dev.tx.createBuffer(1 << 20, false);
//
// console.log(rxbuf, txbuf);
//
// console.log(dev.rx.getSampleSize());
// console.log(dev.tx.getSampleSize());
//
// iio.buf_read(rxbuf, 1 << 17, function () { console.log('CCC'); });

// chans.map(cha => cha.disable());

// iio_channel_is_scan_element
// iio_buffer_foreach_sample (callback)
// iio_buffer_push
// iio_buffer_refill
// iio_buffer_step
// iio_buffer_end
// iio_buffer_first
// iio_device_get_sample_size
