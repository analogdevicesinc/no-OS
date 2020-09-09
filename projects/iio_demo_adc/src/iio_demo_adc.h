#include "iio.h"

struct iio_attribute attr_conf = {
	name = 'configuration',
    show = iio_read__configuration,
    store = iio_change_configuration
};

struct iio_attribute attr_pi = {
	name = 'positive_input',
    show = iio_read_pi_input,
    store = iio_change_p_input
};

struct iio_attribute attr_ni = {
	name = 'positive_input',
    show = iio_read__neg_in,
    store = iio_change_neg_in
};

struct iio_channel *adc_channels[] = {
    &attr_conf,
    &attr_pi,
    &attr_ni,
    NULL
};

struct iio_channel adc_ch_1 = {
    .name = CH_0,
    .ch_type = IIO_ALTVOLTAGE,
    .scan_indx = 0,
    .scan_type = {
        .sign = 's',
        .realbits = 24,
        .storage = 32,
        .shift = 0,
        .is_big_endian = 0
    }
    .attributes = &channel_attributes,
    .ch_out = 0
}

struct iio_channel *adc_channels[] = {
    &adc_ch_1,
    NULL
}

struct iio_device iio_demo_adc_desc {
    .num_ch = 16,
    .channels = ;
}