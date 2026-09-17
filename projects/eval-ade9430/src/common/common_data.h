/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by EVAL-ADE9430 example projects.
 *   @author RaduE (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "ade9430.h"
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_gpio.h"
#include "maxim_spi.h"

/* UART configuration (stdio/debug console) */
#define UART_DEV_ID		0
#define UART_BAUD		115200
#define UART_IRQ		UART0_IRQn

/* SPI configuration for the ADE9430 energy meter */
#define SPI_DEVICE_ID		1
#define SPI_BAUDRATE		1000000
#define SPI_CS			0

#ifdef HW_RESET_ENABLED
/* ADE9430 hardware reset pin (EVAL-PQMON board) */
#define GPIO_RESET_PORT		1
#define GPIO_RESET_NUMBER	27
#endif

/* Number of samples used by the energy accumulation model */
#define ADE9430_SAMPLES_NR	7999

/* Delay (ms) to allow the ADE9430 to settle after power-up */
#define RESET_TIME		500

/* Delay (ms) between consecutive readings */
#define READ_INTERVAL		1000

extern struct no_os_uart_init_param uart_ip;
extern struct no_os_spi_init_param ade9430_spi_ip;
#ifdef HW_RESET_ENABLED
extern struct no_os_gpio_init_param gpio_reset_ip;
#endif

#endif /* __COMMON_DATA_H__ */
