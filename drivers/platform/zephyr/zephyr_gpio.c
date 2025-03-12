/*******************************************************************************
 *   @file   zephyr/zephyr_gpio.c
 *   @brief  Implementation of Zephyr platform GPIO Driver.
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

#include "zephyr_gpio.h"

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, -1 otherwise.
 */
int32_t zephyr_gpio_get(struct no_os_gpio_desc **desc,
			const struct no_os_gpio_init_param *param)
{
	struct zephyr_gpio_desc *xdevice;

	if (NO_OS_DRIVER_DYNAMIC_ALLOC) {
		(*desc) = (struct no_os_gpio_desc *)no_os_malloc(sizeof(*(*desc)));
		if (!(*desc)) {
			return -ENOMEM;
		}
		xdevice = no_os_calloc(1, sizeof(*xdevice));
		if (!xdevice) {
			no_os_free(*desc);
			return -ENOMEM;
		}
	} else {
		if (!(*desc))
			return -ENOMEM;
		if (!((*desc)->extra))
			return -ENOMEM;
		xdevice = (struct zephyr_gpio_desc *)((*desc)->extra);
	}
	xdevice->port = ((struct zephyr_gpio_init_param *)(param->extra))->port;
	xdevice->pin = ((struct zephyr_gpio_init_param *)(param->extra))->pin;
	xdevice->flags = ((struct zephyr_gpio_init_param *)(param->extra))->flags;
	(*desc)->extra = xdevice;
	(*desc)->platform_ops = &zephyr_gpio_ops;

	return 0;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t zephyr_gpio_get_optional(struct no_os_gpio_desc **desc,
				 const struct no_os_gpio_init_param *param)
{
	zephyr_gpio_get(desc, param);

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t zephyr_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (NO_OS_DRIVER_DYNAMIC_ALLOC) {
		if (!desc)
			return -EINVAL;
		if (!desc->extra)
			return -EINVAL;
		no_os_free(desc->extra);
		no_os_free(desc);
	}

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
int32_t zephyr_gpio_set_value(struct no_os_gpio_desc *desc,
			      uint8_t value)
{
	struct zephyr_gpio_desc *zephyr_desc;
	int ret;

	if (!desc || !desc->extra)
		return -EINVAL;

	zephyr_desc = (struct zephyr_gpio_desc *)(desc->extra);

	if (value == NO_OS_GPIO_HIGH) {
		ret = gpio_pin_configure(zephyr_desc->port, zephyr_desc->pin, GPIO_OUTPUT_HIGH);
		if (ret)
			return -1;
	} else if (value == NO_OS_GPIO_LOW) {
		ret = gpio_pin_configure(zephyr_desc->port, zephyr_desc->pin, GPIO_OUTPUT_LOW);
		if (ret)
			return -1;
	}
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
int32_t zephyr_gpio_get_value(struct no_os_gpio_desc *desc,
			      uint8_t *value)
{
	struct zephyr_gpio_desc *zephyr_desc;
	int ret;

	if (!desc || !desc->extra)
		return -EINVAL;

	zephyr_desc = (struct zephyr_gpio_desc *)(desc->extra);

	ret = gpio_pin_get(zephyr_desc->port, zephyr_desc->pin);
	if (ret < 0)
		return -1;
	else {
		if (ret == 0)
			*value = NO_OS_GPIO_LOW;
		else
			*value = NO_OS_GPIO_HIGH;
	}

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t zephyr_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	struct zephyr_gpio_desc *zephyr_desc;
	int ret;

	if (!desc || !desc->extra)
		return -EINVAL;

	zephyr_desc = (struct zephyr_gpio_desc *)(desc->extra);

	ret = gpio_pin_configure(zephyr_desc->port, zephyr_desc->pin,
				 zephyr_desc->flags | GPIO_INPUT);
	return ret;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t zephyr_gpio_direction_output(struct no_os_gpio_desc *desc,
				     uint8_t value)
{
	struct zephyr_gpio_desc *zephyr_desc;
	int ret;

	if (!desc || !desc->extra)
		return -EINVAL;

	zephyr_desc = (struct zephyr_gpio_desc *)(desc->extra);

	ret = gpio_pin_configure(zephyr_desc->port, zephyr_desc->pin,
				 zephyr_desc->flags | GPIO_OUTPUT);
	return ret;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, -1 otherwise.
 */
int32_t zephyr_gpio_get_direction(struct no_os_gpio_desc *desc,
				  uint8_t *direction)
{
#ifdef CONFIG_GPIO_GET_CONFIG
	struct zephyr_gpio_desc *zephyr_desc;
	int ret = 0;

	if (!desc || !desc->extra)
		return -EINVAL;

	zephyr_desc = (struct zephyr_gpio_desc *)(desc->extra);
	ret = gpio_pin_is_input(zephyr_desc->port, zephyr_desc->pin);

	if (ret == 1)
		*direction = NO_OS_GPIO_IN;
	else if (ret == 0)
		*direction = NO_OS_GPIO_OUT;
	else
		return -1;
	return 0;
#endif
	return -ENOTSUP;
}

/**
 * @brief Zeohyr platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops zephyr_gpio_ops = {
	.gpio_ops_get = &zephyr_gpio_get,
	.gpio_ops_get_optional = &zephyr_gpio_get_optional,
	.gpio_ops_remove = &zephyr_gpio_remove,
	.gpio_ops_direction_input = &zephyr_gpio_direction_input,
	.gpio_ops_direction_output = &zephyr_gpio_direction_output,
	.gpio_ops_get_direction = &zephyr_gpio_get_direction,
	.gpio_ops_set_value = &zephyr_gpio_set_value,
	.gpio_ops_get_value = &zephyr_gpio_get_value,
};
