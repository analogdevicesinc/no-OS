/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by lt8722 examples.
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "lt8722.h"

extern struct no_os_uart_init_param lt8722_uart_ip;
extern struct no_os_spi_init_param lt8722_spi_ip;
extern struct no_os_gpio_init_param lt8722_swen_ip;
extern struct no_os_gpio_init_param lt8722_en_ip;
extern struct lt8722_init_param lt8722_ip;

#endif /* __COMMON_DATA_H__ */
