/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by the STOUT project.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
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
struct max_uart_init_param ade9113_uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct max_spi_init_param ade9113_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param ade9113_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_i2c_init_param adt75_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};