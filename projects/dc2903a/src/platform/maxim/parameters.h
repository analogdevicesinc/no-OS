/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Implementation of parameters.h
 *   @author JSanBuenaventura (jose.sanbuenaventura@analog.com)
 *   @author MSosa (marcpaolo.sosa@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define UART_OPS                &max_uart_ops
#define UART_EXTRA              &ltc2672_uart_extra_ip
extern struct max_uart_init_param ltc2672_uart_extra_ip;

#define UART_DEVICE_ID          1
#define UART_BAUDRATE           57600
#define UART_IRQ_ID             UART1_IRQn

#define LTC2672_VAR             LTC2672_12

#define SPI_DEVICE_ID           1
#define SPI_BAUDRATE            5000000
#define SPI_CS                  0
#define SPI_OPS                 &max_spi_ops
#define SPI_EXTRA               &ltc2672_spi_extra_ip

extern struct max_spi_init_param ltc2672_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
