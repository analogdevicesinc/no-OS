/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-adxrs290-pmdz
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
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
#ifdef IIO_SUPPORT
extern UART_HandleTypeDef huart5;
#define INTC_DEVICE_ID  0
#define IIO_APP_HUART   (&huart5)
#define UART_IRQ_ID     UART5_IRQn
#define UART_DEVICE_ID  5
#define UART_BAUDRATE   115200
#define UART_EXTRA		&adxrs290_uart_extra_ip
#define UART_OPS		&stm32_uart_ops
#endif

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    4000000
#define SPI_CS          15
#define SPI_CS_PORT     0
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &adxrs290_spi_extra_ip

#define GPIO_SYNC_PIN_NUM   2
#define GPIO_SYNC_PORT_NUM  0
#define GPIO_OPS            &stm32_gpio_ops
#define GPIO_EXTRA          &adxrs290_gpio_extra_ip

#ifdef IIO_TRIGGER_EXAMPLE
#error IIO_TRIGGER_EXAMPLE is not supported on STM32 platform for adxrs290-pmdz project.
#endif

extern struct stm32_uart_init_param adxrs290_uart_extra_ip;
extern struct stm32_spi_init_param adxrs290_spi_extra_ip;
extern struct stm32_gpio_init_param adxrs290_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */
