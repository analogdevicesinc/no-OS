/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ADHV4710 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#include "adhv4710.h"
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "maxim_irq.h"

/* UART init params */
extern struct no_os_uart_init_param uart_ip;
/* GPIO LED init params */
extern struct no_os_gpio_init_param gpio_led1_ip;
/* SPI init params */
extern struct no_os_spi_init_param adhv4710_spi_ip ;
/* GPIO RESET init params */
extern struct no_os_gpio_init_param gpio_reset_ip;

/* check for triggered alarms interval */
#define READ_INTERVAL                   2000000

/* Setup values for ADHV4710 */
/* Current resolution */
#define ADHV4710_CURRENT_RESOLUTION     15.625
/* Voltage resolution */
#define ADHV4710_VOLTAGE_RESOLUTION     1.953
/* Temperature resolution */
#define ADHV4710_TEMP_RESOLUTION        6.51
/* User defined values (Must be changed by user)*/
/* Overcurrent limit set to 100mA for both source and sink */
#define CURRENT_LIMIT_SOURCE            100
#define CURRENT_LIMIT_SINK              100
/* Overvoltage limit set to 20V for both negative and positive */
#define VOLTAGE_POSITIVE_LIMIT          20
#define VOLTAGE_NEGATIVE_LIMIT          20
/* Overtemperature limit 85℃ */
#define TEMPERATURE_LIMIT               88
#define TEMP_OFFSET                     266.64
/*  */
#endif /* __COMMON_DATA_H__ */
