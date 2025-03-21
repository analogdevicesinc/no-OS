/***************************************************************************//**
 *   @file   aducm36x_gpio.c
 *   @brief  Implementation of ADuCM360 GPIO driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "ADuCM360.h"
#include "no_os_gpio.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t aducm36x_gpio_get(struct no_os_gpio_desc **desc,
			  const struct no_os_gpio_init_param *param)
{
	struct no_os_gpio_desc *descriptor;

	if (!param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->port = param->port;
	descriptor->number = param->number;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t aducm36x_gpio_get_optional(struct no_os_gpio_desc **desc,
				   const struct no_os_gpio_init_param *param)
{
	if (param == NULL) {
		*desc = NULL;
		return 0;
	}

	return aducm36x_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t aducm36x_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc);

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
int32_t aducm36x_gpio_set_value(struct no_os_gpio_desc *desc, uint8_t value)
{
	if (value == NO_OS_GPIO_LOW) {
		switch (desc->port) {
		case 0:
			pADI_GP0->GPOUT &= ~NO_OS_BIT(desc->number);
			break;
		case 1:
			pADI_GP1->GPOUT &= ~NO_OS_BIT(desc->number);
			break;
		case 2:
			pADI_GP2->GPOUT &= ~NO_OS_BIT(desc->number);
			break;
#ifdef pADI_GP3
		case 3:
			pADI_GP3->GPOUT &= ~NO_OS_BIT(desc->number);
			break;
#endif
		default:
			return -EINVAL;
		}
	}

	if (value == NO_OS_GPIO_HIGH) {
		switch (desc->port) {
		case 0:
			pADI_GP0->GPOUT |= NO_OS_BIT(desc->number);
			break;
		case 1:
			pADI_GP1->GPOUT |= NO_OS_BIT(desc->number);
			break;
		case 2:
			pADI_GP2->GPOUT |= NO_OS_BIT(desc->number);
			break;
#ifdef pADI_GP3
		case 3:
			pADI_GP3->GPOUT |= NO_OS_BIT(desc->number);
			break;
#endif
		default:
			return -EINVAL;
		}
	}

	return 0;
}

/**
 * @brief ADuCM360 GPIO platform ops
 */
const struct no_os_gpio_platform_ops aducm36x_gpio_ops = {

	.gpio_ops_get = &aducm36x_gpio_get,
	.gpio_ops_get_optional = &aducm36x_gpio_get_optional,
	.gpio_ops_remove = &aducm36x_gpio_remove,
	.gpio_ops_set_value = &aducm36x_gpio_set_value,
};
