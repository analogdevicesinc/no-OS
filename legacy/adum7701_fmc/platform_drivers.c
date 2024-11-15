/***************************************************************************//**
 * @file platform_drivers.c
 * @brief Implementation of Platform Drivers.
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "platform_drivers.h"
/***************************************************************************//**
 * @brief ad_pow2 Create a mask for a given number of bit
 *******************************************************************************/
uint32_t ad_pow2(uint32_t number)
{

	uint32_t index;
	uint32_t mask = 1;

	for (index=1; index < number; index++) {
		mask = (mask << 1) ^ 1;
	}

	return mask;
}

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param gpio_number - The number of the GPIO.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(gpio_desc **desc,
		 gpio_init_param param)
{
	gpio_desc        *d;
	d = (gpio_desc *)malloc(sizeof(*d));
	if (!d)
		return -1;

	d->number = param.number;
	d->id = param.id;

	d->ps7_config = XGpioPs_LookupConfig(d->id);
	XGpioPs_CfgInitialize(&d->ps7_instance,
			      d->ps7_config,
			      d->ps7_config->BaseAddr);
	*desc = d;

	return 0;
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_remove(gpio_desc *desc)
{
	free(desc);

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_value(gpio_desc *desc,
		       uint8_t value)
{
	XGpioPs_SetDirectionPin(&desc->ps7_instance,
				desc->number, 1);
	XGpioPs_SetOutputEnablePin(&desc->ps7_instance,
				   desc->number, 1);
	XGpioPs_WritePin(&desc->ps7_instance,
			 desc->number, value);

	return 0;
}
