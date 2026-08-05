/*******************************************************************************
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-adxl313z project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct stm32_uart_init_param xuip = {
	.huart = &huart2,
};

struct stm32_spi_init_param xsip  = {
	.chip_select_port = SPI_CS_PORT,
};

