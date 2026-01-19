/***************************************************************************//**
 *   @file   lattice/riscvrx/lattice_gpio.c
 *   @brief  Implementation of lattice GPIO Generic Driver.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>

// Lattice BSP GPIO driver
#include "gpio.h"
#include "sys_platform.h"

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_alloc.h"
#include "riscvrx_gpio.h"

/******************************************************************************/
/************************ Forward Declarations ********************************/
/******************************************************************************/

static int32_t lattice_gpio_set_value(struct no_os_gpio_desc *desc,
				      uint8_t value);
static int32_t lattice_gpio_get_value(struct no_os_gpio_desc *desc,
				      uint8_t *value);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO descriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t lattice_gpio_get(struct no_os_gpio_desc **desc,
			 const struct no_os_gpio_init_param *param)
{
	struct no_os_gpio_desc *descriptor;
	struct lattice_gpio_desc *lattice_descriptor;
	struct lattice_gpio_init_param *lattice_param;
	struct gpio_instance *gpio_inst;

	if (!param || !desc)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->extra = no_os_calloc(1, sizeof(struct lattice_gpio_desc));
	if (!descriptor->extra) {
		no_os_free(descriptor);
		return -ENOMEM;
	}

	lattice_descriptor = descriptor->extra;
	lattice_param = param->extra;

	descriptor->number = param->number;

	// Allocate and initialize the BSP GPIO instance
	gpio_inst = no_os_calloc(1, sizeof(struct gpio_instance));
	if (!gpio_inst) {
		no_os_free(descriptor->extra);
		no_os_free(descriptor);
		return -ENOMEM;
	}

	// Initialize BSP GPIO with the provided parameters
	if (gpio_init(gpio_inst, lattice_param->base_address, 
		      lattice_param->lines_num, lattice_param->gpio_dirs) != 0) {
		no_os_free(gpio_inst);
		no_os_free(descriptor->extra);
		no_os_free(descriptor);
		return -EIO;
	}

	lattice_descriptor->instance = gpio_inst;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t lattice_gpio_get_optional(struct no_os_gpio_desc **desc,
				  const struct no_os_gpio_init_param *param)
{
	if (param == NULL) {
		*desc = NULL;
		return 0;
	}

	return lattice_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t lattice_gpio_remove(struct no_os_gpio_desc *desc)
{
	struct lattice_gpio_desc *lattice_desc;

	if (!desc)
		return 0;

	if (desc->extra) {
		lattice_desc = desc->extra;
		if (lattice_desc->instance)
			no_os_free(lattice_desc->instance);
		no_os_free(desc->extra);
	}

	no_os_free(desc);

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t lattice_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	struct lattice_gpio_desc *lattice_desc;
	struct gpio_instance *gpio_inst;

	if (!desc || !desc->extra)
		return -EINVAL;

	lattice_desc = desc->extra;
	gpio_inst = lattice_desc->instance;

	if (!gpio_inst)
		return -EINVAL;

	if (gpio_set_direction(gpio_inst, desc->number, GPIO_INPUT) != 0)
		return -EIO;

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
int32_t lattice_gpio_direction_output(struct no_os_gpio_desc *desc,
				      uint8_t value)
{
	struct lattice_gpio_desc *lattice_desc;
	struct gpio_instance *gpio_inst;
	int32_t ret;

	if (!desc || !desc->extra)
		return -EINVAL;

	lattice_desc = desc->extra;
	gpio_inst = lattice_desc->instance;

	if (!gpio_inst)
		return -EINVAL;

	if (gpio_set_direction(gpio_inst, desc->number, GPIO_OUTPUT) != 0)
		return -EIO;

	ret = lattice_gpio_set_value(desc, value);

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
int32_t lattice_gpio_get_direction(struct no_os_gpio_desc *desc,
				   uint8_t *direction)
{
	// The BSP GPIO driver doesn't provide a way to read back direction
	// unless _DIRECTION_INTERNAL_MEMORY_USE_ is defined
	// For now, return not supported
	if (!desc || !direction)
		return -EINVAL;

	return -ENOSYS;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t lattice_gpio_set_value(struct no_os_gpio_desc *desc,
			       uint8_t value)
{
	struct lattice_gpio_desc *lattice_desc;
	struct gpio_instance *gpio_inst;
	uint32_t gpio_value;

	if (!desc || !desc->extra)
		return -EINVAL;

	lattice_desc = desc->extra;
	gpio_inst = lattice_desc->instance;

	if (!gpio_inst)
		return -EINVAL;

	// Convert NO_OS_GPIO_HIGH/LOW to bit value
	gpio_value = (value == NO_OS_GPIO_HIGH) ? (1 << desc->number) : 0;

	if (gpio_output_write(gpio_inst, desc->number, gpio_value) != 0)
		return -EIO;

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
int32_t lattice_gpio_get_value(struct no_os_gpio_desc *desc,
			       uint8_t *value)
{
	struct lattice_gpio_desc *lattice_desc;
	struct gpio_instance *gpio_inst;
	uint32_t gpio_data;

	if (!desc || !desc->extra || !value)
		return -EINVAL;

	lattice_desc = desc->extra;
	gpio_inst = lattice_desc->instance;

	if (!gpio_inst)
		return -EINVAL;

	if (gpio_input_get(gpio_inst, desc->number, &gpio_data) != 0)
		return -EIO;

	// Extract the bit value for the specific pin
	*value = ((gpio_data >> desc->number) & 0x01) ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW;

	return 0;
}

/**
 * @brief lattice platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops lattice_gpio_ops = {
	.gpio_ops_get = &lattice_gpio_get,
	.gpio_ops_get_optional = &lattice_gpio_get_optional,
	.gpio_ops_remove = &lattice_gpio_remove,
	.gpio_ops_direction_input = &lattice_gpio_direction_input,
	.gpio_ops_direction_output = &lattice_gpio_direction_output,
	.gpio_ops_get_direction = &lattice_gpio_get_direction,
	.gpio_ops_set_value = &lattice_gpio_set_value,
	.gpio_ops_get_value = &lattice_gpio_get_value,
};
