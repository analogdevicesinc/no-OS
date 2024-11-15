/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by ad74413r
 *           project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include "stm32_gpio_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern UART_HandleTypeDef huart3;

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#define IIO_APP_HUART   (&huart3)
#endif
#define UART_IRQ_ID     USART3_IRQn

#define UART_DEVICE_ID  3
#define UART_BAUDRATE   115200
#define UART_EXTRA      &ad74413r_uart_extra_ip
#define UART_OPS        &stm32_uart_ops

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    1000000
#define SPI_CS          14
#define SPI_CS_PORT     3
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &ad74413r_spi_extra_ip

extern struct stm32_uart_init_param ad74413r_uart_extra_ip;
extern struct stm32_spi_init_param ad74413r_spi_extra_ip;

#ifdef IIO_TRIGGER_EXAMPLE
extern struct stm32_gpio_irq_init_param ad74413r_gpio_irq_extra_ip;
/* Setting for PortG Pin0 used for DATA_READY.
   Has to be adapted accordingly if another pin is used.
 */
#define AD74413R_GPIO_TRIG_IRQ_ID     0    /* Not used in stm32 platform */
#define AD74413R_GPIO_CB_HANDLE       NULL /* Not used in stm32 platform */

#define GPIO_IRQ_ID             0 /* Pin 0 */
#define GPIO_IRQ_OPS            &stm32_gpio_irq_ops
#define GPIO_IRQ_EXTRA          &ad74413r_gpio_irq_extra_ip
#endif

#endif /* __PARAMETERS_H__ */
