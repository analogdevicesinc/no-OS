/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to ADuCM3029 platform used by cn0531 project.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stdio.h"
#include "platform_init.h"
#include "aducm3029_gpio.h"
#include "aducm3029_spi.h"
#include "aducm3029_irq.h"
#include "aducm3029_uart.h"

#define UART_DEVICE_ID	0
#define UART_IRQ_ID		ADUCM_UART_INT_ID
#define UART_BAUDRATE	115200
#define UART_EXTRA      NULL
#define UART_OPS        &aducm_uart_ops

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    5000000
#define SPI_CS          0
#define SPI_OPS         &aducm_spi_ops
#define SPI_EXTRA       &cn0531_spi_extra_ip

#define GPIO_CLR_NUM    0x10
#define GPIO_LDAC_NUM   0x0E
#define GPIO_RESET_NUM  0x0C
#define GPIO_OPS        &aducm_gpio_ops
#define GPIO_EXTRA      NULL

extern struct aducm_spi_init_param cn0531_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
