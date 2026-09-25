/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ad8460 examples.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ad8460.h"
#ifdef IIO_SUPPORT
#include "iio_ad8460.h"
#endif

extern struct no_os_uart_init_param uip;

extern const struct no_os_spi_init_param ad8460_spi_ip;
extern const struct no_os_gpio_init_param ad8460_gpio_rstn;
extern struct ad8460_init_param ad8460_ip;

#endif /* __COMMON_DATA_H__ */
