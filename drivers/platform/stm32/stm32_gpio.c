/***************************************************************************//**
 *   @file   stm32/stm32_gpio.c
 *   @brief  Implementation of stm32 gpio functionality.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#include <stdlib.h>
#include <errno.h>
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "stm32_gpio.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Prepare the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - The structure that contains the GPIO parameters.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t _gpio_init(struct no_os_gpio_desc *desc,
			  const struct no_os_gpio_init_param *param)
{
	int32_t ret = 0;
	struct stm32_gpio_desc *extra = desc->extra;
	struct stm32_gpio_desc *pextra = param->extra;
	GPIO_InitTypeDef gis;

	if (!param)
		return -EINVAL;

	/* enable gpio port in RCC */
	if (pextra->port == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
#ifdef GPIOB
	else if (pextra->port == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
#endif
#ifdef GPIOC
	else if (pextra->port == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
#endif
#ifdef GPIOD
	else if (pextra->port == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();
#endif
#ifdef GPIOE
	else if (pextra->port == GPIOE)
		__HAL_RCC_GPIOE_CLK_ENABLE();
#endif
#ifdef GPIOF
	else if (pextra->port == GPIOF)
		__HAL_RCC_GPIOF_CLK_ENABLE();
#endif
#ifdef GPIOG
	else if (pextra->port == GPIOG)
		__HAL_RCC_GPIOG_CLK_ENABLE();
#endif
#ifdef GPIOH
	else if (pextra->port == GPIOH)
		__HAL_RCC_GPIOH_CLK_ENABLE();
#endif
	else
		return -EINVAL;

	switch (pextra->mode & GPIO_MODE) {
	case MODE_INPUT:
	case MODE_OUTPUT:
		break;
	default:
		return -EINVAL;
	}

	/* copy the settings to gpio descriptor */
	desc->number = param->number;
	extra->port = pextra->port;
	extra->mode = pextra->mode;
	extra->pull = pextra->pull;
	extra->speed = pextra->speed;

	/* configure gpio with user configuration */
	gis.Pin = NO_OS_BIT(param->number);
	gis.Mode = extra->mode;
	gis.Pull = extra->pull;
	gis.Speed = extra->speed;
	HAL_GPIO_Init(extra->port, &gis);

	return ret;
}

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_gpio_get(struct no_os_gpio_desc **desc,
		       const struct no_os_gpio_init_param *param)
{
	struct no_os_gpio_desc *descriptor;
	struct stm32_gpio_desc *extra;
	int32_t ret;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct no_os_gpio_desc *)malloc(sizeof(*descriptor));
	if (!descriptor) {
		ret = -ENOMEM;
		goto error;
	}
	extra = (struct stm32_gpio_desc*)malloc(sizeof(*extra));
	if (!extra) {
		ret = -ENOMEM;
		goto error;
	}

	descriptor->extra = extra;
	ret = _gpio_init(descriptor, param);
	if(ret < 0)
		goto error;

	*desc = descriptor;

	return 0;
error:
	free(extra);
	free(descriptor);

	return ret;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_gpio_get_optional(struct no_os_gpio_desc **desc,
				const struct no_os_gpio_init_param *param)
{
	if(param == NULL) {
		*desc = NULL;
		return 0;
	}

	return stm32_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (desc != NULL)
		free(desc->extra);

	free(desc);

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t stm32_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	GPIO_InitTypeDef gis = {0};
	struct stm32_gpio_desc	*extra;

	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EFAULT;

	extra = desc->extra;

	/* configure gpio with user configuration */
	gis.Pin = NO_OS_BIT(desc->number);
	gis.Mode = GPIO_MODE_INPUT;
	gis.Pull = extra->pull;
	gis.Speed = extra->speed;
	HAL_GPIO_Init(extra->port, &gis);

	/* copy the settings to gpio descriptor */
	extra->mode = gis.Mode;

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
int32_t stm32_gpio_direction_output(struct no_os_gpio_desc *desc,
				    uint8_t value)
{
	GPIO_InitTypeDef gis = {0};
	struct stm32_gpio_desc	*extra;

	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EFAULT;

	extra = desc->extra;

	/* configure gpio output level */
	HAL_GPIO_WritePin(extra->port, NO_OS_BIT(desc->number), (GPIO_PinState)value);

	/* configure gpio with user configuration */
	gis.Pin = NO_OS_BIT(desc->number);
	gis.Mode = GPIO_MODE_OUTPUT_PP;
	gis.Pull = extra->pull;
	gis.Speed = extra->speed;
	HAL_GPIO_Init(extra->port, &gis);

	/* copy the settings to gpio descriptor */
	extra->mode = gis.Mode;

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
int32_t stm32_gpio_get_direction(struct no_os_gpio_desc *desc,
				 uint8_t *direction)
{
	if (!desc || !direction)
		return -EINVAL;

	struct stm32_gpio_desc *extra = desc->extra;
	*direction = (extra->mode & GPIO_MODE) ? NO_OS_GPIO_OUT : NO_OS_GPIO_IN;
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
int32_t stm32_gpio_set_value(struct no_os_gpio_desc *desc,
			     uint8_t value)
{
	struct stm32_gpio_desc *extra;

	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EFAULT;

	extra = desc->extra;

	/* configure gpio output level */
	HAL_GPIO_WritePin(extra->port, NO_OS_BIT(desc->number), (GPIO_PinState)value);

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
int32_t stm32_gpio_get_value(struct no_os_gpio_desc *desc,
			     uint8_t *value)
{
	struct stm32_gpio_desc	*extra;

	if (!desc || !value)
		return -EINVAL;

	if (!desc->extra)
		return -EFAULT;

	extra = desc->extra;

	*value = (uint8_t)HAL_GPIO_ReadPin(extra->port, NO_OS_BIT(desc->number));

	return 0;
}

/**
 * @brief stm32 platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops stm32_gpio_ops = {
	.gpio_ops_get = &stm32_gpio_get,
	.gpio_ops_get_optional = &stm32_gpio_get_optional,
	.gpio_ops_remove = &stm32_gpio_remove,
	.gpio_ops_direction_input = &stm32_gpio_direction_input,
	.gpio_ops_direction_output = &stm32_gpio_direction_output,
	.gpio_ops_get_direction = &stm32_gpio_get_direction,
	.gpio_ops_set_value = &stm32_gpio_set_value,
	.gpio_ops_get_value = &stm32_gpio_get_value,
};
