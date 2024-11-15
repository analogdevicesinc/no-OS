/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-ad74416h project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
struct stm32_uart_init_param ad74416h_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_spi_init_param ad74416h_spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
};
