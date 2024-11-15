/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltc3337 examples.
 *   @author Brent Kowal (brent.kowal@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "ltc3337.h"

extern struct no_os_uart_init_param uip;

extern const struct no_os_i2c_init_param ltc3337_i2c_ip;
extern struct ltc3337_init_param ltc3337_ip;

#endif /* __COMMON_DATA_H__ */
