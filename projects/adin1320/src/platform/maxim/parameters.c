/***************************************************************************//**
*   @file   parameters.c
*   @brief  Definition of Maxim platform data used by ADIN1320 project.
*   @author John Roed Retuya (Johnroed.Retuya@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param max_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_gpio_init_param max_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

/* Both ADIN1320 PHYs share a single MDIO bus over SPI (single CS) */
struct max_spi_init_param max_spi_extra_ip = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW
};
