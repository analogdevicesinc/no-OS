/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-adis1654x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
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

struct stm32_uart_init_param adis1654x_uart_extra_ip = {
	.huart = &huart5,
};


struct stm32_spi_init_param adis1654x_spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
};

struct stm32_gpio_init_param adis1654x_gpio_reset_extra_ip = {
	.mode = GPIO_MODE_OUTPUT_OD,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

#ifdef IIO_TRIGGER_EXAMPLE
struct stm32_gpio_irq_init_param adis1654x_gpio_irq_extra_ip = {
	.port_nb = 0, /* Port A */
};
#endif