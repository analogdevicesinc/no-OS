/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of STM32 platform data used by adf4382 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com))
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_irq.h"
#include "stm32_spi.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

extern UART_HandleTypeDef huart5;

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#define IIO_APP_HUART   (&huart5)
#endif
#define UART_IRQ_ID     UART5_IRQn

#define UART_DEVICE_ID  5
#define UART_BAUDRATE   115200
#define UART_EXTRA      &adf4382_uart_extra_ip
#define UART_OPS        &stm32_uart_ops

#define SPI_DEVICE_ID   5
#define SPI_BAUDRATE    4000000
#define SPI_CS          9
#define SPI_CS_PORT     1
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &adf4382_spi_extra_ip

extern struct stm32_uart_init_param	adf4382_uart_extra_ip;
extern struct stm32_spi_init_param	adf4382_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
