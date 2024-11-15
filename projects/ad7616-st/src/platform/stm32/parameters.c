/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-ad7616 project.
 *   @author Esteban Blanc (eblanc@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
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
struct stm32_uart_init_param ad7616_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_spi_init_param ad7616_spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
};

struct stm32_gpio_init_param ad7616_gpio_convst_extra_ip = {
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
	.mode = GPIO_MODE_OUTPUT_PP,
};
