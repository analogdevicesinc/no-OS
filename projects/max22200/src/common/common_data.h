/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Common data used by the MAX22200 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "max22200.h"

#ifdef IIO_SUPPORT
#include "iio_max22200.h"
#endif

extern struct no_os_uart_init_param max22200_uart_ip;
extern struct no_os_spi_init_param max22200_spi_ip;
extern struct no_os_gpio_init_param max22200_cmd_ip;
extern struct max22200_init_param max22200_ip;

#endif /* __COMMON_DATA_H__ */
