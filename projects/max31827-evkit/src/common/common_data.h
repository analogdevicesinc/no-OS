/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max31827 examples.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "max31827.h"
#ifdef IIO_SUPPORT
#include "iio_max31827.h"
#endif

extern struct no_os_uart_init_param uip;

extern const struct no_os_i2c_init_param max31827_i2c_ip;
extern struct max31827_init_param max31827_ip;

#endif /* __COMMON_DATA_H__ */
