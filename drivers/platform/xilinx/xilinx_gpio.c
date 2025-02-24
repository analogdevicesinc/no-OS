/***************************************************************************//**
 *   @file   xilinx/xilinx_gpio.c
 *   @brief  Implementation of Xilinx GPIO Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include <stdlib.h>

#include <xparameters.h>
#ifdef XPAR_XGPIO_NUM_INSTANCES
#include <xgpio.h>
#endif
#ifdef XPAR_XGPIOPS_NUM_INSTANCES
#include <xgpiops.h>
#endif

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_alloc.h"
#include "xilinx_gpio.h"

/**
 * @brief Prepare the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - The structure that contains the GPIO parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t _gpio_init(struct no_os_gpio_desc *desc,
		   const struct no_os_gpio_init_param *param)
{
	int32_t				ret;
	struct xil_gpio_desc		*xdesc;
	struct xil_gpio_init_param	*xinit;

	ret = -1;

	xdesc = desc->extra;
	xinit = param->extra;

	xdesc->type = xinit->type;
	desc->number = param->number;

	switch (xinit->type) {
	case GPIO_PL:
#ifdef XGPIO_H
		xdesc->instance = (XGpio *)no_os_malloc(sizeof(XGpio));
		if (!xdesc->instance)
			goto pl_error;

		xdesc->config = XGpio_LookupConfig(xinit->device_id);
		if (xdesc->config == NULL)
			goto pl_error;

		ret = XGpio_CfgInitialize(xdesc->instance,
					  xdesc->config,
					  ((XGpio_Config*)xdesc->config)
					  ->BaseAddress);
		if (ret < 0)
			goto pl_error;

		break;
pl_error:
		no_os_free(xdesc->instance);
#endif
		goto error;
	case GPIO_PS:
#ifdef XGPIOPS_H
		xdesc->instance = (XGpioPs *)no_os_malloc(sizeof(XGpioPs));
		if (!xdesc->instance)
			goto ps_error;

		xdesc->config = XGpioPs_LookupConfig(xinit->device_id);
		if (xdesc->config == NULL)
			goto ps_error;

		ret = XGpioPs_CfgInitialize(xdesc->instance,
					    xdesc->config,
					    ((XGpioPs_Config*)xdesc->config)->BaseAddr);
		if (ret < 0)
			goto ps_error;
		break;
ps_error:
		no_os_free(xdesc->instance);
#endif
		goto error;
error:
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_get(struct no_os_gpio_desc **desc,
		     const struct no_os_gpio_init_param *param)
{
	struct no_os_gpio_desc	*descriptor;
	struct xil_gpio_desc	*extra;
	int32_t			ret;

	descriptor = (struct no_os_gpio_desc *)no_os_malloc(sizeof(*descriptor));
	extra = (struct xil_gpio_desc*)no_os_malloc(sizeof(*extra));

	if (!descriptor || !extra)
		return -1;

	descriptor->extra = extra;
	ret = _gpio_init(descriptor, param);

	if (ret != 0)
		goto error;

	*desc = descriptor;

	return 0;
error:
	no_os_free(extra);
	no_os_free(descriptor);

	return -1;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_get_optional(struct no_os_gpio_desc **desc,
			      const struct no_os_gpio_init_param *param)
{
	if (param == NULL) {
		*desc = NULL;
		return 0;
	}

	return xil_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (desc != NULL) {
		no_os_free(((struct xil_gpio_desc *)(desc->extra))->instance);
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
int32_t xil_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	struct xil_gpio_desc	*extra;
#ifdef XGPIO_H
	uint8_t pin = desc->number;
	uint32_t channel = 1;
	uint32_t config	 = 0;
#endif
	extra = desc->extra;

	switch (extra->type) {
	case GPIO_PL:
#ifdef XGPIO_H
		/* We assume that pin 32 is the first pin from channel 2 */
		if (pin >= 32) {
			channel = 2;
			pin -= 32;
		}

		config = XGpio_GetDataDirection((XGpio *)extra->instance,
						channel);
		config |= (1 << pin);
		XGpio_SetDataDirection((XGpio *)extra->instance,
				       channel,
				       config);
#endif
		break;
	case GPIO_PS:
#ifdef XGPIOPS_H
		XGpioPs_SetDirectionPin(extra->instance,
					desc->number,
					NO_OS_GPIO_IN);
#endif
		break;
	default:
		return -1;
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
int32_t xil_gpio_direction_output(struct no_os_gpio_desc *desc,
				  uint8_t value)
{
	struct xil_gpio_desc	*extra;
#ifdef XGPIO_H
	uint8_t pin = desc->number;
	uint8_t channel;
	uint32_t reg_val;
#endif
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
		if (value)
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
					NO_OS_GPIO_OUT);
		XGpioPs_SetOutputEnablePin(extra->instance,
					   desc->number,
					   NO_OS_GPIO_OUT);
		XGpioPs_WritePin(extra->instance,
				 desc->number,
				 value);
#endif
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_get_direction(struct no_os_gpio_desc *desc,
			       uint8_t *direction)
{
	if (!desc)
		return -1;

	struct xil_gpio_desc	*extra;
	uint8_t pin = desc->number;

#ifdef XGPIO_H
	uint8_t channel;
#endif
	extra = desc->extra;

	switch (extra->type) {
	case GPIO_PL:
#ifdef XGPIO_H
		if (pin >= 32) {
			channel = 2;
			pin -= 32;
		} else
			channel = 1;
		*direction = ((XGpio_GetDataDirection(extra->instance,
						      channel) >> pin) & 0x1) ^ 0x1;
#endif
		break;
	case GPIO_PS:
#ifdef XGPIOPS_H
		*direction = XGpioPs_GetDirectionPin(extra->instance, pin);
#endif
		break;
	default:
		return -1;
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
int32_t xil_gpio_set_value(struct no_os_gpio_desc *desc,
			   uint8_t value)
{
	struct xil_gpio_desc	*extra;
#ifdef XGPIO_H
	uint8_t pin = desc->number;
	uint8_t channel;
	uint32_t reg_val;
#endif
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
		if (value)
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
int32_t xil_gpio_get_value(struct no_os_gpio_desc *desc,
			   uint8_t *value)
{
	if (!desc)
		return -1;

	struct xil_gpio_desc	*extra;
	uint8_t pin = desc->number;

#ifdef XGPIO_H
	uint8_t channel;
#endif
	extra = desc->extra;

	switch (extra->type) {
	case GPIO_PL:
#ifdef XGPIO_H
		if (pin >= 32) {
			channel = 2;
			pin -= 32;
		} else
			channel = 1;
		*value = (XGpio_DiscreteRead(extra->instance, channel) >> pin) & 0x01;
#endif
		break;
	case GPIO_PS:
#ifdef XGPIOPS_H
		*value = XGpioPs_ReadPin(extra->instance, pin);
#endif
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief Xilinx platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops xil_gpio_ops = {
	.gpio_ops_get = &xil_gpio_get,
	.gpio_ops_get_optional = &xil_gpio_get_optional,
	.gpio_ops_remove = &xil_gpio_remove,
	.gpio_ops_direction_input = &xil_gpio_direction_input,
	.gpio_ops_direction_output = &xil_gpio_direction_output,
	.gpio_ops_get_direction = &xil_gpio_get_direction,
	.gpio_ops_set_value = &xil_gpio_set_value,
	.gpio_ops_get_value = &xil_gpio_get_value,
};
