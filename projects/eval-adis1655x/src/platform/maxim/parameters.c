/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by eval-adis1655x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "parameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

struct max_uart_init_param adis1655x_uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct max_gpio_init_param adis1655x_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param adis1655x_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

#ifdef IIO_TRIGGER_EXAMPLE
/* Initialization for Sync pin GPIO. */
struct no_os_gpio_init_param adis_gpio_drdy_ip = {
	.port = GPIO_DRDY_PORT_NUM,
	.number = GPIO_DRDY_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct max_gpio_init_param adis_gpio_drdy_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
#endif
