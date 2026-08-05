/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ad74413r examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ad74413r.h"

extern struct no_os_uart_init_param ad74413r_uart_ip;

extern struct no_os_spi_init_param ad74413r_spi_ip;
extern struct ad74413r_init_param ad74413r_ip;

#endif /* __COMMON_DATA_H__ */
