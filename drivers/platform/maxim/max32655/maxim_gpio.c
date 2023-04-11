/***************************************************************************//**
 *   @file   maxim_gpio.c
 *   @brief  Implementation of gpio driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "gpio.h"
#include "gpio_regs.h"
#include "maxim_gpio.h"
#include "maxim_irq.h"
#include "max32655.h"
#include "mxc_errors.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static void set_enable(mxc_gpio_regs_t *regs, uint32_t mask, uint8_t is_enabled)
{
	if (is_enabled)
		regs->en0 |= mask;
	else
		regs->en0 &= ~mask;
}

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_gpio_get(struct no_os_gpio_desc **desc,
		     const struct no_os_gpio_init_param *param)
{
	int32_t ret;
	mxc_gpio_cfg_t *g_cfg;
	struct max_gpio_init_param *pextra;
	uint32_t m_pad;
	struct no_os_gpio_desc *descriptor;

	if (!param || !param->extra || param->number >= N_PINS)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	g_cfg = no_os_calloc(1, sizeof(*g_cfg));
	if (!g_cfg) {
		ret = -ENOMEM;
		goto free_descriptor;
	}

	pextra = param->extra;

	switch(param->pull) {
	case NO_OS_PULL_NONE:
		m_pad = MXC_GPIO_PAD_NONE;
		break;
	case NO_OS_PULL_UP:
		m_pad = MXC_GPIO_PAD_PULL_UP;
		break;
	case NO_OS_PULL_DOWN:
		m_pad = MXC_GPIO_PAD_PULL_DOWN;
		break;
	default:
		ret = -EINVAL;
		goto free_g_cfg;
	}

	if (param->port >= N_PORTS) {
		ret = -EINVAL;
		goto free_g_cfg;
	}

	g_cfg->port = MXC_GPIO_GET_GPIO(param->port);
	g_cfg->mask = NO_OS_BIT(param->number);
	g_cfg->pad = m_pad;
	g_cfg->func = MXC_GPIO_FUNC_IN;
	g_cfg->vssel = pextra->vssel;

	descriptor->port = param->port;
	descriptor->number = param->number;
	descriptor->pull = param->pull;
	descriptor->extra = g_cfg;

	MXC_GPIO_Init(param->port);
	ret = MXC_GPIO_Config(descriptor->extra);
	if (ret) {
		ret = -EINVAL;
		goto free_g_cfg;
	}

	*desc = descriptor;

	return 0;

free_g_cfg:
	no_os_free(g_cfg);
free_descriptor:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_gpio_get_optional(struct no_os_gpio_desc **desc,
			      const struct no_os_gpio_init_param *param)
{
	if (param == NULL) {
		*desc = NULL;
		return 0;
	}

	return max_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	mxc_gpio_cfg_t *maxim_extra;

	if (!desc || desc->number >= N_PINS)
		return -EINVAL;

	maxim_extra = desc->extra;
	maxim_extra->func = MXC_GPIO_FUNC_IN;
	MXC_GPIO_Config(maxim_extra);

	return 0;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_gpio_direction_output(struct no_os_gpio_desc *desc, uint8_t value)
{
	mxc_gpio_regs_t *gpio_regs;
	mxc_gpio_cfg_t *maxim_extra;

	if (!desc || desc->number >= N_PINS || value >= NO_OS_GPIO_HIGH_Z)
		return -EINVAL;

	maxim_extra = desc->extra;
	gpio_regs = maxim_extra->port;
	maxim_extra->func = MXC_GPIO_FUNC_OUT;
	MXC_GPIO_Config(maxim_extra);

	set_enable(gpio_regs, NO_OS_BIT(desc->number), true);
	switch(value) {
	case NO_OS_GPIO_LOW:
		MXC_GPIO_OutClr(gpio_regs, NO_OS_BIT(desc->number));
		break;
	case NO_OS_GPIO_HIGH:
		MXC_GPIO_OutSet(gpio_regs, NO_OS_BIT(desc->number));
		break;
	}

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_gpio_get_direction(struct no_os_gpio_desc *desc,
			       uint8_t *direction)
{
	mxc_gpio_cfg_t *maxim_extra;

	if (!desc || desc->number >= N_PINS)
		return -EINVAL;

	maxim_extra = desc->extra;

	if (maxim_extra->func == MXC_GPIO_FUNC_OUT)
		*direction = NO_OS_GPIO_OUT;
	else
		*direction = NO_OS_GPIO_IN;

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_gpio_set_value(struct no_os_gpio_desc *desc, uint8_t value)
{
	mxc_gpio_cfg_t *max_gpio_cfg;
	mxc_gpio_regs_t *gpio_regs;

	if (!desc || desc->number >= N_PINS)
		return -EINVAL;

	max_gpio_cfg = desc->extra;
	gpio_regs = max_gpio_cfg->port;

	set_enable(gpio_regs, NO_OS_BIT(desc->number), true);
	switch(value) {
	case NO_OS_GPIO_LOW:
		MXC_GPIO_OutClr(gpio_regs, NO_OS_BIT(desc->number));
		break;
	case NO_OS_GPIO_HIGH:
		MXC_GPIO_OutSet(gpio_regs, NO_OS_BIT(desc->number));
		break;
	case NO_OS_GPIO_HIGH_Z:
		set_enable(gpio_regs, NO_OS_BIT(desc->number), false);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_gpio_get_value(struct no_os_gpio_desc *desc, uint8_t *value)
{
	mxc_gpio_cfg_t *max_gpio_cfg;
	mxc_gpio_regs_t *gpio_regs;

	if (!desc || desc->number >= N_PINS)
		return -EINVAL;

	max_gpio_cfg = desc->extra;
	gpio_regs = max_gpio_cfg->port;

	set_enable(gpio_regs, NO_OS_BIT(desc->number), true);
	if (max_gpio_cfg->func == MXC_GPIO_FUNC_IN)
		*value = MXC_GPIO_InGet(gpio_regs, NO_OS_BIT(desc->number)) >> desc->number;
	else
		*value = MXC_GPIO_OutGet(gpio_regs, NO_OS_BIT(desc->number)) >> desc->number;

	return 0;
}

/**
 * @brief maxim platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops max_gpio_ops = {
	.gpio_ops_get = &max_gpio_get,
	.gpio_ops_get_optional = &max_gpio_get_optional,
	.gpio_ops_remove = &max_gpio_remove,
	.gpio_ops_direction_input = &max_gpio_direction_input,
	.gpio_ops_direction_output = &max_gpio_direction_output,
	.gpio_ops_get_direction = &max_gpio_get_direction,
	.gpio_ops_set_value = &max_gpio_set_value,
	.gpio_ops_get_value = &max_gpio_get_value
};
