#ifndef IIO_MAX14906_H
#define IIO_MAX14906_H

#include "iio.h"
#include "max14906.h"

struct max14906_iio_desc {
	struct max14906_desc *max14906_desc;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint32_t no_active_channels;
	struct max14906_ch_config channel_configs[MAX14906_CHANNELS];
};

struct max14906_iio_desc_init_param {
	struct max14906_init_param *max14906_init_param;
};

int max14906_iio_init(struct max14906_iio_desc **,
                      struct max14906_iio_desc_init_param *);
int max14906_iio_remove(struct max14906_iio_desc *);

#endif