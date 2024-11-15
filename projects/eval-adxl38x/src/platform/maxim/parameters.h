/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adxl38x
 *           project.
 *   @author BRajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_IRQ_ID     UART0_IRQn
#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_EXTRA      &adxl38x_uart_extra_ip
#define UART_OPS        &max_uart_ops

#if (TARGET_NUM == 78000)
#define SPI_DEVICE_ID   1
#define SPI_CS          1
#endif

#define SPI_BAUDRATE    8000000
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adxl38x_spi_extra_ip

extern struct max_uart_init_param adxl38x_uart_extra_ip;
extern struct max_spi_init_param adxl38x_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
