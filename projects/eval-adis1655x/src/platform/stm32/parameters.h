/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-adis1655x
 *           project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
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

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#endif

#define UART_DEVICE_ID  5
#define UART_BAUDRATE   115200
#define UART_EXTRA      &adis1655x_uart_extra_ip
#define UART_OPS        &stm32_uart_ops
#define UART_IRQ_ID     UART5_IRQn

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    15000000
#define SPI_CS          15
#define SPI_CS_PORT     0
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &adis1655x_spi_extra_ip

extern struct stm32_uart_init_param adis1655x_uart_extra_ip;
extern struct stm32_spi_init_param adis1655x_spi_extra_ip;

#define GPIO_RESET_PIN_NUM   4
#define GPIO_RESET_PORT_NUM  0
#define GPIO_OPS            &stm32_gpio_ops
#define GPIO_EXTRA          &adis1655x_gpio_reset_extra_ip
extern struct stm32_gpio_init_param adis1655x_gpio_reset_extra_ip ;

#ifdef IIO_TRIGGER_EXAMPLE

#define GPIO_DRDY_PIN_NUM   2
#define GPIO_DRDY_PORT_NUM  0

extern struct stm32_gpio_irq_init_param adis1655x_gpio_irq_extra_ip;
/* Setting for PortA Pin2 used for DATA_READY.
   Has to be adapted accordingly if another pin is used.
 */
#define ADIS1655X_GPIO_TRIG_IRQ_ID     0    /* Not used in stm32 platform */
#define ADIS1655X_GPIO_CB_HANDLE       NULL /* Not used in stm32 platform */

#define GPIO_IRQ_ID             2 /* Pin 2 */
#define GPIO_IRQ_OPS            &stm32_gpio_irq_ops
#define GPIO_IRQ_EXTRA          &adis1655x_gpio_irq_extra_ip
#endif

#endif /* __PARAMETERS_H__ */
