/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by ADF5611 project.
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_irq.h"
#include "stm32_spi.h"
#include "stm32_i2c.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#include "no_os_uart.h"
#include "no_os_i2c.h"

extern UART_HandleTypeDef huart5;

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#define IIO_APP_HUART   (&huart5)
#endif
#define UART_IRQ_ID     UART5_IRQn

#define UART_DEVICE_ID  5
#define UART_BAUDRATE   230400
#define UART_EXTRA      &adf5611_uart_extra_ip
#define UART_OPS        &stm32_uart_ops

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    4000000
#define SPI_CS          15
#define SPI_CS_PORT     0
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &adf5611_spi_extra

#define I2C_DEVICE_ID   1
#define I2C_TIMING      0x00000E14 /* (Unused) */
#define I2C_OPS         &stm32_i2c_ops
#define I2C_EXTRA       &adf5611_i2c_extra_param

extern struct stm32_uart_init_param	adf5611_uart_extra_ip;
extern struct stm32_spi_init_param	adf5611_spi_extra;
extern struct stm32_i2c_init_param	adf5611_i2c_extra_param;

#endif /* __PARAMETERS_H__ */
