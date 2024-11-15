/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-pulsar_adc project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
uint8_t in_buff[MAX_SIZE_BASE_ADDR] = {0};

struct stm32_uart_init_param pulsar_adc_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_spi_init_param pulsar_adc_spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
	.get_input_clock = HAL_RCC_GetPCLK2Freq,
};
