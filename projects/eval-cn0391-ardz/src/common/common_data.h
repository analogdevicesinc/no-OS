/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-cn0391-ardz examples.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ad7124.h"
#include "cn0391.h"

extern struct no_os_uart_init_param cn0391_uart_ip;
extern struct no_os_spi_init_param cn0391_spi_ip;
extern struct ad7124_init_param cn0391_ad7124_ip;
extern struct cn0391_init_param cn0391_ip;

#endif /* __COMMON_DATA_H__ */
