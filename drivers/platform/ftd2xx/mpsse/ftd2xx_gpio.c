/*******************************************************************************
 *   @file   ftd2xx_gpio.c
 *   @brief  Implementation of GPIO driver for ftd2xx platform.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "ftd2xx_gpio.h"
#include "libmpsse_i2c.h"
#include "ftd2xx_platform.h"

static uint8_t ftd2xx_gpio_pins_dir[FTD2XX_MAX_PORT_NB] = { 0 };
static uint8_t ftd2xx_gpio_pins_val[FTD2XX_MAX_PORT_NB] = { 0 };

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

	if (ftHandle[param->port] == NULL) {
		Init_libMPSSE();
		status = FT_Open(param->port, &extra_desc->ftHandle);
		if (status != FT_OK) {
			ret = status;
			goto free_extra;
		}
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

	extra_desc = desc->extra;

	dir = ftd2xx_gpio_pins_dir[desc->port];
	dir &= ~NO_OS_BIT(desc->number);
	dir |= NO_OS_BIT(desc->number) & no_os_field_prep(NO_OS_BIT(desc->number), 0);
	status = FT_WriteGPIO(extra_desc->ftHandle, (UCHAR)dir,
			      ftd2xx_gpio_pins_val[desc->port]);
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

	extra_desc = desc->extra;
	dir = ftd2xx_gpio_pins_dir[desc->port];
	dir &= ~NO_OS_BIT(desc->number);
	dir |= NO_OS_BIT(desc->number) & no_os_field_prep(NO_OS_BIT(desc->number), 1);
	val = ftd2xx_gpio_pins_val[desc->port];
	val &= ~NO_OS_BIT(desc->number);
	val |= NO_OS_BIT(desc->number) & no_os_field_prep(NO_OS_BIT(desc->number),
			value);
	status = FT_WriteGPIO(extra_desc->ftHandle, (UCHAR)dir, (UCHAR)val);
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

	if (no_os_field_get(FTD2XX_GPIO_PIN(desc->number),
			    ftd2xx_gpio_pins_dir[desc->port]) == 0)
		return -EINVAL;

	extra_desc = desc->extra;

	val = ftd2xx_gpio_pins_val[desc->port];
	val &= ~NO_OS_BIT(desc->number);
	val |= NO_OS_BIT(desc->number) & no_os_field_prep(NO_OS_BIT(desc->number),
			value);
	status = FT_WriteGPIO(extra_desc->ftHandle, ftd2xx_gpio_pins_dir[desc->port],
			      val);
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
	UCHAR val = 0;
	int32_t ret;

	extra_desc = desc->extra;
	if (no_os_field_get(FTD2XX_GPIO_PIN(desc->number),
			    ftd2xx_gpio_pins_dir[desc->port]) == NO_OS_GPIO_IN) {
		status = FT_ReadGPIO(extra_desc->ftHandle, &val);
		if (status != FT_OK) {
			ret = status;
			return ret;
		}

		*value = no_os_field_get(FTD2XX_GPIO_PIN(desc->number), val);
	}

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
