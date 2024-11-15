/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-adxl355-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
struct stm32_uart_init_param adxl355_uart_extra_ip = {
	.huart = UART_INSTANCE,
};

struct stm32_spi_init_param adxl355_spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
};

#ifdef IIO_TRIGGER_EXAMPLE
struct stm32_gpio_irq_init_param adxl355_gpio_irq_extra_ip = {
	.port_nb = 0, /* Port A */
};
#endif

#ifdef IIO_LWIP_EXAMPLE
struct stm32_gpio_init_param adin1110_reset_gpio_extra_ip = {
	.mode = GPIO_MODE_OUTPUT_OD,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_spi_init_param adin1110_spi_extra_ip  = {
	.chip_select_port = ADIN1110_SPI_CS_PORT,
};
#endif
