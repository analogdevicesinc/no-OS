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

#include <xparameters.h>
#ifdef XPAR_XGPIO_NUM_INSTANCES
#include <xgpio.h>
#endif
#ifdef XPAR_XGPIOPS_NUM_INSTANCES
#include <xgpiops.h>
#endif

#include "error.h"
#include "gpio.h"
#include "gpio_extra.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Prepare the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param init_param - The structure that contains the GPIO parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t _gpio_init(struct gpio_desc *desc,
		   struct gpio_init_param *init_param)
{
	int32_t				ret;
	struct xil_gpio_desc		*xdesc;
	struct xil_gpio_init_param	*xinit;

	ret = FAILURE;

	xdesc = desc->extra;
	xinit = init_param->extra;

	xdesc->type = xinit->type;
	desc->number = init_param->number;

	switch (xinit->type) {
	case GPIO_PL:
#ifdef XGPIO_H
		xdesc->instance = (XGpio *)malloc(sizeof(XGpio));
		if(!xdesc->instance)
			goto pl_error;

		xdesc->config = XGpio_LookupConfig(xinit->device_id);
		if(xdesc->config == NULL)
			goto pl_error;

		ret = XGpio_CfgInitialize(xdesc->instance,
					  xdesc->config,
					  ((XGpio_Config*)xdesc->config)
					  ->BaseAddress);
		if(ret < 0)
			goto pl_error;

		break;
pl_error:
		free(xdesc->instance);

		break;
#endif
		goto error;
	case GPIO_PS:
#ifdef XGPIOPS_H
		xdesc->instance = (XGpioPs *)malloc(sizeof(XGpioPs));
		if(!xdesc->instance)
			goto ps_error;

		xdesc->config = XGpioPs_LookupConfig(xinit->device_id);
		if(xdesc->config == NULL)
			goto ps_error;

		ret = XGpioPs_CfgInitialize(xdesc->instance,
					    xdesc->config,
					    ((XGpioPs_Config*)xdesc->config)->BaseAddr);
		if(ret < 0)
			goto ps_error;
		break;
ps_error:
		free(xdesc->instance);
#endif
		goto error;
error:
	default:
		return FAILURE;
		break;
	}

	return SUCCESS;
}

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param gpio_number - The number of the GPIO.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(struct gpio_desc **desc,
		 const struct gpio_init_param *init_param)
{
	struct gpio_desc	*descriptor;
	struct xil_gpio_desc	*extra;
	int32_t			ret;

	descriptor = (struct gpio_desc *)malloc(sizeof(*descriptor));
	extra = (struct xil_gpio_desc*)malloc(sizeof(*extra));

	if (!descriptor || !extra)
		return FAILURE;

	descriptor->extra = extra;
	ret = _gpio_init(descriptor, init_param);

	if(ret != SUCCESS)
		goto error;

	*desc = descriptor;

	return SUCCESS;
error:
	free(extra);
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
	if (desc != NULL) {
		free(((xil_gpio_desc *)(desc->extra))->instance);
		free(desc->extra);
		free(desc);
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
	struct xil_gpio_desc	*extra;
	uint32_t channel = 1;
	uint32_t config	 = 0;

	extra = desc->extra;

	switch (extra->type) {
	case GPIO_PL:
#ifdef XGPIO_H
		/* We assume that pin 32 is the first pin from channel 2 */
		if (desc->number >= 32) {
			channel = 2;
			desc->number -= 32;
		}

		config = XGpio_GetDataDirection((XGpio *)extra->instance,
						channel);
		config &= ~(1 << desc->number);
		XGpio_SetDataDirection((XGpio *)extra->instance,
				       channel,
				       config);
#endif
		break;
	case GPIO_PS:
#ifdef XGPIOPS_H
		XGpioPs_SetDirectionPin(extra->instance,
					desc->number,
					GPIO_IN);
		XGpioPs_SetOutputEnablePin(extra->instance,
					   desc->number,
					   GPIO_OUT);
#endif
		break;
	default:
		return FALSE;
		break;
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
	struct xil_gpio_desc	*extra;
	uint8_t pin = desc->number;
	uint8_t channel;
	uint32_t reg_val;

	extra = desc->extra;

	switch (extra->type) {
	case GPIO_PL:
#ifdef XGPIO_H
		if (pin >= 32) {
			channel = 2;
			pin -= 32;
		} else
			channel = 1;

		reg_val = XGpio_GetDataDirection(extra->instance,
						 channel);
		reg_val &= ~(1 << pin);
		XGpio_SetDataDirection(extra->instance,
				       channel,
				       reg_val);
		reg_val = XGpio_DiscreteRead(extra->instance,
					     channel);
		if(value)
			reg_val |= (1 << pin);
		else
			reg_val &= ~(1 << pin);
		XGpio_DiscreteWrite(extra->instance,
				    channel,
				    reg_val);
#endif
		break;
	case GPIO_PS:
#ifdef XGPIOPS_H
		XGpioPs_SetDirectionPin(extra->instance,
					desc->number,
					GPIO_OUT);
		XGpioPs_SetOutputEnablePin(extra->instance,
					   desc->number,
					   GPIO_OUT);
		XGpioPs_WritePin(extra->instance,
				 desc->number,
				 value);
#endif
		break;
	default:
		return FALSE;
		break;
	}

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
	struct xil_gpio_desc	*extra;
	uint8_t pin = desc->number;
	uint8_t channel;
	uint32_t reg_val;

	extra = desc->extra;

	switch (extra->type) {
	case GPIO_PL:
#ifdef XGPIO_H
		if (pin >= 32) {
			channel = 2;
			pin -= 32;
		} else
			channel = 1;

		reg_val = XGpio_DiscreteRead(extra->instance, channel);
		if(value)
			reg_val |= (1 << pin);
		else
			reg_val &= ~(1 << pin);
		XGpio_DiscreteWrite(extra->instance, channel, reg_val);
#endif
		break;
	case GPIO_PS:
#ifdef XGPIOPS_H
		XGpioPs_WritePin(extra->instance, desc->number, value);
#endif
		break;
	default:
		return FALSE;
		break;
	}

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
