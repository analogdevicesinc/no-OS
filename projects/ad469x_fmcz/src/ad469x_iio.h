#ifndef AD469X_IIO
#define AD469X_IIO

#include "iio_types.h"
#include "ad469x.h"


struct scan_type ad469x_scan_type = {
	.sign = 'u',
	.realbits = 19,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false
};

static struct iio_channel ad469x_ch0 = {
	.ch_type = IIO_VOLTAGE,
	.channel = 0,
	.scan_index = 0,
	.scan_type = &ad469x_scan_type,
	.indexed = true,
};

static struct iio_channel ad469x_ch1 = {
	.ch_type = IIO_VOLTAGE,
	.channel = 1,
	.scan_index = 1,
	.scan_type = &ad469x_scan_type,
	.indexed = true,
};

static struct iio_channel *ad469x_iio_channels[] = {
	&ad469x_ch0,
	&ad469x_ch1,
	NULL,
};

const struct iio_device ad469x_iio_descriptor = {
	.num_ch = 2,
	.channels = ad469x_iio_channels,
	.prepare_transfer = ad469x_std_sequence_ch,
	.read_dev = ad469x_seq_read_data
};

#endif