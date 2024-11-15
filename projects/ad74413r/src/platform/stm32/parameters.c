/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by ad74413r project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
struct stm32_uart_init_param ad74413r_uart_extra_ip = {
	.huart = &huart3,
};

struct stm32_spi_init_param ad74413r_spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
};

#ifdef IIO_TRIGGER_EXAMPLE
struct stm32_gpio_irq_init_param ad74413r_gpio_irq_extra_ip = {
	.port_nb = 6, /* Port G */
};
#endif
