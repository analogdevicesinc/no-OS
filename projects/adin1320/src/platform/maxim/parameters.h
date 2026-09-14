/***************************************************************************//**
*   @file   parameters.h
*   @brief  Definitions used by the ADIN1320 project.
*   @author John Roed Retuya (Johnroed.Retuya@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_gpio.h"
#include "maxim_gpio_irq.h"
#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_uart.h"

/* Platform operation structures (from maxim driver headers) */
#define SPI_OPS		&max_spi_ops
#define GPIO_OPS	&max_gpio_ops
#define IRQ_OPS		&max_irq_ops
#define GPIO_IRQ_OPS	&max_gpio_irq_ops
#define UART_OPS	&max_uart_ops

/* Platform extra initialization parameters (project specific) */
#define UART_EXTRA_IP	&max_uart_extra_ip
#define SPI_EXTRA_IP	&max_spi_extra_ip
#define GPIO_EXTRA_IP	&max_gpio_extra_ip

extern struct max_uart_init_param max_uart_extra_ip;
extern struct max_spi_init_param max_spi_extra_ip;
extern struct max_gpio_init_param max_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */
