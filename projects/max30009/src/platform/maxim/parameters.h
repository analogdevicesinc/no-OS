/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform-specific parameters for MAX30009 project
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_gpio.h"
#include "maxim_gpio_irq.h"
#include "maxim_i2c.h"
#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_uart.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID		0
#endif

/* I2C Configuration */
#define I2C_DEVICE_ID		2
#define I2C_SPEED		400000
#define I2C_OPS			&max_i2c_ops
#define I2C_EXTRA		&max30009_i2c_extra

/* SPI Configuration */
#define SPI_DEVICE_ID		0
#define SPI_BAUDRATE		1000000
#define SPI_CS			0
#define SPI_OPS			&max_spi_ops
#define SPI_EXTRA		&max30009_spi_extra

/* GPIO Configuration for SPI CSB */
#define GPIO_CSB_PORT_NUM	0
#define GPIO_CSB_PIN_NUM	20
#define GPIO_OPS		&max_gpio_ops

/* GPIO Configuration for Interrupt Pin */
#define GPIO_INTB_PORT_NUM	0
#define GPIO_INTB_PIN_NUM	25
#define NVIC_GPIO_IRQ		GPIO0_IRQn
#define GPIO_IRQ_ID		GPIO_INTB_PORT_NUM
#define MAX30009_GPIO_TRIG_IRQ_ID	GPIO_INTB_PIN_NUM
#define MAX30009_GPIO_CB_HANDLE		MXC_GPIO_GET_GPIO(GPIO_INTB_PORT_NUM)
#define GPIO_IRQ_OPS		&max_gpio_irq_ops
#define IRQ_OPS			&max_irq_ops
#define GPIO_IRQ_EXTRA		&max30009_gpio_extra_ip

/* UART Configuration */
#define UART_IRQ_ID		UART0_IRQn
#define UART_DEVICE_ID		0
#define UART_BAUDRATE		115200
#define UART_EXTRA		&max30009_uart_extra_ip
#define UART_OPS		&max_uart_ops

#define MAX30009_BOARD_NAME "MAX30009_EVKIT"
#define MAX30009_HW_CARRIER "MAX32655_FTHR"
#define MAX30009_ACTIVE_DEVICE_NAME "DEV_MAX30009"
#define MAX30009_ACTIVE_DEVICE_VENDOR "Analog Devices"

extern struct max_i2c_init_param max30009_i2c_extra;
extern struct max_spi_init_param max30009_spi_extra;
extern struct max_uart_init_param max30009_uart_extra_ip;
extern struct max_gpio_init_param max30009_gpio_csb_extra;
extern struct max_gpio_init_param max30009_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */
