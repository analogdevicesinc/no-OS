#ifndef IIO_SWIOT_H
#define IIO_SWIOT_H

#include <stdbool.h>
#include "no_os_gpio.h"
#include "ad74413r.h"
#include "max14906.h"
#include "adin1110.h"

#define SWIOT_CHANNELS	4

enum swiot_device {
	SWIOT_AD74413R,
	SWIOT_MAX14906,
	SWIOT_DEV_CNT,
};

struct swiot_config_state {
	bool enabled;
	enum swiot_device device;
	uint32_t function;
};

struct swiot_iio_desc {
	struct ad74413r_iio_desc *ad74413r;
	struct max14906_iio_desc *max14906;
	struct adin1110_desc *adin1110;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;

	int32_t mode;
	volatile bool mode_change;
	struct no_os_gpio_desc *psu_gpio;
	struct no_os_gpio_desc *identify_gpio;

	struct max14906_ch_config max14906_configs[MAX14906_CHANNELS];
	struct ad74413r_channel_config ad74413r_configs[AD74413R_N_CHANNELS];
};

struct swiot_iio_desc_init_param {
	struct ad74413r_iio_desc *ad74413r;
	struct max14906_iio_desc *max14906;
	struct no_os_gpio_init_param psu_gpio_param;
	struct no_os_gpio_init_param identify_gpio_param;
	int32_t mode;
};

int swiot_iio_init(struct swiot_iio_desc **,
		   struct swiot_iio_desc_init_param *);
int swiot_iio_remove(struct swiot_iio_desc *);

#endif