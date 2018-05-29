'use strict';

const tree = {
    'ad9361-phy': { // PHY_DEVICE
        channel: {
            altvoltage0: {  // alt_ch0
                frequency: 0.0 // |RX_LO_frequency : rx_lo_freq
                external: 0.0 // rx_lo_external
                fastlock_store: [],
                RX_LO_fastlock_store: [],
                fastlock_save: [],
                RX_LO_fastlock_save: []
            },
            voltage1: { // ch1
                hardwaregain: true // is_2rx_2tx
            }
        }
    },
    'cf-ad9361-lpc': { // CAP_DEVICE

    }
};

const sampling_freq = {
    'ad9361-phy': {
        channel: {
            voltage0: {
                sampling_frequency: 0.0 // double
            }
        }
    }
};

const gain_control_mode = {
    'ad9361-phy': {
        channel: {
            voltage0: {
                gain_control_mode: 0.0 // double
            },
            voltage1: { // 2rx2tx
                gain_control_mode: 0.0 // double
            }
        }
    }
};

const sampling_freq_rx_decim = {
    'cf-ad9361-lpc': {
        channel: {
            voltage0: {
                sampling_frequency: 0.0 // double, freq / mhz_scale
            }
        }
    }
};

const sampling_freq_tx_inter = {
    'cf-ad9361-dds-core-lpc': {
        channel: {
            voltage0: {
                sampling_frequency: 0.0 // double, freq / mhz_scale
            }
        }
    }
};

const filter_fir_update = {
    'ad9361-phy': { // PHY_DEVICE
        channel: {
            'voltage0:false': { // rx
                filter_fir_en: true // bool
            }
            'voltage0:true': { // tx
                filter_fir_en: true // bool
            }
        }
    }
};

const rx_phase_rotation_set = {
    'cf-ad9361-lpc': { // CAP_DEVICE
        channel: {
            'voltage2:false': { // voltage0
                calibscale: 0.0, // double cos(phase)
                calibphase: 0.0  // (-1 * sin(phase)))
            },
            'voltage3:false': { // voltage1
                calibscale: 0.0, // double cos(phase)
                calibphase: 0.0  // sin(phase)
            }
        }
    }
};
