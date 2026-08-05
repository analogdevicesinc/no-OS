/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by max17616 project.
 *   @author Carlos Jones (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

/* UART */
struct max_uart_init_param uart_extra = {
	.flow = MAX_UART_FLOW_DIS
};

/* I2C */
struct max_i2c_init_param i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
