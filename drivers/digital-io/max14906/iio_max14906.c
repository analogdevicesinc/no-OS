#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "max14906.h"
#include "iio_max14906.h"

#define MAX14906_CHANNEL			\
        {					\
            .ch_type = IIO_VOLTAGE,		\
            .indexed = 1,			\
	}

static int max14906_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
					   const struct iio_ch_info *channel,
					   intptr_t priv);
static int max14906_iio_read_do_mode(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_do_mode_avail(void *dev, char *buf, uint32_t len,
				     	   const struct iio_ch_info *channel,
					   intptr_t priv);
static int max14906_iio_write_do_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_samples(void *dev, uint32_t *buf, uint32_t samples);
static int max14906_iio_trigger_handler(struct iio_device_data *dev_data);
static int max14906_iio_update_channels(void *dev, uint32_t mask);

static const char *const max14906_do_mode_avail[4] = {
	[MAX14906_HIGH_SIDE] = "High_side",
	[MAX14906_HIGH_SIDE_INRUSH] = "High_side_inrush",
	[MAX14906_PUSH_PULL_CLAMP] = "Push_pull_clamp",
	[MAX14906_PUSH_PULL] = "Push_pull",
};

static struct scan_type max14906_iio_scan_type = {
	.sign = 'u',
	.realbits = 1,
	.storagebits = 8,
	.shift = 0,
	.is_big_endian = false,
};

static struct iio_attribute max14906_out_attrs[] = {
	{
		.name = "raw",
		.show = max14906_iio_read_raw,
		.store = max14906_iio_write_raw,
	},
	{
		.name = "offset",
		.show = max14906_iio_read_offset,
	},
	{
		.name = "scale",
		.show = max14906_iio_read_scale,
	},
	{
		.name = "do_mode",
		.show = max14906_iio_read_do_mode,
		.store = max14906_iio_write_do_mode,
	},
	{
		.name = "do_mode_available",
		.show = max14906_iio_read_do_mode_avail
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max14906_in_attrs[] = {
	{
		.name = "raw",
		.show = max14906_iio_read_raw,
		.store = max14906_iio_write_raw,
	},
	{
		.name = "offset",
		.show = max14906_iio_read_offset,
	},
	{
		.name = "scale",
		.show = max14906_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel max14906_channels[MAX14906_CHANNELS] = {
	MAX14906_CHANNEL,
	MAX14906_CHANNEL,
	MAX14906_CHANNEL,
	MAX14906_CHANNEL,
};

static struct iio_device max14906_iio_dev = {
	.num_ch = 4,
	.channels = max14906_channels,
    .read_dev = (int32_t (*)())max14906_iio_read_samples,
	.debug_reg_read = (int32_t (*)())max14906_reg_read,
	.debug_reg_write = (int32_t (*)())max14906_reg_write,
};

static int max14906_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	ret = max14906_get_ch_val(desc->max14906_desc, channel->address, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

static int max14906_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *desc = dev;
	int32_t val;
	int ret;

	if (!channel->ch_out)
		return -EINVAL;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return max14906_set_ch_val(desc, channel->address, val);
}

static int max14906_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int max14906_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val = 1;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int max14906_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
					   const struct iio_ch_info *channel,
					   intptr_t priv)
{
	int32_t val = 100;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int max14906_iio_read_do_mode(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint8_t val;
	int ret;

	ret = max14906_reg_read(desc, MAX14906_CONFIG_DO_REG, &val);
	if (ret)
		return ret;

	val = no_os_field_get(MAX14906_DO_MASK(channel->address), val);

	return snprintf(buf, NO_OS_ARRAY_SIZE(max14906_do_mode_avail[val]), "%s",
			max14906_do_mode_avail[val]);
}

static int max14906_iio_read_do_mode_avail(void *dev, char *buf, uint32_t len,
				     	   const struct iio_ch_info *channel,
					   intptr_t priv)
{
	int length;
	int i;

	for (i = 0; i < 4; i++){
		length += snprintf(buf + length, NO_OS_ARRAY_SIZE(max14906_do_mode_avail), "%s ",
				   max14906_do_mode_avail[i]);
	}

	return length;
}

static int max14906_iio_write_do_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint32_t do_mode;
	int i;

	do_mode = 0;
	for (i = 0; i < NO_OS_ARRAY_SIZE(max14906_do_mode_avail); i++){
		if (!strncmp(buf, max14906_do_mode_avail[i],
		    NO_OS_ARRAY_SIZE(max14906_do_mode_avail[i])))
		    	break;

		do_mode++;
	}

	if (do_mode == NO_OS_ARRAY_SIZE(max14906_do_mode_avail))
		return -EINVAL;

	return max14906_do_config(desc, channel->address, do_mode);
}

int max14906_iio_setup_channels(struct max14906_iio_desc *desc)
{
	struct max14906_desc *dev = desc->max14906_desc;
	uint8_t dir;
	int ret;
	int i;

	for (i = 0; i < MAX14906_CHANNELS; i++){
		ret = no_os_gpio_get_direction(dev->dio[i], &dir);
		if (ret)
			return ret;
		if (dir == NO_OS_GPIO_IN){
			max14906_channels[i].attributes = max14906_in_attrs;
			max14906_channels[i].ch_out = 0;
		} else {
			max14906_channels[i].attributes = max14906_out_attrs;
			max14906_channels[i].ch_out = 1;
		}

		max14906_channels[i].scan_index = i;
	}

	return 0;
}

static int max14906_iio_read_samples(void *dev, uint8_t *buf, uint32_t samples)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint32_t val;
	int i, j;
	int ret;

	for (i = 0; i < samples * iio_desc->no_of_active_channels; i++){
		for (j = 0; j < MAX14906_CHANNELS; j++){
			if (iio_desc->mask & NO_OS_BIT(j)){
				ret = max14906_ch_read(desc, j, &val);
				if (ret)
					return ret;
				buf[i++] = val;
			}
		}
	}

	return samples;
}

static int max14906_iio_trigger_handler(struct iio_device_data *dev_data)
{
	struct max14906_iio_desc *iio_desc = dev_data->dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint8_t data_buff[MAX14906_CHANNELS];
	uint32_t val;
	int ret;
	int i;

	if (iio_desc->mask & NO_OS_BIT(i)){
		ret = max14906_ch_read(desc, i, &val);
		if (ret)
			return ret;
		data_buff[i++] = val;
	}

	// for (i = 0; i < MAX14906_CHANNELS; i++){
		
	// }
}

int max14906_iio_init(struct max14906_iio_desc **iio_desc,
		      struct max14906_iio_desc_init_param *init_param)
{
	struct max14906_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->max14906_init_param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max14906_init(&descriptor->max14906_desc,
			    init_param->max14906_init_param);
	if (ret)
		goto free_desc;

	ret = max14906_iio_setup_channels(descriptor);
	if (ret)
		return ret;

	descriptor->iio_dev = &max14906_iio_dev;
	*iio_desc = descriptor;

free_desc:
	free(descriptor);

	return ret;
}
