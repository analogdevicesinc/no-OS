/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by adf4382 examples.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "adf4382.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_util.h"

extern struct no_os_uart_init_param	adf4382_uart_ip;
extern struct no_os_spi_init_param	adf4382_spi_ip;
extern struct adf4382_init_param 	adf4382_ip;

#endif /* __COMMON_DATA_H__ */
