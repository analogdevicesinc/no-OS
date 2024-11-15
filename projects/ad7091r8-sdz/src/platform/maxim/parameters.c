/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definitions specific to Maxim platform used by eval-ad7091r8-sdz
 *           project.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
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
struct max_uart_init_param ad7091r8_uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct max_spi_init_param ad7091r8_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param ad7091r8_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

/* Initialization for RESET GPIO. */
struct no_os_gpio_init_param ad7091r8_gpio_reset_ip = {
	.port = GPIO_RESET_PORT_NUM,
	.number = GPIO_RESET_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

/* Initialization for CONVST GPIO. */
struct no_os_gpio_init_param ad7091r8_gpio_convst_ip = {
	.port = GPIO_CONVST_PORT_NUM,
	.number = GPIO_CONVST_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};
