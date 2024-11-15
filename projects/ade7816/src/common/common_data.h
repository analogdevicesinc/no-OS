/***************************************************************************//**
 *    @file  common_data.h
 *   @brief  Defines common data to be used by ade7816 example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "ade7816.h"

extern struct no_os_uart_init_param ade7816_uart_ip;
extern struct no_os_spi_init_param ade7816_spi_ip;
extern struct no_os_gpio_init_param ade7816_reset_ip;
extern struct no_os_gpio_init_param ade7816_gpio_irq0_ip;
extern struct no_os_gpio_init_param ade7816_gpio_irq1_ip;

extern struct ade7816_init_param ade7816_ip;

#endif /* __COMMON_DATA_H__ */
