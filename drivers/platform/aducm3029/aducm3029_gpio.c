/***************************************************************************//**
 *   @file   aducm3029/aducm3029_gpio.c
 *   @brief  Implementation of GPIO driver for ADuCM302x
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_alloc.h"
#include <drivers/gpio/adi_gpio.h>
#include <stdlib.h>
#include "aducm3029_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** Memory for GPIO device */
static uint8_t mem_gpio_handler[ADI_GPIO_MEMORY_SIZE];
/* Number of initialized devices */
static uint8_t nb_gpio = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO descriptor from the number specified in param.
 * @param desc - Pointer to a structure were the descriptor will be stored.
 * @param param - Parameter describing the GPIO to be initialized
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_gpio_get(struct no_os_gpio_desc **desc,
			   const struct no_os_gpio_init_param *param)
{
	if (!desc || !param)
		return -1;

	(*desc) = no_os_calloc(1, sizeof(**desc));
	if (!(*desc))
		return -1;

	(*desc)->number = param->number;
	/* If this is the first GPIO initialize GPIO controller */
	if (nb_gpio == 0) {
		/** Deinitialize GPIO driver in case it was initialized by the
		 *  GPIO IRQ driver */
		adi_gpio_UnInit();
		if (0 != adi_gpio_Init(mem_gpio_handler,
				       ADI_GPIO_MEMORY_SIZE)) {
			no_os_free(*desc);
			*desc = NULL;
			return -1;
		}

		/* Disable the group interrupts, they are enabled in adi_gpio_Init */
		NVIC_DisableIRQ(SYS_GPIO_INTA_IRQn);
		NVIC_DisableIRQ(SYS_GPIO_INTB_IRQn);
	}

	/* Increment number of GPIOs */
	nb_gpio++;

	return 0;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_gpio_get_optional(struct no_os_gpio_desc **desc,
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
int32_t aducm3029_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (!desc || !nb_gpio)
		return -1;

	no_os_free(desc);
	/* Decrement number of GPIOs */
	nb_gpio--;
	/* If no more GPIOs free driver memory */
	if (nb_gpio == 0)
		return adi_gpio_UnInit();

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	if (!desc || !nb_gpio)
		return -1;
	/* Enable input driver */
	if (ADI_GPIO_SUCCESS != adi_gpio_InputEnable(PORT(desc->number),
			PIN(desc->number), true))
		return -1;

	return 0;
}

/**
 * @brief Enable the output direction of the specified GPIO and set the GPIO to
 * the specified value
 * @param desc - The GPIO descriptor.
 * @param value - The value. \ref NO_OS_GPIO_HIGH or \ref NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_gpio_direction_output(struct no_os_gpio_desc *desc,
					uint8_t value)
{
	ADI_GPIO_RESULT ret;

	if (!desc || !nb_gpio)
		return -1;

	/* Enable output driver */
	ret = adi_gpio_OutputEnable(PORT(desc->number), PIN(desc->number),
				    true);
	if (ret != ADI_GPIO_SUCCESS)
		return -1;

	/* Initialize pin with a value */
	if (value == 1)
		ret = adi_gpio_SetHigh(PORT(desc->number), PIN(desc->number));
	else
		ret = adi_gpio_SetLow(PORT(desc->number), PIN(desc->number));
	if (ret != ADI_GPIO_SUCCESS)
		return -1;

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - Variable where to store the direction. Will be set to \ref
 * NO_OS_GPIO_OUT or \ref NO_OS_GPIO_IN
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_gpio_get_direction(struct no_os_gpio_desc *desc,
				     uint8_t *direction)
{
	uint16_t pins;

	if (!desc || !nb_gpio)
		return -1;

	if (ADI_GPIO_SUCCESS != adi_gpio_GetOutputEnable(PORT(desc->number),
			&pins))
		return -1;
	if (pins & PIN(desc->number))
		*direction = NO_OS_GPIO_OUT;
	else
		*direction = NO_OS_GPIO_IN;

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value: NO_OS_GPIO_HIGH, NO_OS_GPIO_LOW or NO_OS_GPIO_HIGH_Z
 *                Choosing NO_OS_GPIO_HIGH_Z will deactivate the input and output
 *                buffers.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_gpio_set_value(struct no_os_gpio_desc *desc, uint8_t value)
{
	ADI_GPIO_RESULT ret;

	if (!desc || !nb_gpio)
		return -1;

	switch (value) {
	case NO_OS_GPIO_LOW:
		ret = adi_gpio_SetLow(PORT(desc->number), PIN(desc->number));
		break;
	case NO_OS_GPIO_HIGH:
		ret = adi_gpio_SetHigh(PORT(desc->number), PIN(desc->number));
		break;
	case NO_OS_GPIO_HIGH_Z:
		ret = adi_gpio_OutputEnable(PORT(desc->number),
					    PIN(desc->number), false);
		if (ret != ADI_GPIO_SUCCESS)
			return -1;

		ret = adi_gpio_InputEnable(PORT(desc->number),
					   PIN(desc->number), false);
		break;
	default:
		ret = adi_gpio_SetHigh(PORT(desc->number), PIN(desc->number));
		break;
	}
	if (ret != ADI_GPIO_SUCCESS)
		return -1;

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - Variable where to store the direction. Will be set to \ref
 * NO_OS_GPIO_HIGH or \ref NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_gpio_get_value(struct no_os_gpio_desc *desc,
				 uint8_t *value)
{
	uint16_t pins;
	uint16_t port;

	if (!desc || !nb_gpio)
		return -1;

	if (ADI_GPIO_SUCCESS != adi_gpio_GetOutputEnable(PORT(desc->number),
			&port))
		return -1;

	if (port & PIN(desc->number)) {
		if (ADI_GPIO_SUCCESS != adi_gpio_GetOutputData(
			    PORT(desc->number), &port))
			return -1;
		*value = !!(port & PIN(desc->number));
	} else {
		if (ADI_GPIO_SUCCESS != adi_gpio_GetData(PORT(desc->number),
				PIN(desc->number), &pins))
			return -1;
		*value = !!pins;
	}

	return 0;
}

/**
 * @brief ADuCM3029 platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops aducm_gpio_ops = {
	.gpio_ops_get = &aducm3029_gpio_get,
	.gpio_ops_get_optional = &aducm3029_gpio_get_optional,
	.gpio_ops_remove = &aducm3029_gpio_remove,
	.gpio_ops_direction_input = &aducm3029_gpio_direction_input,
	.gpio_ops_direction_output = &aducm3029_gpio_direction_output,
	.gpio_ops_get_direction = &aducm3029_gpio_get_direction,
	.gpio_ops_set_value = &aducm3029_gpio_set_value,
	.gpio_ops_get_value = &aducm3029_gpio_get_value
};
