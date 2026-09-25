/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by all examples.
 *   @author Monica Constandachi (monica.constandachi@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_print_log.h"

#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_gpio.h"
#include "maxim_spi.h"

#ifdef CONFIG_APARDSPOE_COLOR_SENSOR_EXAMPLE
#include "tcs34725.h"

extern struct tcs34725_init_param tcs34725_ip;
#endif

extern struct no_os_uart_init_param uart_ip;
extern struct adin1110_init_param adin1110_ip;
extern struct no_os_gpio_init_param adin1110_spi_cfg_0;
extern struct no_os_gpio_init_param port2_cfg_0;

int spi_cfg_0(const struct no_os_gpio_init_param adin1110_spi_cfg);
int port2_cfg(const struct no_os_gpio_init_param port2_gpio_cfg, int state);

#endif /* __COMMON_DATA_H__ */
