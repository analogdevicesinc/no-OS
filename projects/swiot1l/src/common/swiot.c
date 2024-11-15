/***************************************************************************//**
 *   @file   swiot.c
 *   @brief  Source file for the swiot IIO driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "iio.h"
#include "swiot.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "iio_ad74413r.h"
#include "iio_max14906.h"
#include "flc.h"
#include "mxc_sys.h"

static const char *const swiot_mode_available[] = {
	[SWIOT_CONFIG] = "config",
	[SWIOT_RUNTIME] = "runtime"
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

/**
 * @brief Setup the device configurations to be written when entering runtime mode
 * @param swiot_iio_desc - swiot device descriptor
 */
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
			break;
		case SWIOT_MAX14906:
			swiot->max14906_configs[i].enabled = true;
			swiot->max14906_configs[i].function = swiot_config[i].function;
			swiot->ad74413r_configs[i].enabled = false;
			swiot->ad74413r_configs[i].function = 0;
			break;
		default:
			return;
		}
	}

	/* Add all the AD74413R diagnostics channels as voltage type */
	for (i = 0; i < SWIOT_CHANNELS; i++) {
		swiot->ad74413r_diag_configs[i].enabled = true;
		swiot->ad74413r_diag_configs[i].function = AD74413R_DIAG_AGND;
	}
}

/**
 * @brief Read the mode device attribute
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_read_mode(void *dev, char *buf, uint32_t len,
			   const struct iio_ch_info *channel,
			   intptr_t priv)
{
	struct swiot_iio_desc *swiot = dev;

	strcpy(buf, swiot_mode_available[swiot->mode]);

	return strlen(buf);
}

/**
 * @brief Write the mode device attribute
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_write_mode(void *dev, char *buf, uint32_t len,
			    const struct iio_ch_info *channel,
			    intptr_t priv)
{
	struct swiot_iio_desc *swiot = dev;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(swiot_mode_available); i++) {
		if (!strcmp(buf, swiot_mode_available[i])) {
			if (swiot->mode != i) {
				swiot->mode_change = true;
				swiot->mode = i;

				if (swiot->mode == SWIOT_RUNTIME)
					swiot_config_devices(swiot);
			}

			return 0;
		}
	}

	return -EINVAL;
}

/**
 * @brief Read the avaialable values for the mode attribute
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_read_mode_avail(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	uint32_t i;

	strcpy(buf, "");
	for (i = 0; i < NO_OS_ARRAY_SIZE(swiot_mode_available); i++) {
		strcat(buf, swiot_mode_available[i]);
		if (i !=  NO_OS_ARRAY_SIZE(swiot_mode_available) - 1)
			strcat(buf, " ");
	}

	return strlen(buf);
}

/**
 * @brief Read the avaialable values for the device attribute
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
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

/**
 * @brief Read the avaialable values for the function attribute
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_read_function_avail(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t ch_num = priv;
	uint32_t func_cnt;
	size_t i;

	strcpy(buf, "");

	switch (swiot_config[ch_num].device) {
	case SWIOT_AD74413R:
		func_cnt = NO_OS_ARRAY_SIZE(ad74413r_function_available);
		for (i = 0; i < func_cnt; i++) {
			strcat(buf, ad74413r_function_available[i]);
			if (i != func_cnt - 1)
				strcat(buf, " ");
		}
		break;
	case SWIOT_MAX14906:
		func_cnt = NO_OS_ARRAY_SIZE(max14906_function_avail);
		for (i = 0; i < func_cnt; i++) {
			strcat(buf, max14906_function_avail[i]);
			if (i != func_cnt - 1)
				strcat(buf, " ");
		}
		break;
	default:
		return -EINVAL;
	}

	return strlen(buf);
}

/**
 * @brief Read the value of the ext_psu attribute
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
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

/**
 * @brief Read the device attribute for a specific swiot1l channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_read_device(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel,
			     intptr_t priv)
{
	int32_t ch_num = priv;

	strcpy(buf, swiot_devices_available[swiot_config[ch_num].device]);

	return strlen(buf);
}

/**
 * @brief Read the device attribute for a specific swiot1l channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_write_device(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	int32_t ch_num = priv;
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

/**
 * @brief Read the function attribute for a specific swiot1l channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_read_function(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	enum swiot_device device;
	int32_t ch_num = priv;

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

/**
 * @brief Read the enable attribute for a specific swiot1l channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_read_enable(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel,
			     intptr_t priv)
{
	int32_t ch_num = priv;

	return sprintf(buf, "%d", swiot_config[ch_num].enabled);
}

/**
 * @brief Write the enable attribute for a specific swiot1l channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_write_enable(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	int32_t ch_num = priv;
	uint32_t state;

	sscanf(buf, "%d", &state);
	swiot_config[ch_num].enabled = !!state;

	return 0;
}

/**
 * @brief Write the function attribute for a specific swiot1l channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_write_function(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	int32_t ch_num = priv;
	uint32_t func_cnt;
	uint32_t i;

	switch (swiot_config[ch_num].device) {
	case SWIOT_AD74413R:
		func_cnt = NO_OS_ARRAY_SIZE(ad74413r_function_available);
		for (i = 0; i < func_cnt; i++) {
			if (!strcmp(buf, ad74413r_function_available[i])) {
				swiot_config[ch_num].function = i;
				break;
			}

			if (i == func_cnt - 1)
				return -EINVAL;

		}
		break;
	case SWIOT_MAX14906:
		func_cnt = NO_OS_ARRAY_SIZE(max14906_function_avail);
		for (i = 0; i < func_cnt; i++) {
			if (!strcmp(buf, max14906_function_avail[i])) {
				swiot_config[ch_num].function = i;
				break;
			}

			if (i == func_cnt - 1)
				return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Write the identify attribute, which causes the identify LED to blink
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
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

/**
 * @brief Read the unique ID of the swiot1l board
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int swiot_read_id(void *dev, char *buf, uint32_t len,
			 const struct iio_ch_info *channel,
			 intptr_t priv)
{
	size_t length = 0;
	uint8_t usn[13];
	int ret;

	ret = MXC_SYS_GetUSN(usn, 13);
	if (ret)
		return ret;

	for (int i = 0; i < 12; i++)
		length += sprintf(buf + length, "%x-", usn[i]);

	length += sprintf(buf + length, "%x", usn[12]);

	return length;
}

static struct iio_attribute swiot_attrs[] = {
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

static struct iio_device swiot_iio_dev = {
	.attributes = swiot_attrs,
};

/**
 * @brief Initializes the swiot IIO descriptor.
 * @param swiot_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int swiot_iio_init(struct swiot_iio_desc **swiot_desc,
		   struct swiot_iio_desc_init_param *init_param)
{
	struct swiot_iio_desc *descriptor;
	int ret;

	if (!init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_gpio_get(&descriptor->psu_gpio, &init_param->psu_gpio_param);
	if (ret)
		goto free_descriptor;

	ret = no_os_gpio_direction_input(descriptor->psu_gpio);
	if (ret)
		goto free_psu_gpio;

	ret = no_os_gpio_get(&descriptor->identify_gpio,
			     &init_param->identify_gpio_param);
	if (ret)
		goto free_identify_gpio;

	ret = no_os_gpio_direction_output(descriptor->identify_gpio, 0);
	if (ret)
		goto free_identify_gpio;

	descriptor->ad74413r = init_param->ad74413r;
	descriptor->max14906 = init_param->max14906;
	descriptor->mode = init_param->mode;

	descriptor->iio_dev = &swiot_iio_dev;

	*swiot_desc = descriptor;

	return 0;

free_identify_gpio:
	no_os_gpio_remove(descriptor->identify_gpio);
free_psu_gpio:
	no_os_gpio_remove(descriptor->psu_gpio);
free_descriptor:
	free(descriptor);

	return ret;
}

/**
 * @brief Free the swiot iio descriptor.
 * @param swiot_desc - The descriptor to be freed.
 * @return 0 in case of success, an error code otherwise.
 */
int swiot_iio_remove(struct swiot_iio_desc *swiot_desc)
{
	int i;

	no_os_gpio_remove(swiot_desc->psu_gpio);
	no_os_gpio_remove(swiot_desc->identify_gpio);

	for (i = 0; i < SWIOT_CHANNELS; i++)
		swiot_config[i].enabled = false;

	free(swiot_desc);

	return 0;
}
