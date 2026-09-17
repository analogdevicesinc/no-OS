/***************************************************************************//**
 *   @file   ad74416h/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-ad74416h examples.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ad74416h.h"

extern struct no_os_uart_init_param ad74416h_uart_ip;
extern struct no_os_spi_init_param ad74416h_spi_ip;
extern struct ad74416h_init_param ad74416h_ip;
extern struct ad74416h_init_param ad74416h_ad1_ip;

#endif /* __COMMON_DATA_H__ */
