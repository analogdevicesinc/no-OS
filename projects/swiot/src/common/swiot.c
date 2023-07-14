#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include "iio.h"
#include "swiot.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "iio_ad74413r.h"
#include "iio_max14906.h"
#include "adc.h"
#include "flc.h"

static const char *const swiot_mode_available[] = {
	"config",
	"runtime"
};

static const char *const swiot_devices_available[SWIOT_DEV_CNT] = {
	[SWIOT_AD74413R] = "ad74413r",
	[SWIOT_MAX14906] = "max14906",
};

static struct swiot_config_state swiot_config[SWIOT_CHANNELS] = {
	[0 ... SWIOT_CHANNELS - 1] = {
		.device = SWIOT_AD74413R,
		.function = AD74413R_HIGH_Z,
	}
};

static void swiot_config_devices(struct swiot_iio_desc *swiot)
{
	int i;

	for (i = 0; i < SWIOT_CHANNELS; i++) {
		if (!swiot_config[i].enabled)
			continue;

		switch (swiot_config[i].device) {
		case SWIOT_AD74413R:
			swiot->ad74413r_configs[i].enabled = true;
			swiot->ad74413r_configs[i].function = swiot_config[i].function;
			swiot->max14906_configs[i].enabled = false;
			swiot->max14906_configs[i].function = MAX14906_HIGH_Z;
			swiot->max14906_configs[i].val = 0;
			break;
		case SWIOT_MAX14906:
			swiot->max14906_configs[i].enabled = true;
			swiot->max14906_configs[i].function = swiot_config[i].function;
			swiot->max14906_configs[i].val = AD74413R_HIGH_Z;
			swiot->ad74413r_configs[i].enabled = false;
			swiot->ad74413r_configs[i].function = 0;
			break;
		default:
			return;
		}
	}
}

static int swiot_read_mode(void *dev, char *buf, uint32_t len,
			   const struct iio_ch_info *channel,
			   intptr_t priv)
{
	struct swiot_iio_desc *swiot = dev;
	
	strcpy(buf, swiot_mode_available[swiot->mode]);

	return strlen(buf);
}

static int swiot_write_mode(void *dev, char *buf, uint32_t len,
			    const struct iio_ch_info *channel,
			    intptr_t priv)
{
	struct swiot_iio_desc *swiot = dev;
	int32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(swiot_mode_available); i++) {
		if (!strcmp(buf, swiot_mode_available[i])) {
			if (swiot->mode != i) {
				swiot->mode_change = true;
				swiot->mode = i;

				if (swiot->mode == 1)
					swiot_config_devices(swiot);
			}

			return 0;
		} 
	}

	return -EINVAL;
}

static int swiot_read_mode_avail(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	int32_t i;

	strcpy(buf, "");
	for (i = 0; i < NO_OS_ARRAY_SIZE(swiot_mode_available); i++) {
		strcat(buf, swiot_mode_available[i]);
		if (i !=  NO_OS_ARRAY_SIZE(swiot_mode_available) - 1)
			strcat(buf, " ");
	}

	return strlen(buf);
}

static int swiot_read_devices_avail(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	size_t i;

	strcpy(buf, "");
	for (i = 0; i < NO_OS_ARRAY_SIZE(swiot_devices_available); i++) {
		strcat(buf, swiot_devices_available[i]);
		if (i !=  NO_OS_ARRAY_SIZE(swiot_devices_available) - 1)
			strcat(buf, " ");
	}

	return strlen(buf);
}

static int swiot_read_function_avail(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	enum swiot_device device;
	int32_t ch_num = priv;
	size_t i;

	strcpy(buf, "");

	switch (swiot_config[ch_num].device) {
	case SWIOT_AD74413R:
		for (i = 0; i < AD74413R_FUNCTION_CNT; i++) {
			strcat(buf, ad74413r_function_available[i]);
			if (i != AD74413R_FUNCTION_CNT - 1)
				strcat(buf, " ");
		}
		break;
	case SWIOT_MAX14906:
		for (i = 0; i <= MAX14906_HIGH_Z; i++) {
			strcat(buf, max14906_function_avail[i]);
			if (i != MAX14906_HIGH_Z)
				strcat(buf, " ");
		}
		break;
	default:
		return -EINVAL;
	}

	return strlen(buf);
}

static int swiot_read_signal_mse(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct swiot_iio_desc *swiot = dev;
	int32_t iio_val = 0;
	uint16_t val = 0;
	int ret;

	ret = adin1110_mdio_read_c45(swiot->adin1110, 0x1, 0x1, 0x830B, &val);
	if (ret)
		return ret;

	iio_val = val;
	
	return iio_format_value(buf, len, IIO_VAL_INT, 1, &iio_val);
}

static int swiot_read_ext_psu(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	struct swiot_iio_desc *swiot = dev;
	int32_t val = 0;
	int ret;

	ret = no_os_gpio_get_value(swiot->psu_gpio, (uint8_t *)&val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int swiot_read_device(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	int32_t ch_num = priv;
	int ret;

	strcpy(buf, swiot_devices_available[swiot_config[ch_num].device]);

	return strlen(buf);
}

static int swiot_write_device(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	int32_t ch_num = priv;
	int ret;
	int i;

	for (i = 0; i < SWIOT_DEV_CNT; i++) {
		if (!strcmp(buf, swiot_devices_available[i])) {
			swiot_config[ch_num].device = i;
			swiot_config[ch_num].function = 0;
			break;
		}

		if (i == SWIOT_DEV_CNT - 1)
			return -EINVAL;
	}

	return 0;
}

static int swiot_read_function(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	enum swiot_device device;
	int32_t ch_num = priv;
	int ret;

	device = swiot_config[ch_num].device;

	switch (device) {
	case SWIOT_AD74413R:
		strcpy(buf, ad74413r_function_available[swiot_config[ch_num].function]);
		break;
	case SWIOT_MAX14906:
		strcpy(buf, max14906_function_avail[swiot_config[ch_num].function]);
		break;
	default:
		return -EINVAL;
	}

	return strlen(buf);
}

static int swiot_read_enable(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel,
			     intptr_t priv)
{
	int32_t ch_num = priv;
	int ret;

	return sprintf(buf, "%d", swiot_config[ch_num].enabled);
}

static int swiot_write_enable(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	int32_t ch_num = priv;
	uint32_t state;
	int ret;

	sscanf(buf, "%d", &state);
	swiot_config[ch_num].enabled = !!state;

	return 0;
}

static int swiot_write_function(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	int32_t ch_num = priv;
	int ret;
	int i;

	switch (swiot_config[ch_num].device) {
	case SWIOT_AD74413R:
		for (i = 0; i < AD74413R_FUNCTION_CNT; i++) {
			if (!strcmp(buf, ad74413r_function_available[i])) {
				swiot_config[ch_num].function = i;
				break;
			}

			if (i == AD74413R_FUNCTION_CNT - 1)
				return -EINVAL;

		}
		break;
	case SWIOT_MAX14906:
		for (i = 0; i < MAX14906_FUNCTION_CNT; i++) {
			if (!strcmp(buf, max14906_function_avail[i])) {
				swiot_config[ch_num].function = i;
				break;
			}

			if (i == MAX14906_FUNCTION_CNT - 1)
				return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int swiot_write_identify(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	struct swiot_iio_desc *desc = dev;

	for (int i = 0; i < 15; i++) {
		no_os_gpio_set_value(desc->identify_gpio, 1);
		no_os_mdelay(100);
		no_os_gpio_set_value(desc->identify_gpio, 0);
		no_os_mdelay(100);
	}

	return 0;
}

static int swiot_write_reset(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel,
			     intptr_t priv)
{
	*((uint32_t *)0x40000004) |= 1 << 31;

	return 0;
}

static int swiot_read_id(void *dev, char *buf, uint32_t len,
			 const struct iio_ch_info *channel,
			 intptr_t priv)
{
	volatile uint32_t *access_ctrl = (uint32_t *)0x40029040;
	size_t length = 0;
	uint32_t usn;

	MXC_FLC_UnlockInfoBlock(MXC_INFO_MEM_BASE);

	usn = no_os_field_get(NO_OS_GENMASK(31, 15), *(uint32_t *)0x10800000);
	length += sprintf(buf, "%x-", usn);
	usn = no_os_field_get(NO_OS_GENMASK(30, 0), *(uint32_t *)0x10800004);
	length += sprintf(buf + length, "%x-", usn);
	usn = no_os_field_get(NO_OS_GENMASK(31, 15), *(uint32_t *)0x10800008);
	length += sprintf(buf + length, "%x-", usn);
	usn = no_os_field_get(NO_OS_GENMASK(30, 0), *(uint32_t *)0x1080000C);
	length += sprintf(buf + length, "%x-", usn);
	usn = no_os_field_get(NO_OS_GENMASK(22, 15), *(uint32_t *)0x10800010);
	length += sprintf(buf + length, "%x", usn);

	return length;
}

static int swiot_adc_read_raw(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	mxc_adc_conversion_req_t req = {
		.scale = MXC_ADC_SCALE_1,
	};

	uint16_t adc_val;
	int32_t val;
	int ret;

	switch (priv)
	{
	case 0:
		req.channel = MXC_ADC_CH_VCORE;
		break;
	case 1:
		req.channel = MXC_ADC_CH_VDDIOH_DIV4;
		break;
	default:
		return -EINVAL;
	}

	ret = MXC_ADC_Convert(&req);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&req.rawADCValue);
}

static int swiot_adc_read_scale(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	int32_t val;

	switch (priv) {
	case 0:
		val = 0;
		break;
	case 1:
		val = 4;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}


static struct iio_attribute swiot_attrs[] = {
	{
		.name = "reset",
		.store = swiot_write_reset,
	},
	{
		.name = "serial_id",
		.show = swiot_read_id,
	},
	{
		.name = "mode",
		.show = swiot_read_mode,
		.store = swiot_write_mode,
	},
	{
		.name = "mode_available",
		.show = swiot_read_mode_avail,
	},
	{
		.name = "identify",
		.store = swiot_write_identify,
	},
	{
		.name = "ext_psu",
		.show = swiot_read_ext_psu,
	},
	{
		.name = "signal_mse",
		.show = swiot_read_signal_mse,
	},
	{
		.name = "ch0_enable",
		.show = swiot_read_enable,
		.store = swiot_write_enable,
		.priv = 0,
	},
	{
		.name = "ch1_enable",
		.show = swiot_read_enable,
		.store = swiot_write_enable,
		.priv = 1,
	},
	{
		.name = "ch2_enable",
		.show = swiot_read_enable,
		.store = swiot_write_enable,
		.priv = 2,
	},
	{
		.name = "ch3_enable",
		.show = swiot_read_enable,
		.store = swiot_write_enable,
		.priv = 3,
	},
	{
		.name = "ch0_function",
		.show = swiot_read_function,
		.store = swiot_write_function,
		.priv = 0,
	},
	{
		.name = "ch1_function",
		.show = swiot_read_function,
		.store = swiot_write_function,
		.priv = 1,
	},
	{
		.name = "ch2_function",
		.show = swiot_read_function,
		.store = swiot_write_function,
		.priv = 2,
	},
	{
		.name = "ch3_function",
		.show = swiot_read_function,
		.store = swiot_write_function,
		.priv = 3,
	},
	{
		.name = "ch0_device",
		.show = swiot_read_device,
		.store = swiot_write_device,
		.priv = 0,
	},
	{
		.name = "ch1_device",
		.show = swiot_read_device,
		.store = swiot_write_device,
		.priv = 1,
	},
	{
		.name = "ch2_device",
		.show = swiot_read_device,
		.store = swiot_write_device,
		.priv = 2,
	},
	{
		.name = "ch3_device",
		.show = swiot_read_device,
		.store = swiot_write_device,
		.priv = 3,
	},
	{
		.name = "ch0_function_available",
		.show = swiot_read_function_avail,
		.priv = 0
	},
	{
		.name = "ch1_function_available",
		.show = swiot_read_function_avail,
		.priv = 1
	},
	{
		.name = "ch2_function_available",
		.show = swiot_read_function_avail,
		.priv = 2
	},
	{
		.name = "ch3_function_available",
		.show = swiot_read_function_avail,
		.priv = 3
	},
	{
		.name = "ch0_device_available",
		.show = swiot_read_devices_avail,
	},
	{
		.name = "ch1_device_available",
		.show = swiot_read_devices_avail,
	},
	{
		.name = "ch2_device_available",
		.show = swiot_read_devices_avail,
	},
	{
		.name = "ch3_device_available",
		.show = swiot_read_devices_avail,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute swiot_vcore_attrs[] = {
	{
		.name = "raw",
		.show = swiot_adc_read_raw,
		.priv = 0,
	},
	{
		.name = "scale",
		.show = swiot_adc_read_scale,
		.priv = 0,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute swiot_vddioh_attrs[] = {
	{
		.name = "raw",
		.show = swiot_adc_read_raw,
		.priv = 1,
	},
	{
		.name = "scale",
		.show = swiot_adc_read_scale,
		.priv = 1,
	},
	END_ATTRIBUTES_ARRAY
};

static const struct iio_channel swiot_chan[2] = {
	{
		.name = "v_core",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.ch_out = false,
		.indexed = 1,
		.attributes = swiot_vcore_attrs,
	},
	{
		.name = "vddioh",
		.ch_type = IIO_VOLTAGE,
		.channel = 1,
		.ch_out = false,
		.indexed = 1,
		.attributes = swiot_vddioh_attrs,
	},
};

static const struct iio_device swiot_iio_dev = {
	.channels = swiot_chan,
	.num_ch = 2,
	.attributes = swiot_attrs,
};

int swiot_iio_init(struct swiot_iio_desc **swiot_desc,
		   struct swiot_iio_desc_init_param *init_param)
{
	struct swiot_iio_desc *descriptor;
	int ret;

	if (!init_param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_gpio_get(&descriptor->psu_gpio, &init_param->psu_gpio_param);
	if (ret)
		goto free_descriptor;

	ret = no_os_gpio_get(&descriptor->identify_gpio, &init_param->identify_gpio_param);
	if (ret)
		goto free_psu_gpio;

	ret = no_os_gpio_direction_output(descriptor->identify_gpio, 0);
	if (ret)
		goto free_psu_gpio;

	descriptor->ad74413r = init_param->ad74413r;
	descriptor->max14906 = init_param->max14906;
	descriptor->mode = init_param->mode;
	
	descriptor->iio_dev = &swiot_iio_dev;

	*swiot_desc = descriptor;

	return 0;

free_psu_gpio:
	no_os_gpio_remove(descriptor->psu_gpio);
free_descriptor:
	free(descriptor);

	return ret;
}

int swiot_iio_remove(struct swiot_iio_desc *swiot_desc)
{
	int i;

	no_os_gpio_remove(swiot_desc->psu_gpio);

	for (i = 0; i < SWIOT_CHANNELS; i++) {
		swiot_config[i].device = SWIOT_AD74413R;
		swiot_config[i].function = AD74413R_HIGH_Z;
		swiot_config[i].enabled = false;
	}

	free(swiot_desc);
	swiot_desc = NULL;

	return 0;
}