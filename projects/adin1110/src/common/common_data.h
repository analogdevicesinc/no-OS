/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by the ADIN1110 examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_gpio.h"

extern struct no_os_uart_init_param adin1110_uart_ip;
extern struct no_os_spi_init_param adin1110_spi_ip;
extern struct no_os_gpio_init_param adin1110_reset_gpio_ip;

#endif /* __COMMON_DATA_H__ */
