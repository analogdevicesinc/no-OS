/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by MAX22007 example
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_spi.h"
#include "stm32_uart.h"

/* SPI device parameters */
#define SPI_DEVICE_ID		1 // SPI1
#define GPIO_CS_PIN         15 // PA15
#define GPIO_CS_PORT        0 // GPIOA
#define spi_platform_ops    stm32_spi_ops

/* UART device parameters */
#define UART_INSTANCE        &huart5
#define UART_DEVICE_ID	    5U // UART5
#define UART_IRQ_ID	        UART5_IRQn
#define UART_BAUDRATE	    230400
#define uart_platform_ops   stm32_uart_ops
#define UART_EXTRA	        &max22007_uart_extra_ip

extern struct stm32_spi_init_param max22007_spi_extra_ip;
extern struct stm32_uart_init_param max22007_uart_extra_ip;

extern UART_HandleTypeDef huart5;

#endif /* __PARAMETERS_H__ */
