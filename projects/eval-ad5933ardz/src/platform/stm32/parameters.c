/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-ad5933ardz project.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_i2c_init_param i2c_extra_ip = {
	.i2c_timing = I2C_TIMING
};
