/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Common data header for MAX30009 project
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "max30009.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

extern struct max30009_init_param max30009_init_params;
extern struct no_os_uart_init_param max30009_uart_ip;
extern struct no_os_irq_init_param max30009_gpio_irq_ip;
extern struct no_os_gpio_init_param max30009_gpio_ip;

#endif /* __COMMON_DATA_H__ */
