/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by ades1754 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

struct max_uart_init_param ades1754_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_uart_init_param ades1754_comm_extra = {
	.flow = MAX_UART_FLOW_DIS,
	.vssel = MXC_GPIO_VSSEL_VDDIO,
};
