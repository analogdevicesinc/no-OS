/***************************************************************************//**
 *   @file   ad5460/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-ad5460 examples.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "ad5460.h"

extern struct no_os_uart_init_param ad5460_uart_ip;
extern struct no_os_spi_init_param ad5460_spi_ip;
extern struct ad5460_init_param ad5460_ip;

#endif /* COMMON_DATA_H_ */
