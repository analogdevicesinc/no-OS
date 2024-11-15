/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adis1657x
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
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
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#endif

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_EXTRA      &adis1657x_uart_extra_ip
#define UART_OPS        &max_uart_ops
#define UART_IRQ_ID     UART0_IRQn


#define SPI_DEVICE_ID   1
#define SPI_CS          1

#define SPI_BAUDRATE    8000000
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adis1657x_spi_extra_ip

#define GPIO_OPS            &max_gpio_ops
#define GPIO_EXTRA          &adis1657x_gpio_extra_ip

extern struct max_uart_init_param adis1657x_uart_extra_ip;
extern struct max_spi_init_param adis1657x_spi_extra_ip;
extern struct max_gpio_init_param adis1657x_gpio_extra_ip;

#define GPIO_RESET_PIN_NUM   19
#define GPIO_RESET_PORT_NUM  0


#ifdef IIO_TRIGGER_EXAMPLE
#define GPIO_DRDY_PIN_NUM   6
#define GPIO_DRDY_PORT_NUM  1

#define NVIC_GPIO_IRQ   GPIO1_IRQn

#define ADIS1657X_GPIO_TRIG_IRQ_ID    GPIO_DRDY_PIN_NUM
#define ADIS1657X_GPIO_CB_HANDLE      MXC_GPIO_GET_GPIO(GPIO_DRDY_PORT_NUM)

#define GPIO_IRQ_ID             GPIO_DRDY_PORT_NUM
#define GPIO_IRQ_OPS            &max_gpio_irq_ops
#define GPIO_IRQ_EXTRA          &adis_gpio_drdy_extra_ip

extern struct no_os_gpio_init_param adis_gpio_drdy_ip;
extern struct max_gpio_init_param adis_gpio_drdy_extra_ip;
#endif

#endif /* __PARAMETERS_H__ */
