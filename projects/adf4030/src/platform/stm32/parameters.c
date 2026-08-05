/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by ADF4030 project.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param adf4030_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_spi_init_param adf4030_spi_extra = {
	.chip_select_port = SPI_CS_PORT,
};
