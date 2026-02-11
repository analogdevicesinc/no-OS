/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Header file of common data for eval-ublox-gnss project.
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
