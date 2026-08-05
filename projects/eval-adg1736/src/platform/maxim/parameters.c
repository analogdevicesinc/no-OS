/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by eval-adg1736 project.
 *   @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

struct max_uart_init_param adg1736_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_gpio_init_param adg1736_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
