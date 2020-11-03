/***************************************************************************//**
 *   @file   gpio.c
 *   @brief  Implementation of the GPIO Interface
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <inttypes.h>
#include "gpio.h"
#include <stdlib.h>
#include "error.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(struct gpio_desc **desc,
		 const struct gpio_init_param *param)
{
	if (!param)
		return FAILURE;

	if ((param->platform_ops->gpio_ops_get(desc, param)))
		return FAILURE;

	(*desc)->platform_ops = param->platform_ops;

	return SUCCESS;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_optional(struct gpio_desc **desc,
			  const struct gpio_init_param *param)
{
	if (!param || (param->number == -1)) {
		*desc = NULL;
		return SUCCESS;
	}

	if ((param->platform_ops->gpio_ops_get_optional(desc, param)))
		return FAILURE;

	(*desc)->platform_ops = param->platform_ops;

	return SUCCESS;
}
/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The GPIO descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_remove(struct gpio_desc *desc)
{
	if (desc)
		return desc->platform_ops->gpio_ops_remove(desc);
	else
		return SUCCESS;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_input(struct gpio_desc *desc)
{
	if (desc)
		return desc->platform_ops->gpio_ops_direction_input(desc);
	else
		return SUCCESS;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_output(struct gpio_desc *desc,
			      uint8_t value)
{
	if (desc)
		return desc->platform_ops->
			gpio_ops_direction_output(desc, value);
	else
		return SUCCESS;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: GPIO_OUT
 *                             GPIO_IN
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_direction(struct gpio_desc *desc,
			   uint8_t *direction)
{
	if (desc)
		return desc->platform_ops->
			gpio_ops_get_direction(desc, direction);
	else
		return SUCCESS;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_value(struct gpio_desc *desc,
		       uint8_t value)
{
	if (desc)
		return desc->platform_ops->gpio_ops_set_value(desc, value);
	else
		return SUCCESS;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_value(struct gpio_desc *desc,
		       uint8_t *value)
{
	if (desc)
		return desc->platform_ops->gpio_ops_get_value(desc, value);
	else
		return SUCCESS;
}
