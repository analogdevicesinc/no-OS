/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adxrs290-pmdz
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
#define UART_IRQ_ID     UART0_IRQn
#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_EXTRA		  &adxrs290_uart_extra_ip
#define UART_OPS		    &max_uart_ops
#endif

#define SPI_DEVICE_ID   0
#define SPI_BAUDRATE    1000000
#define SPI_CS          0
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adxrs290_spi_extra_ip

#define GPIO_OPS            &max_gpio_ops
#define GPIO_EXTRA          &adxrs290_gpio_extra_ip
#if (TARGET_NUM != 32655)
#error PIN and PORT configuration have to be adapted for the used target.
#else
#define GPIO_SYNC_PIN_NUM   9
#define GPIO_SYNC_PORT_NUM  1
#endif

extern struct max_uart_init_param adxrs290_uart_extra_ip;
extern struct max_gpio_init_param adxrs290_gpio_extra_ip;
extern struct max_spi_init_param adxrs290_spi_extra_ip;


#ifdef IIO_TRIGGER_EXAMPLE
#if (TARGET_NUM != 32655)
#error IIO_TRIGGER_EXAMPLE is currently supported only on max32655 targets.
#else
/* Setting for Port1 Pin9 used for Sync pin.
   Has to be adapted accordingly if another pin is used.
 */
#define NVIC_GPIO_IRQ   GPIO1_IRQn
#define GPIO_IRQ_ID     1
#endif
#define GPIO_IRQ_OPS    &max_gpio_irq_ops
#define GPIO_IRQ_EXTRA  &adxrs290_gpio_extra_ip

#define ADXRS290_GPIO_TRIG_IRQ_ID    GPIO_SYNC_PIN_NUM
#define ADXRS290_GPIO_CB_HANDLE      MXC_GPIO_GET_GPIO(GPIO_SYNC_PORT_NUM)
#endif

#endif /* __PARAMETERS_H__ */
