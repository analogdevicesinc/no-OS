/***************************************************************************//**
 *   @file   mbed_gpio.cpp
 *   @brief  Implementation of GPIO Mbed platform driver interfaces
********************************************************************************
 * Copyright (c) 2019-2022 Analog Devices, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <mbed.h>

// Platform drivers needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif //  _cplusplus

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "mbed_gpio.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO descriptor.
 * @param desc[in,out] - The GPIO descriptor.
 * @param param[in] - The structure that contains the GPIO init parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_get(struct no_os_gpio_desc **desc,
		      const struct no_os_gpio_init_param *param)
{
	struct no_os_gpio_desc *gpio_desc;
	struct mbed_gpio_desc *mbed_gpio_desc;

	if (!desc || !param)
		return -EINVAL;

	/* Create gpio description object for the device */
	gpio_desc = (struct no_os_gpio_desc *)calloc(1, sizeof(*gpio_desc));
	if (!gpio_desc)
		return -ENOMEM;

	gpio_desc->number = param->number;

	/* Create the gpio extra descriptor object to store extra mbed gpio info */
	mbed_gpio_desc = (struct mbed_gpio_desc *)calloc(1,
			 sizeof(*mbed_gpio_desc));
	if (!mbed_gpio_desc)
		goto err_mbed_gpio_desc;

	mbed_gpio_desc->direction = NO_OS_GPIO_IN;
	mbed_gpio_desc->gpio_pin = NULL;

	if (param->extra)
		mbed_gpio_desc->pin_mode = ((struct mbed_gpio_init_param *)
					    param->extra)->pin_mode;
	else
		mbed_gpio_desc->pin_mode = NULL;

	gpio_desc->extra = mbed_gpio_desc;
	*desc = gpio_desc;

	return 0;

err_mbed_gpio_desc:
	free(gpio_desc);

	return -ENOMEM;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc[in,out] - The GPIO descriptor.
 * @param param[in] - GPIO Initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_get_optional(struct no_os_gpio_desc **desc,
			       const struct no_os_gpio_init_param *param)
{
	if (param == NULL) {
		*desc = NULL;
		return 0;
	}

	return mbed_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by mbed_gpio_get().
 * @param desc[in] - The GPIO descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	/* Free the gpio object */
	if (((struct mbed_gpio_desc *)(desc->extra))->gpio_pin) {
		if (((struct mbed_gpio_desc *)(desc->extra))->direction == NO_OS_GPIO_IN)
			delete((DigitalIn *)(((struct mbed_gpio_desc *)(desc->extra))->gpio_pin));
		else
			delete((DigitalOut *)(((struct mbed_gpio_desc *)(desc->extra))->gpio_pin));
	}

	/* Free the gpio descriptor objects */
	free(desc->extra);
	free(desc);

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc[in] - The GPIO descriptor.
 * @return 0 in case of success, negative error code otherwise.
 * @note does not support reconfiguration of already set pin direction
 */
int32_t mbed_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	DigitalIn *gpio_input;				// pointer to gpio input object
	struct mbed_gpio_desc
		*gpio_desc_extra; 	// pointer to gpio desc extra parameters

	if (!desc || !desc->extra)
		return -EINVAL;

	gpio_desc_extra = (struct mbed_gpio_desc *)(desc->extra);
	if (gpio_desc_extra->gpio_pin != NULL)
		return -EINVAL;

	/* Configure and instantiate GPIO pin as input */
	gpio_input = new DigitalIn((PinName)desc->number);
	if (!gpio_input)
		return -ENOMEM;

	if (!gpio_input->is_connected()) {
		delete(gpio_input);
		return -EIO;
	}

	gpio_desc_extra->gpio_pin = (struct mbed_gpio_desc *)gpio_input;
	gpio_desc_extra->direction = NO_OS_GPIO_IN;

	/* Set the gpio pin mode */
	gpio_input->mode((PinMode)gpio_desc_extra->pin_mode);

	return 0;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc[in] - The GPIO descriptor.
 * @param value[in] - The GPIO value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, negative error code otherwise.
 * @note does not support reconfiguration of already set pin direction
 */
int32_t mbed_gpio_direction_output(struct no_os_gpio_desc *desc, uint8_t value)
{
	DigitalOut *gpio_output;    	// pointer to gpio output object
	struct mbed_gpio_desc
		*gpio_desc_extra;   // pointer to gpio desc extra parameters

	if (!desc || !desc->extra)
		return -EINVAL;

	gpio_desc_extra = (struct mbed_gpio_desc *)(desc->extra);
	if (gpio_desc_extra->gpio_pin != NULL)
		return -EINVAL;

	/* Configure and instantiate GPIO pin as output */
	gpio_output = new DigitalOut((PinName)desc->number);
	if (!gpio_output)
		return -ENOMEM;

	if (!gpio_output->is_connected()) {
		delete(gpio_output);
		return -EIO;
	}

	gpio_desc_extra->gpio_pin = (struct mbed_gpio_desc *)gpio_output;
	gpio_desc_extra->direction = NO_OS_GPIO_OUT;

	/* Set the GPIO value */
	gpio_output->write(value);

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc[in] - The GPIO descriptor.
 * @param direction[in,out] - The GPIO direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_get_direction(struct no_os_gpio_desc *desc, uint8_t *direction)
{
	mbed_gpio_desc *gpio_desc_extra;     // pointer to gpio desc extra parameters

	if (!desc || !desc->extra || !direction)
		return -EINVAL;

	gpio_desc_extra = (struct mbed_gpio_desc *)(desc->extra);
	if (!gpio_desc_extra->gpio_pin)
		return -EINVAL;

	*direction = gpio_desc_extra->direction;

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc[in] - The GPIO descriptor.
 * @param value[in] - The GPIO value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_set_value(struct no_os_gpio_desc *desc, uint8_t value)
{
	DigitalOut *gpio_output; 		// pointer to gpio output object
	mbed_gpio_desc *gpio_desc_extra;    // pointer to gpio desc extra parameters

	if (!desc || !desc->extra)
		return -EINVAL;

	gpio_desc_extra = (struct mbed_gpio_desc *)(desc->extra);
	if (!gpio_desc_extra->gpio_pin || (gpio_desc_extra->direction != NO_OS_GPIO_OUT))
		return -EINVAL;

	gpio_output = (DigitalOut *)(gpio_desc_extra->gpio_pin);
	if (!gpio_output->is_connected())
		return -EINVAL;

	gpio_output->write(value);

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc[in] - The GPIO descriptor.
 * @param value[in,out] - The GPIO value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_get_value(struct no_os_gpio_desc *desc, uint8_t *value)
{
	DigitalIn *gpio_input;    	// pointer to gpio input object

	if (!desc || !desc->extra || !value)
		return -EINVAL;

	gpio_input = (DigitalIn *)((struct mbed_gpio_desc *)desc->extra)->gpio_pin;
	if (!gpio_input->is_connected())
		return -EINVAL;

	*value = (uint8_t)gpio_input->read();

	return 0;
}

/**
 * @brief Mbed platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops mbed_gpio_ops = {
	.gpio_ops_get = &mbed_gpio_get,
	.gpio_ops_get_optional = &mbed_gpio_get_optional,
	.gpio_ops_remove = &mbed_gpio_remove,
	.gpio_ops_direction_input = &mbed_gpio_direction_input,
	.gpio_ops_direction_output = &mbed_gpio_direction_output,
	.gpio_ops_get_direction = &mbed_gpio_get_direction,
	.gpio_ops_set_value = &mbed_gpio_set_value,
	.gpio_ops_get_value = &mbed_gpio_get_value,
};

#ifdef __cplusplus
}
#endif //  _cplusplus
