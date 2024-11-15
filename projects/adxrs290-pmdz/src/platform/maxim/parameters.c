/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by eval-adxrs290-pmdz project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
struct max_uart_init_param adxrs290_uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct max_gpio_init_param adxrs290_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param adxrs290_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
