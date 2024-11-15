/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-ad7616
 *           project.
 *   @author Esteban Blanc (eblanc@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern UART_HandleTypeDef huart5;

#define UART_IRQ_ID		UART5_IRQn
#define INTC_DEVICE_ID		0

#define UART_DEVICE_ID		5
#define UART_BAUDRATE		230400
#define UART_EXTRA		&ad7616_uart_extra_ip
#define UART_OPS		&stm32_uart_ops

#define GPIO_OPS		&stm32_gpio_ops
#define GPIO_PORT_A		0
#define GPIO_PORT_G		6
#define GPIO_BUSY_PORT		GPIO_PORT_A
#define GPIO_BUSY_NB		10
#define GPIO_RESET_PORT		GPIO_PORT_G
#define GPIO_RESET_NB		10
#define GPIO_CONVST_PORT	GPIO_PORT_A
#define GPIO_CONVST_NB		11

#define SPI_DEVICE_ID		1
#define SPI_BAUDRATE		22500000
#define SPI_CS			15
#define SPI_CS_PORT		GPIO_PORT_A
#define SPI_OPS			&stm32_spi_ops
#define SPI_EXTRA		&ad7616_spi_extra_ip

extern struct stm32_uart_init_param ad7616_uart_extra_ip;
extern struct stm32_spi_init_param ad7616_spi_extra_ip;
extern struct stm32_gpio_init_param ad7616_gpio_convst_extra_ip;

#endif /* __PARAMETERS_H__ */
