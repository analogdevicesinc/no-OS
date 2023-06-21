#ifndef IIO_MAX14906_H
#define IIO_MAX14906_H

#include "iio.h"
#include "max14906.h"

#define MAX14906_FUNCTION_CNT	3

static const char *const max14906_function_avail[MAX14906_FUNCTION_CNT] = {
	[MAX14906_OUT] = "output",
	[MAX14906_IN] = "input",
	[MAX14906_HIGH_Z] = "high_z"
};

static const char *max14906_do_mode_avail[4] = {
	[MAX14906_HIGH_SIDE] = "High_side",
	[MAX14906_HIGH_SIDE_INRUSH] = "High_side_2x_inrush",
	[MAX14906_PUSH_PULL_CLAMP] = "Push_pull_clamp",
	[MAX14906_PUSH_PULL] = "Push_pull",
};

static const char *max14906_iec_avail[2] = {
	"Type_1_3",
	"Type_2"
};

struct max14906_iio_desc {
	struct max14906_desc *max14906_desc;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint32_t no_active_channels;
	struct max14906_ch_config (*channel_configs)[MAX14906_CHANNELS];
};

struct max14906_iio_desc_init_param {
	struct max14906_init_param *max14906_init_param;
	struct max14906_ch_config (*channel_configs)[MAX14906_CHANNELS];
};

int max14906_iio_init(struct max14906_iio_desc **,
                      struct max14906_iio_desc_init_param *,
		      bool);
int max14906_iio_remove(struct max14906_iio_desc *);

#endif