/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by adf4382 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
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
struct stm32_uart_init_param adf4382_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_spi_init_param adf4382_spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
};
