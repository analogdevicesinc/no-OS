/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltc4162l examples.
 *   @author Marc Paolo Sosa (marcpaolo.sosa@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ltc4162l.h"
#ifdef IIO_SUPPORT
#include "iio_ltc4162l.h"
#endif

extern struct no_os_uart_init_param uip;

extern const struct no_os_i2c_init_param ltc4162l_i2c_ip;
extern struct ltc4162l_init_param ltc4162l_ip;

#endif /* __COMMON_DATA_H__ */
