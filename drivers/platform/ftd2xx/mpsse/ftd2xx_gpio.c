/*******************************************************************************
 *   @file   ftd2xx_gpio.c
 *   @brief  Implementation of GPIO driver for ftd2xx platform.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "ftd2xx_gpio.h"
#include "ftd2xx_platform.h"

uint8_t ftd2xx_gpio_pins_dir[FTD2XX_MAX_PORT_NB];
uint8_t ftd2xx_gpio_pins_val[FTD2XX_MAX_PORT_NB];

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ftd2xx_gpio_get(struct no_os_gpio_desc **desc,
			const struct no_os_gpio_init_param *param)
{
	struct ftd2xx_gpio_desc *extra_desc;
	struct ftd2xx_gpio_init *extra_init;
	struct no_os_gpio_desc *descriptor;
	FT_STATUS status;
	bool gpio_dir;
	int32_t ret;

	if (!param || param->port >= FTD2XX_MAX_DEV_PER_CHIP
	    || !param->number > FTD2XX_MAX_PIN_NB)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	extra_desc = (struct ftd2xx_gpio_desc*)no_os_calloc(1,
			sizeof(struct ftd2xx_gpio_desc));
	if (!extra_desc) {
		ret = -ENOMEM;
		goto error;
	}

	extra_init = param->extra;
	if (ftHandle[param->port] == NULL) {
		Init_libMPSSE();
		status = FT_Open(param->port, &extra_desc->ftHandle);
		if (status != FT_OK) {
			ret = status;
			goto free_extra;
		}

		status = FT_SetBitMode(extra_desc->ftHandle, extra_init->extra_pins_dir, 0x02);
		if (status != FT_OK) {
			ret = status;
			goto free_extra;
		}

		ftd2xx_gpio_pins_dir[param->port] = extra_init->extra_pins_dir;
		ftHandle[param->port] = extra_desc->ftHandle;
	} else {
		extra_desc->ftHandle = ftHandle[param->port];
	}

	descriptor->extra = extra_desc;
	descriptor->number = param->number;
	descriptor->port = param->port;
	descriptor->pull = param->pull;

	*desc = descriptor;

	return 0;

free_extra:
	no_os_free(extra_desc);
error:
	no_os_free(descriptor);
	if (ret)
		return ret;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ftd2xx_gpio_get_optional(struct no_os_gpio_desc **desc,
				 const struct no_os_gpio_init_param *param)
{
	if (!param) {
		*desc = NULL;
		return 0;
	}

	return ftd2xx_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ftd2xx_gpio_remove(struct no_os_gpio_desc *desc)
{
	struct ftd2xx_gpio_desc *extra_desc;
	if (!desc)
		return -EINVAL;

	extra_desc = desc->extra;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ftd2xx_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	struct ftd2xx_gpio_desc *extra_desc;
	FT_STATUS status;
	uint8_t dir;
	int32_t ret;
	uint8_t data[3] = {0x80, 0x00, 0x00};
	DWORD bytes_transferred;

	extra_desc = desc->extra;

	dir = ftd2xx_gpio_pins_dir[desc->port];
	dir &= ~NO_OS_BIT(desc->number);
	dir |= NO_OS_BIT(desc->number) & no_os_field_prep(NO_OS_BIT(desc->number), 0);
	data[2] = dir;
	data[1] = ftd2xx_gpio_pins_val[desc->port];
	status = FT_Write(extra_desc->ftHandle, data, sizeof(data), &bytes_transferred);
	if (status != FT_OK) {
		ret = status;
		return ret;
	}

	ftd2xx_gpio_pins_dir[desc->port] = dir;

	return 0;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ftd2xx_gpio_direction_output(struct no_os_gpio_desc *desc,
				     uint8_t value)
{
	struct ftd2xx_gpio_desc *extra_desc;
	FT_STATUS status;
	uint8_t dir, val;
	int32_t ret;
	uint8_t data[3] = {0x80, 0x00, 0x00};
	DWORD bytes_transferred;

	extra_desc = desc->extra;
	dir = ftd2xx_gpio_pins_dir[desc->port];
	dir &= ~NO_OS_BIT(desc->number);
	dir |= NO_OS_BIT(desc->number) & no_os_field_prep(NO_OS_BIT(desc->number), 1);
	val = ftd2xx_gpio_pins_val[desc->port];
	val &= ~NO_OS_BIT(desc->number);
	val |= NO_OS_BIT(desc->number) & no_os_field_prep(NO_OS_BIT(desc->number),
			value);

	data[2] = dir;
	data[1] = val;
	status = FT_Write(extra_desc->ftHandle, data, sizeof(data), &bytes_transferred);
	if (status != FT_OK) {
		ret = status;
		return ret;
	}

	ftd2xx_gpio_pins_dir[desc->port] = dir;
	ftd2xx_gpio_pins_val[desc->port] = val;

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ftd2xx_gpio_get_direction(struct no_os_gpio_desc *desc,
				  uint8_t *direction)
{
	struct ftd2xx_gpio_desc *extra_desc;
	extra_desc = desc->extra;

	*direction = no_os_field_get(FTD2XX_GPIO_PIN(desc->number),
				     ftd2xx_gpio_pins_dir[desc->port]);

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ftd2xx_gpio_set_value(struct no_os_gpio_desc *desc, uint8_t value)
{
	struct ftd2xx_gpio_desc *extra_desc;
	FT_STATUS status;
	uint8_t val;
	int32_t ret;
	uint8_t data[3] = {0x80, 0x00, 0x00};
	DWORD bytes_transferred;

	if (no_os_field_get(FTD2XX_GPIO_PIN(desc->number),
			    ftd2xx_gpio_pins_dir[desc->port]) == 0)
		return -EINVAL;

	extra_desc = desc->extra;

	val = ftd2xx_gpio_pins_val[desc->port];
	val &= ~NO_OS_BIT(desc->number);
	val |= NO_OS_BIT(desc->number) & no_os_field_prep(NO_OS_BIT(desc->number),
			value);

	data[2] = ftd2xx_gpio_pins_dir[desc->port];
	data[1] = val;
	status = FT_Write(extra_desc->ftHandle, data, sizeof(data), &bytes_transferred);
	if (status != FT_OK) {
		ret = status;
		return ret;
	}
	ftd2xx_gpio_pins_val[desc->port] = val;

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ftd2xx_gpio_get_value(struct no_os_gpio_desc *desc, uint8_t *value)
{
	struct ftd2xx_gpio_desc *extra_desc;
	FT_STATUS status;
	int32_t ret;
	UCHAR cmd, val;
	DWORD bytesWritten, bytesRead;

	extra_desc = desc->extra;

	if (no_os_field_get(FTD2XX_GPIO_PIN(desc->number),
			    ftd2xx_gpio_pins_dir[desc->port]) == NO_OS_GPIO_IN) {
		cmd = FTD2XX_GPIO_READ_CMD(desc->port);
		status = FT_Write(extra_desc->ftHandle, &cmd, 1, &bytesWritten);
		if (status != FT_OK) {
			ret = status;
			return ret;
		}

		status = FT_Read(extra_desc->ftHandle, &val, 1, &bytesRead);
		if (status != FT_OK) {
			ret = status;
			return ret;
		}

		*value = no_os_field_get(FTD2XX_GPIO_PIN(desc->number), val);
	} else
		return -EINVAL;

	*value = no_os_field_get(FTD2XX_GPIO_PIN(desc->number),
				 ftd2xx_gpio_pins_dir[desc->port]);

	return 0;
}

/**
 * @brief ftd2xx platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops ftd2xx_gpio_ops = {
	.gpio_ops_get = &ftd2xx_gpio_get,
	.gpio_ops_get_optional = &ftd2xx_gpio_get_optional,
	.gpio_ops_remove = &ftd2xx_gpio_remove,
	.gpio_ops_direction_input = &ftd2xx_gpio_direction_input,
	.gpio_ops_direction_output = &ftd2xx_gpio_direction_output,
	.gpio_ops_get_direction = &ftd2xx_gpio_get_direction,
	.gpio_ops_set_value = &ftd2xx_gpio_set_value,
	.gpio_ops_get_value = &ftd2xx_gpio_get_value
};
