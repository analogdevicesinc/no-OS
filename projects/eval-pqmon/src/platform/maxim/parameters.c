/*******************************************************************************
 *   @file   parameter.c
 *   @brief  Parameter definitions for pqm
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "parameters.h"

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

struct max_i2c_init_param vddioh_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param spi_extra_ip = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_uart_init_param uart_stdio_extra_ip = {.flow = UART_FLOW_DIS};

struct max_usb_uart_init_param iio_demo_usb_uart_extra_ip = {.vid = 0x0B6B,
	       .pid = 0x003C
};

struct max_gpio_init_param max_reset_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct max_gpio_init_param max_intr_gpio_extra_ip = {.vssel =
		MXC_GPIO_VSSEL_VDDIOH
};