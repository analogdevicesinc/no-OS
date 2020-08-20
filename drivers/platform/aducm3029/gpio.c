/***************************************************************************//**
 *   @file   aducm3029/gpio.c
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

#include "error.h"
#include "gpio.h"
#include <drivers/gpio/adi_gpio.h>
#include <stdlib.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** Get GPIO pin from GPIO number */
#define PIN(nb)		(1u << ((nb) & 0x0F))
/** Get GPIO port from GPIO number */
#define PORT(nb)	(((nb) & 0xF0) >> 4)

/** Memory for GPIO device */
static uint8_t mem_gpio_handler[ADI_GPIO_MEMORY_SIZE];
/* Number of initialized devices */
static uint8_t nb_gpio = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO descriptor from the number specified in param
 * @param desc - Pointer to a structure were the descriptor will be stored.
 * @param param - Parameter describing the GPIO to be initialized
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t gpio_get(struct gpio_desc **desc, const struct gpio_init_param *param)
{
	if (!desc || !param)
		return FAILURE;

	(*desc) = calloc(1, sizeof(**desc));
	if (!(*desc))
		return FAILURE;

	(*desc)->number = param->number;
	/* If this is the first GPIO initialize GPIO controller */
	if (nb_gpio == 0)
		if (SUCCESS != adi_gpio_Init(mem_gpio_handler,
					     ADI_GPIO_MEMORY_SIZE)) {
			free(*desc);
			*desc = NULL;
			return FAILURE;
		}

	/* Increment number of GPIOs */
	nb_gpio++;

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
	if(param == NULL) {
		*desc = NULL;
		return SUCCESS;
	}

	return gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The GPIO descriptor.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t gpio_remove(struct gpio_desc *desc)
{
	if (!desc || !nb_gpio)
		return FAILURE;

	free(desc);
	/* Decrement number of GPIOs */
	nb_gpio--;
	/* If no more GPIOs free driver memory */
	if (nb_gpio == 0)
		return adi_gpio_UnInit();

	return SUCCESS;
}

/**
 * @brief Enable the input direction of the specified GPIO
 * @param desc - The GPIO descriptor.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t gpio_direction_input(struct gpio_desc *desc)
{
	if (!desc || !nb_gpio)
		return FAILURE;
	/* Enable input driver */
	if (ADI_GPIO_SUCCESS != adi_gpio_InputEnable(PORT(desc->number),
			PIN(desc->number), true))
		return FAILURE;

	return SUCCESS;
}

/**
 * @brief Enable the output direction of the specified GPIO and set the GPIO to
 * the specified value
 * @param desc - The GPIO descriptor.
 * @param value - The value. \ref GPIO_HIGH or \ref GPIO_LOW
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t gpio_direction_output(struct gpio_desc *desc, uint8_t value)
{
	ADI_GPIO_RESULT ret;

	if (!desc || !nb_gpio)
		return FAILURE;

	/* Enable output driver */
	ret = adi_gpio_OutputEnable(PORT(desc->number), PIN(desc->number),
				    true);
	if (ret != ADI_GPIO_SUCCESS)
		return FAILURE;

	/* Initialize pin with a value */
	if (value == 1)
		ret = adi_gpio_SetHigh(PORT(desc->number), PIN(desc->number));
	else
		ret = adi_gpio_SetLow(PORT(desc->number), PIN(desc->number));
	if (ret != ADI_GPIO_SUCCESS)
		return FAILURE;

	return SUCCESS;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - Variable where to store the direction. Will be set to \ref
 * GPIO_OUT or \ref GPIO_IN
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t gpio_get_direction(struct gpio_desc *desc, uint8_t *direction)
{
	uint16_t pins;

	if (!desc || !nb_gpio)
		return FAILURE;

	if (ADI_GPIO_SUCCESS != adi_gpio_GetOutputEnable(PORT(desc->number),
			&pins))
		return FAILURE;
	if (pins & PIN(desc->number))
		*direction = GPIO_OUT;
	else
		*direction = GPIO_IN;

	return SUCCESS;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value: GPIO_HIGH, GPIO_LOW or GPIO_HIGH_Z
 *                Choosing GPIO_HIGH_Z will deactivate the input and output
 *                buffers.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t gpio_set_value(struct gpio_desc *desc, uint8_t value)
{
	ADI_GPIO_RESULT ret;

	if (!desc || !nb_gpio)
		return FAILURE;

	switch (value) {
	case GPIO_LOW:
		ret = adi_gpio_SetLow(PORT(desc->number), PIN(desc->number));
		break;
	case GPIO_HIGH:
		ret = adi_gpio_SetHigh(PORT(desc->number), PIN(desc->number));
		break;
	case GPIO_HIGH_Z:
		ret = adi_gpio_OutputEnable(PORT(desc->number),
					    PIN(desc->number), false);
		if (ret != ADI_GPIO_SUCCESS)
			return FAILURE;

		ret = adi_gpio_InputEnable(PORT(desc->number),
					   PIN(desc->number), false);
		break;
	default:
		ret = adi_gpio_SetHigh(PORT(desc->number), PIN(desc->number));
		break;
	}
	if (ret != ADI_GPIO_SUCCESS)
		return FAILURE;

	return SUCCESS;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - Variable where to store the direction. Will be set to \ref
 * GPIO_HIGH or \ref GPIO_LOW
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t gpio_get_value(struct gpio_desc *desc, uint8_t *value)
{
	uint16_t pins;
	uint16_t port;

	if (!desc || !nb_gpio)
		return FAILURE;

	if (ADI_GPIO_SUCCESS != adi_gpio_GetOutputEnable(PORT(desc->number),
			&port))
		return FAILURE;

	if (port & PIN(desc->number)) {
		if (ADI_GPIO_SUCCESS != adi_gpio_GetOutputData(
			    PORT(desc->number), &port))
			return FAILURE;
		*value = !!(port & PIN(desc->number));
	} else {
		if (ADI_GPIO_SUCCESS != adi_gpio_GetData(PORT(desc->number),
				PIN(desc->number), &pins))
			return FAILURE;
		*value = !!pins;
	}

	return SUCCESS;
}
