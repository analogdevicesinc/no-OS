/***************************************************************************//**
 *   @file   gpio.c
 *   @brief  Implementation of Xilinx GPIO Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include "error.h"
#include "gpio.h"
#include "xilinx_platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param gpio_number - The number of the GPIO.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(struct gpio_desc **desc,
		 uint8_t gpio_number)
{
	gpio_desc *descriptor;
	xil_gpio_desc *xil_descriptor;
	int32_t ret;

	descriptor = calloc(1, sizeof *descriptor);
	if (!descriptor)
		return FAILURE;

	descriptor->extra = calloc(1, sizeof *xil_descriptor);
	if (!(descriptor->extra)) {
		free(descriptor);
		return FAILURE;
	}

	xil_descriptor = descriptor->extra;

#ifdef _XPARAMETERS_PS_H_
	xil_descriptor->config = XGpioPs_LookupConfig(0);
	if (xil_descriptor->config == NULL)
		goto error;

	ret = XGpioPs_CfgInitialize(&xil_descriptor->instance,
				    xil_descriptor->config, xil_descriptor->config->BaseAddr);
	if (ret != 0)
		goto error;
#else
	ret = XGpio_Initialize(&xil_descriptor->instance, 0);
	if (ret != 0)
		goto error;
#endif

	descriptor->number = gpio_number;

	*desc = descriptor;

	return SUCCESS;

error:
	free(descriptor);

	return FAILURE;
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_remove(struct gpio_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_input(struct gpio_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

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
	xil_gpio_desc *xil_desc;
	xil_desc = desc->extra;

#ifdef _XPARAMETERS_PS_H_
	XGpioPs_SetDirectionPin(&xil_desc->instance, desc->number, 1);

	XGpioPs_SetOutputEnablePin(&xil_desc->instance, desc->number, 1);

	XGpioPs_WritePin(&xil_desc->instance, desc->number, value);
#else
	uint8_t pin = desc->number;
	uint8_t channel;
	uint32_t reg_val;

	if (pin >= 32) {
		channel = 2;
		pin -= 32;
	} else
		channel = 1;

	reg_val = XGpio_GetDataDirection(&xil_desc->instance, channel);
	reg_val &= ~(1 << pin);
	XGpio_SetDataDirection(&xil_desc->instance, channel, reg_val);

	reg_val = XGpio_DiscreteRead(&xil_desc->instance, channel);
	if(value)
		reg_val |= (1 << pin);
	else
		reg_val &= ~(1 << pin);
	XGpio_DiscreteWrite(&xil_desc->instance, channel, reg_val);
#endif

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
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (direction) {
		// Unused variable - fix compiler warning
	}

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
	xil_gpio_desc *xil_desc;
	xil_desc = desc->extra;

#ifdef _XPARAMETERS_PS_H_
	XGpioPs_WritePin(&xil_desc->instance, desc->number, value);
#else
	uint8_t pin = desc->number;
	uint8_t channel;
	uint32_t reg_val;

	if (pin >= 32) {
		channel = 2;
		pin -= 32;
	} else
		channel = 1;

	reg_val = XGpio_DiscreteRead(&xil_desc->instance, channel);
	if(value)
		reg_val |= (1 << pin);
	else
		reg_val &= ~(1 << pin);
	XGpio_DiscreteWrite(&xil_desc->instance, channel, reg_val);
#endif

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
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (value) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}
