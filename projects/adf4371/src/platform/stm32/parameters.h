/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of STM32 platform data used by adf4371 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_gpio.h"
#include "stm32_irq.h"
#include "stm32_spi.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

extern UART_HandleTypeDef 	huart5;

#define UART_IRQ_ID     	UART5_IRQn

#define UART_DEVICE_ID  	5
#define UART_BAUDRATE   	115200
#define UART_EXTRA      	&adf4371_uart_extra_ip
#define UART_OPS        	&stm32_uart_ops

#define SPI_DEVICE_ID   	5
#define SPI_BAUDRATE    	1000000
#define SPI_CS          	9
#define SPI_CS_PORT     	1
#define SPI_OPS         	&stm32_spi_ops
#define SPI_EXTRA       	&adf4371_spi_extra_ip

#define GPIO_CE_PORT_NUM	9
#define GPIO_CE_PIN_NUM		3
#define GPIO_OPS		&stm32_gpio_ops
#define GPIO_EXTRA		&adf4371_gpio_extra_ip

extern struct stm32_gpio_init_param 	adf4371_gpio_extra_ip;
extern struct stm32_uart_init_param	adf4371_uart_extra_ip;
extern struct stm32_spi_init_param	adf4371_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
