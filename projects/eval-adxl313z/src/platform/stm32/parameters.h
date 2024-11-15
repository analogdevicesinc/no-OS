/*******************************************************************************
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-adxl313z
 *           project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern UART_HandleTypeDef huart2;

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID 0
#define IIO_APP_HUART	(&huart2)
#endif
#define UART_IRQ_ID     USART2_IRQn
#define UART_DEVICE_ID      2
#define UART_BAUDRATE  115200
#define UART_OPS        &stm32_uart_ops

#define SPI_DEVICE_ID    1
#define SPI_CS          4
#define SPI_CS_PORT     0
#define SPI_OPS         &stm32_spi_ops

extern struct stm32_uart_init_param xuip;
extern struct stm32_spi_init_param xsip;

#endif /* __PARAMETERS_H__ */
