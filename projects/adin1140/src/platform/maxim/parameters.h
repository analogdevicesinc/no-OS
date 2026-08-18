/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to the Maxim platform used by the ADIN1140
 *           project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "no_os_irq.h"
#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_i2c.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_dma.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#endif

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_OPS        &max_uart_ops
#define UART_EXTRA      &adin1140_uart_extra_ip

#if TARGET_NUM == 32690
#define SPI_DEVICE_ID   0
#define SPI_BAUDRATE    25000000
#else
#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    30000000
#endif
#define SPI_CS          1
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adin1140_spi_extra_ip

#define I2C_DEVICE_ID   1
#define I2C_CLK_SPEED   100000
#define I2C_OPS         &max_i2c_ops
#define I2C_EXTRA       &adin1140_i2c_extra_ip

#define ADIN1140_NVIC_IRQ_ID	0
#define ADIN1140_NVIC_IRQ_OPS	&max_irq_ops

#if TARGET_NUM == 32690
#define ADIN1140_INT_IRQ_ID	0
#define ADIN1140_INT_PIN	8
#define ADIN1140_INT_GPIO_IRQn	GPIO0_IRQn
#else
#define ADIN1140_INT_IRQ_ID	1
#define ADIN1140_INT_PIN	6
#define ADIN1140_INT_GPIO_IRQn	GPIO1_IRQn
#endif
#define ADIN1140_INT_IRQ_OPS	&max_gpio_irq_ops
#define ADIN1140_INT_IRQ_EXTRA	NULL
#define ADIN1140_INT_TRIG	NO_OS_IRQ_LEVEL_LOW

extern struct max_uart_init_param adin1140_uart_extra_ip;
extern struct max_spi_init_param adin1140_spi_extra_ip;
extern struct max_i2c_init_param adin1140_i2c_extra_ip;
extern struct no_os_dma_init_param adin1140_dma_ip;

#endif /* PARAMETERS_H_ */
