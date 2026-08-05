/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by LTM3360B project.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "stm32_uart.h"
#include "stm32_i2c.h"

/**
 * @brief STM32 UART extra initialization parameters for console communication
 */
struct stm32_uart_init_param ltm3360b_uart_extra_ip = {
	.huart = &huart5,
};
