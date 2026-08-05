/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Header file of common data for eval-ublox-gnss project.
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform.h"

/* UART parameters for console output (UART0) */
extern struct no_os_uart_init_param uart_console_ip;

/* UART parameters for GNSS communication (UART1) */
extern struct no_os_uart_init_param uart_gnss_ip;

/* GPIO parameters for GNSS reset */
extern struct no_os_gpio_init_param gpio_gnss_reset_ip;

/* GPIO parameters for GNSS led */
extern struct no_os_gpio_init_param gpio_led_ip;

/* IRQ parameters */
extern struct no_os_irq_init_param gnss_nvic_ip;

/* Console UART configuration (UART0) */
#define CONSOLE_UART_DEVICE_ID		0
#define CONSOLE_UART_BAUDRATE		115200
#define CONSOLE_UART_PARITY		NO_OS_UART_PAR_NO
#define CONSOLE_UART_STOP		NO_OS_UART_STOP_1_BIT

/* GNSS UART configuration (UART1) */
#define GNSS_UART_DEVICE_ID		1
#define GNSS_UART_BAUDRATE		38400
#define GNSS_UART_PARITY		NO_OS_UART_PAR_NO
#define GNSS_UART_STOP			NO_OS_UART_STOP_1_BIT

#define GNSS_PPS_1HZ         1
#define GNSS_PPS_LENGTH_100MS 100
#endif /* __COMMON_DATA_H__ */
