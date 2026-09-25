/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltc2378 examples.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ltc2378.h"
#ifdef IIO_SUPPORT
#include "iio_ltc2378.h"
#endif

extern struct no_os_uart_init_param uip;

extern const struct no_os_spi_init_param ltc2378_spi_ip;
extern const struct no_os_gpio_init_param ltc2378_gpio_cnv;
extern const struct no_os_gpio_init_param ltc2378_gpio_busy;
extern const struct ltc2378_init_param ltc2378_ip;

#endif /* __COMMON_DATA_H__ */
