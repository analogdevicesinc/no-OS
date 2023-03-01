/***************************************************************************//**
 *   @file   pico/pico_gpio.c
 *   @brief  Implementation of pico gpio functionality.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
/************************* Include Files **************************************/
/******************************************************************************/

#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "pico_gpio.h"
#include "hardware/gpio.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Prepare the GPIO descriptor.
 * @param desc  - The GPIO descriptor.
 * @param param - The structure that contains the GPIO parameters.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t _gpio_init(struct no_os_gpio_desc *desc,
			  const struct no_os_gpio_init_param *param)
{
	bool up;
	bool down;

	if (!param)
		return -EINVAL;

	desc->number = param->number;
	gpio_init(desc->number);

	switch(param->pull) {
	case NO_OS_PULL_NONE:
		up = false;
		down = false;
		break;
	case NO_OS_PULL_UP:
		up = true;
		down = false;
		break;
	case NO_OS_PULL_DOWN:
		up = false;
		down = true;
		break;
	default:
		return -EINVAL;
	}

	desc->pull = param->pull;
	gpio_set_pulls(desc->number, up, down);

	return 0;
}

/**
 * @brief Obtain the GPIO descriptor.
 * @param desc  - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_gpio_get(struct no_os_gpio_desc **desc,
		      const struct no_os_gpio_init_param *param)
{
	struct no_os_gpio_desc *descriptor;
	struct pico_gpio_desc *extra;
	int32_t ret;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct no_os_gpio_desc *)no_os_malloc(sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	extra = (struct pico_gpio_desc*)no_os_malloc(sizeof(*extra));
	if (!extra) {
		ret = -ENOMEM;
		goto error;
	}

	descriptor->extra = extra;
	ret = _gpio_init(descriptor, param);
	if(ret)
		goto error;

	*desc = descriptor;

	return 0;
error:
	no_os_free(extra);
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc  - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_gpio_get_optional(struct no_os_gpio_desc **desc,
			       const struct no_os_gpio_init_param *param)
{
	if(param == NULL) {
		*desc = NULL;
		return 0;
	}

	return pico_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0.
 */
int32_t pico_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (desc != NULL)
		no_os_free(desc->extra);

	no_os_free(desc);

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	struct pico_gpio_desc *pico_gpio;

	if (!desc || !desc->extra)
		return -EINVAL;

	pico_gpio = desc->extra;

	gpio_set_dir(desc->number, false);
	pico_gpio->input_enabled = true;

	return 0;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc  - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_gpio_direction_output(struct no_os_gpio_desc *desc,
				   uint8_t value)
{
	struct pico_gpio_desc *pico_gpio;

	if (!desc || !desc->extra)
		return -EINVAL;

	pico_gpio = desc->extra;

	gpio_set_dir(desc->number, true);
	pico_gpio->input_enabled = false;

	gpio_put(desc->number, value == NO_OS_GPIO_HIGH ? true : false);

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc      - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_gpio_get_direction(struct no_os_gpio_desc *desc,
				uint8_t *direction)
{
	struct pico_gpio_desc *pico_gpio;

	if (!desc || !desc->extra || !direction)
		return -EINVAL;

	pico_gpio = desc->extra;
	*direction = (pico_gpio->input_enabled) ? NO_OS_GPIO_IN : NO_OS_GPIO_OUT;

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc  - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_gpio_set_value(struct no_os_gpio_desc *desc,
			    uint8_t value)
{
	if (!desc)
		return -EINVAL;

	gpio_put(desc->number, value == NO_OS_GPIO_HIGH ? true : false);

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc  - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_gpio_get_value(struct no_os_gpio_desc *desc,
			    uint8_t *value)
{
	if (!desc || !value)
		return -EINVAL;

	*value = gpio_get(desc->number);

	return 0;
}

/**
 * @brief pico platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops pico_gpio_ops = {
	.gpio_ops_get = &pico_gpio_get,
	.gpio_ops_get_optional = &pico_gpio_get_optional,
	.gpio_ops_remove = &pico_gpio_remove,
	.gpio_ops_direction_input = &pico_gpio_direction_input,
	.gpio_ops_direction_output = &pico_gpio_direction_output,
	.gpio_ops_get_direction = &pico_gpio_get_direction,
	.gpio_ops_set_value = &pico_gpio_set_value,
	.gpio_ops_get_value = &pico_gpio_get_value,
};
