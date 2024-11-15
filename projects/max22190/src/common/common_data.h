/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max22190 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "max22190.h"

#ifdef IIO_SUPPORT
#include "iio_max22190.h"
#endif

extern struct no_os_uart_init_param max22190_uart_ip;
extern struct no_os_spi_init_param max22190_spi_ip;
extern struct max22190_init_param max22190_ip;

#endif /* __COMMON_DATA_H__ */
