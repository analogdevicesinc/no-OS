/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltc4296 examples.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_pwm.h"
#include "ltc4296.h"

extern struct no_os_uart_init_param ltc4296_uart_ip;
extern struct no_os_spi_init_param ltc4296_spi_ip;
extern struct ltc4296_init_param ltc4296_ip;

#endif /* __COMMON_DATA_H__ */
