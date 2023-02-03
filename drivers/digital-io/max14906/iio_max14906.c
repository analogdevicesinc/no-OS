#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "max14906.h"
#include "iio_max14906.h"

#define MAX14906_CHANNEL(_addr)			\
        {					\
            	.ch_type = IIO_VOLTAGE,		\
        	.indexed = 1,			\
		.channel = _addr,		\
	    	.address = _addr,		\
	}

#define MAX14906_FAULT_CHANNEL				\
	{						\
            	.ch_type = IIO_VOLTAGE,			\
		.name = "fault",			\
		.attributes = max14906_fault_attrs,	\
	}

#define MAX14906_CONFIG_CHANNEL(_addr)			\
        {						\
        	.ch_type = IIO_VOLTAGE,			\
        	.indexed = 1,				\
		.address = _addr,			\
		.channel = _addr,			\
        	.attributes = max14906_config_attrs,	\
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
static int max14906_iio_read_samples(void *dev, uint8_t *buf, uint32_t samples);
static int max14906_iio_trigger_handler(struct iio_device_data *dev_data);
static int max14906_iio_update_channels(void *dev, uint32_t mask);

static int max14906_iio_read_fault_raw(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);

static int max14906_iio_read_config_function(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_write_config_function(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_config_function_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);
static int max14906_iio_read_config_iec(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_write_config_iec(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_read_config_iec_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);
static int max14906_iio_read_config_enabled(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_write_config_enabled(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int max14906_iio_read_config_apply(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_write_config_apply(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int max14906_iio_read_runtime_back(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int max14906_iio_write_runtime_back(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

extern int max14906_apply;
extern int max14906_back;

struct max14906_ch_config max14906_ch_configs[MAX14906_CHANNELS];

static char *max14906_function_avail[3] = {
	[MAX14906_OUT] = "direction_out",
	[MAX14906_IN] = "direction_in",
	[MAX14906_HIGH_Z] = "direction_high_z"
};

static const char *const max14906_do_mode_avail[4] = {
	[MAX14906_HIGH_SIDE] = "High_side",
	[MAX14906_HIGH_SIDE_INRUSH] = "High_side_inrush",
	[MAX14906_PUSH_PULL_CLAMP] = "Push_pull_clamp",
	[MAX14906_PUSH_PULL] = "Push_pull",
};

static const char *max14906_iec_avail[2] = {
	"Type_1_3",
	"Type_2"
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
		.shared = IIO_SHARED_BY_ALL,
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
	{
		.name = "IEC_type",
		.shared = IIO_SHARED_BY_DIR,
		.show = max14906_iio_read_config_iec,
		.store = max14906_iio_write_config_iec,
	},
	{
		.name = "IEC_type_available",
		.show = max14906_iio_read_config_iec_available,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max14906_fault_attrs[] = {
	{
		.name = "raw",
		.show = max14906_iio_read_fault_raw,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max14906_config_attrs[] = {
	{
		.name = "function",
		.show = max14906_iio_read_config_function,
		.store = max14906_iio_write_config_function,
	},
	{
		.name = "function_available",
		.shared = IIO_SHARED_BY_ALL,
		.show = max14906_iio_read_config_function_available,
	},
	{
		.name = "IEC_type",
		.shared = IIO_SHARED_BY_DIR,
		.show = max14906_iio_read_config_iec,
		.store = max14906_iio_write_config_iec,
	},
	{
		.name = "IEC_type_available",
		.shared = IIO_SHARED_BY_DIR,
		.show = max14906_iio_read_config_iec_available,
	},
	{
		.name = "enabled",
		.show = max14906_iio_read_config_enabled,
		.store = max14906_iio_write_config_enabled,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max14906_config_dev_attrs[] = {
	{
		.name = "apply",
		.show = max14906_iio_read_config_apply,
		.store = max14906_iio_write_config_apply,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max14906_runtime_dev_attrs[] = {
	{
		.name = "back",
		.show = max14906_iio_read_runtime_back,
		.store = max14906_iio_write_runtime_back,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel max14906_iio_channels[MAX14906_CHANNELS + 1];

static struct iio_channel max14906_config_channels[MAX14906_CHANNELS] = {
	MAX14906_CONFIG_CHANNEL(0),
	MAX14906_CONFIG_CHANNEL(1),
	MAX14906_CONFIG_CHANNEL(2),
	MAX14906_CONFIG_CHANNEL(3),
};

static struct iio_device max14906_iio_dev = {
	.read_dev = (int32_t (*)())max14906_iio_read_samples,
	.debug_reg_read = (int32_t (*)())max14906_reg_read,
	.debug_reg_write = (int32_t (*)())max14906_reg_write,
	.attributes = max14906_runtime_dev_attrs,
};

static struct iio_device max14906_iio_config_dev = {
	.num_ch = 4,
	.channels = max14906_config_channels,
	.attributes = max14906_config_dev_attrs,
};

static int max14906_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	ret = max14906_ch_get(desc->max14906_desc, channel->address, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

static int max14906_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *desc = dev;
	int32_t val;

	if (!channel->ch_out)
		return -EINVAL;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return max14906_ch_set(desc->max14906_desc, channel->address, val);
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
					   const struct iio_ch_info *channel, intptr_t priv)
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

	return sprintf(buf, "%s", max14906_do_mode_avail[val]);
}

static int max14906_iio_read_do_mode_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	int i;

	for (i = 0; i < 4; i++) {
		length += sprintf(buf + length, "%s ", max14906_do_mode_avail[i]);
	}

	return length;
}

static int max14906_iio_write_do_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint32_t do_mode;
	size_t i;

	do_mode = 0;
	for (i = 0; i < NO_OS_ARRAY_SIZE(max14906_do_mode_avail); i++) {
		if (!strncmp(buf, max14906_do_mode_avail[i],
			     NO_OS_ARRAY_SIZE(max14906_do_mode_avail[i])))
			break;

		do_mode++;
	}

	if (do_mode == NO_OS_ARRAY_SIZE(max14906_do_mode_avail))
		return -EINVAL;

	do_mode = no_os_field_prep(MAX14906_DO_MASK(channel->address), do_mode);

	return max14906_reg_update(desc, MAX14906_CONFIG_DO_REG,
				   MAX14906_DO_MASK(channel->address), do_mode);
}

static int max14906_iio_read_config_function(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t function_dev;
	char *function;

	function_dev = max14906_ch_configs[channel->address].function;
	function = max14906_function_avail[function_dev];

	strcpy(buf, function);

	return strlen(buf);
}

static int max14906_iio_write_config_function(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	size_t i;

	for (i = 0; i < 2; i++) {
		if (!strcmp(buf, max14906_function_avail[i])) {
			max14906_ch_configs[channel->address].function = i;
			break;
		}

		if (i == 1)
			return -EINVAL;
	}

	return 0;
}

static int max14906_iio_read_config_function_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	size_t i;

	strcpy(buf, "");

	for (i = 0; i < NO_OS_ARRAY_SIZE(max14906_function_avail); i++) {
		strcat(buf, max14906_function_avail[i]);

		if (i != NO_OS_ARRAY_SIZE(max14906_function_avail) - 1)
			strcat(buf, " ");
	}

	return strlen(buf);
}

static int max14906_iio_read_config_iec(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	enum max14906_iec_type iec_type;
	struct max14906_desc *desc;
	uint8_t val;
	int ret;

	desc = iio_desc->max14906_desc;
	ret = max14906_reg_read(desc, MAX14906_CONFIG_DI_REG, &val);
	if (ret)
		return ret;

	iec_type = no_os_field_get(MAX14906_IEC_TYPE_MASK, val);
	strcpy(buf, max14906_iec_avail[iec_type]);

	return strlen(buf);
}

static int max14906_iio_write_config_iec(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct max14906_iio_desc *iio_desc = dev;
	enum max14906_iec_type iec_type;
	struct max14906_desc *desc;
	uint8_t val;
	size_t i;

	desc = iio_desc->max14906_desc;
	for (i = 0; i < NO_OS_ARRAY_SIZE(max14906_iec_avail); i++) {
		if (!strcmp(buf, max14906_iec_avail[i]))
			iec_type = i;

		if (i == NO_OS_ARRAY_SIZE(max14906_iec_avail) - 1)
			return -EINVAL;
	}

	val = no_os_field_prep(MAX14906_IEC_TYPE_MASK, iec_type);

	return max14906_reg_update(desc, MAX14906_CONFIG_DI_REG, MAX14906_IEC_TYPE_MASK, val);
}

static int max14906_iio_read_config_iec_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	size_t i;
	size_t avail_size = NO_OS_ARRAY_SIZE(max14906_iec_avail);

	strcpy(buf, "");
	for (i = 0; i < avail_size; i++) {
		strcat(buf, max14906_iec_avail[i]);

		if (i != avail_size - 1)
			strcat(buf, " ");
	}

	return strlen(buf);
}

int max14906_iio_setup_channels(struct max14906_iio_desc *desc)
{
	struct iio_channel *max14906_iio_channels;
	struct iio_channel channel;
	/* The fault channel is always active */
	uint32_t enabled_ch = 1;
	size_t ch_offset = 0;
	int ret;
	size_t i;

	for (i = 0; i < MAX14906_CHANNELS; i++)
		if (max14906_ch_configs[i].enable)
			enabled_ch++;

	max14906_iio_channels = calloc(enabled_ch, sizeof(*max14906_iio_channels));
	if (!max14906_iio_channels)
		return -ENOMEM;

	/*
	 * Once get to setup the IIO channels, the device is already configured
	 * with the attributes specified by the configuration context. This only
	 * mirror's the state.
	 */
	for (i = 0; i < MAX14906_CHANNELS; i++) {
		if (!max14906_ch_configs[i].enable)
			continue;

		// max14906_iio_channels[ch_offset++].ch_type = IIO_VOLTAGE;
		// /* Each channel's index will corespond to the physical channel's number */
		// max14906_iio_channels[ch_offset++].indexed = true;
		// max14906_iio_channels[ch_offset++].channel = i;
		// max14906_iio_channels[ch_offset++].address = i;

		max14906_iio_channels[ch_offset] = (struct iio_channel)MAX14906_CHANNEL(i);

		/* Set the direction and attributes based on configuration */
		if (max14906_ch_configs[i].function == MAX14906_IN) {
			max14906_iio_channels[ch_offset].attributes = max14906_in_attrs;
			max14906_iio_channels[ch_offset].ch_out = 0;
		} else {
			max14906_iio_channels[ch_offset].attributes = max14906_out_attrs;
			max14906_iio_channels[ch_offset].ch_out = 1;
		}

		ch_offset++;
	}

	max14906_iio_channels[ch_offset++] = (struct iio_channel)MAX14906_FAULT_CHANNEL;
	max14906_iio_dev.num_ch = ch_offset;
	max14906_iio_dev.channels = max14906_iio_channels;

	return 0;
}

static int max14906_iio_read_samples(void *dev, uint8_t *buf, uint32_t samples)
{
	struct max14906_iio_desc *iio_desc = dev;
	struct max14906_desc *desc = iio_desc->max14906_desc;
	uint32_t val;
	size_t i, j;
	int ret;

	for (i = 0; i < samples * iio_desc->no_active_channels; i++) {
		for (j = 0; j < MAX14906_CHANNELS; j++) {
			if (iio_desc->active_channels & NO_OS_BIT(j)) {
				ret = max14906_ch_get(desc, j, &val);
				if (ret)
					return ret;
				buf[i++] = val;
			}
		}
	}

	return samples;
}

static int max14906_iio_read_fault_raw(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int max14906_iio_read_config_enabled(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t enabled = max14906_ch_configs[channel->address].enable;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &enabled);
}

static int max14906_iio_write_config_enabled(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t enabled;

	iio_parse_value(buf, IIO_VAL_INT, &enabled, NULL);
	max14906_ch_configs[channel->address].enable = !!enabled;

	return 0;
}

static int max14906_iio_read_config_apply(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&max14906_apply);
}

static int max14906_iio_write_config_apply(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	max14906_apply = 1;

	return 0;
}

static int max14906_iio_read_runtime_back(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&max14906_back);
}

static int max14906_iio_write_runtime_back(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	max14906_back = 1;

	return 0;
}

int max14906_iio_init(struct max14906_iio_desc **iio_desc,
		      struct max14906_iio_desc_init_param *init_param,
		      bool config)
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

	if (config) {
		*iio_desc = descriptor;
		descriptor->iio_dev = &max14906_iio_config_dev;

		return 0;
	}

	descriptor->iio_dev = &max14906_iio_dev;

	ret = max14906_iio_setup_channels(descriptor);
	if (ret)
		return ret;

	*iio_desc = descriptor;

	return 0;

free_desc:
	free(descriptor);

	return ret;
}

int max14906_iio_remove(struct max14906_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	max14906_remove(iio_desc->max14906_desc);
	free(iio_desc);

	return 0;
}
