/***************************************************************************//**
 *   @file   platform_drivers.h
 *   @brief  Header file of Generic Platform Drivers.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DRIVERS_H_
#define PLATFORM_DRIVERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <sleep.h>
#include <xil_io.h>
#include <xgpiops.h>
#include <xspips.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define mdelay(msecs)	usleep(1000 * msecs)
#define GPIO_HIGH	0x01
#define GPIO_LOW	0x00

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef enum {
	GENERIC_GPIO,
	PS7_GPIO
} gpio_type;

typedef struct {
	gpio_type	type;
	uint32_t	id;
	uint8_t		number;
} gpio_init_param;

typedef struct {
	gpio_type	type;
	uint32_t	id;
	uint8_t		number;
	XGpioPs_Config *ps7_config;
	XGpioPs		ps7_instance;
} gpio_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Obtain the GPIO decriptor. */
int32_t gpio_get(gpio_desc **desc,
		 gpio_init_param param);

/* Free the resources allocated by gpio_get() */
int32_t gpio_remove(gpio_desc *desc);

/* Set the value of the specified GPIO. */
int32_t gpio_set_value(gpio_desc *desc,
		       uint8_t value);
uint32_t ad_pow2(uint32_t number);
#endif /* PLATFORM_DRIVERS_H_ */
