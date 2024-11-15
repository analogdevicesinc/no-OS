/***************************************************************************//**
 *   @file   altera/altera_gpio.c
 *   @brief  Implementation of Altera GPIO Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include <altera_avalon_spi_regs.h>
#include "no_os_gpio.h"
#include "altera_gpio.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "parameters.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_gpio_get(struct no_os_gpio_desc **desc,
			const struct no_os_gpio_init_param *param)
{

	struct no_os_gpio_desc *descriptor;
	struct altera_gpio_desc *altera_descriptor;
	struct altera_gpio_init_param *altera_param;

	descriptor = no_os_calloc(1, sizeof(*descriptor));

	if (!descriptor)
		return -1;

	descriptor->extra = no_os_calloc(1, sizeof *altera_descriptor);
	if (!(descriptor->extra)) {
		no_os_free(descriptor);
		return -1;
	}

	altera_descriptor = descriptor->extra;
	altera_param = param->extra;

	descriptor->number = param->number;

	altera_descriptor->type = altera_param->type;
	altera_descriptor->device_id = altera_param->device_id;
	altera_descriptor->base_address = altera_param->base_address;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_gpio_get_optional(struct no_os_gpio_desc **desc,
				 const struct no_os_gpio_init_param *param)
{
	if(param == NULL) {
		*desc = NULL;
		return 0;
	}

	return no_os_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (desc) {
		no_os_free(desc->extra);
		no_os_free(desc);
	}

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

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
int32_t altera_gpio_direction_output(struct no_os_gpio_desc *desc,
				     uint8_t value)
{
	return no_os_gpio_set_value(desc, value);
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, -1 otherwise.
 */
int32_t altera_gpio_get_direction(struct no_os_gpio_desc *desc,
				  uint8_t *direction)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (direction) {
		// Unused variable - fix compiler warning
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
int32_t altera_gpio_set_value(struct no_os_gpio_desc *desc,
			      uint8_t value)
{
	if(!desc)
		return -1;

	uint32_t ppos;
	uint32_t pdata;
	uint32_t pmask;

	struct altera_gpio_desc *altera_desc;
	altera_desc = desc->extra;

	if (desc->number < 32)
		return -1;

	switch(altera_desc->type) {
	case NIOS_II_GPIO:
		ppos = desc->number - 32;
		pmask = 0x1 << ppos;

		pdata = IORD_32DIRECT(altera_desc->base_address, 0x0);
		IOWR_32DIRECT(altera_desc->base_address,
			      0x0, ((pdata & ~pmask) | (value << ppos)));

		break;
	default:
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
int32_t altera_gpio_get_value(struct no_os_gpio_desc *desc,
			      uint8_t *value)
{
	if(!desc)
		return -1;

	uint32_t ppos;
	uint32_t pdata;

	struct altera_gpio_desc *altera_desc;
	altera_desc = desc->extra;

	if (desc->number < 32)
		return -1;

	switch(altera_desc->type) {
	case NIOS_II_GPIO:
		ppos = desc->number - 32;

		pdata = IORD_32DIRECT(altera_desc->base_address, 0x0);
		*value = (pdata >> ppos) & 0x01;

		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief Altera platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops altera_gpio_ops = {
	.gpio_ops_get = &altera_gpio_get,
	.gpio_ops_get_optional = &altera_gpio_get_optional,
	.gpio_ops_remove = &altera_gpio_remove,
	.gpio_ops_direction_input = &altera_gpio_direction_input,
	.gpio_ops_direction_output = &altera_gpio_direction_output,
	.gpio_ops_get_direction = &altera_gpio_get_direction,
	.gpio_ops_set_value = &altera_gpio_set_value,
	.gpio_ops_get_value = &altera_gpio_get_value,
};
