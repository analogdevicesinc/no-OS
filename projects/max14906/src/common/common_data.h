/***************************************************************************//**
 *   @file   max14906/src/common/common_data.h
 *   @brief  Defines common data to be used by max14906 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "max149x6-base.h"
#include "max14906.h"

#ifdef IIO_SUPPORT
#include "iio_max14906.h"
#endif

extern struct no_os_uart_init_param max14906_uart_ip;
extern struct no_os_spi_init_param max14906_spi_ip;
extern struct no_os_gpio_init_param max14906_fault_gpio_param;
extern struct max149x6_init_param max14906_ip;

#endif /* __COMMON_DATA_H__ */
