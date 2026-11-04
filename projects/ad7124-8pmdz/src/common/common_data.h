/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Declarations of common data to be used by ad7124-8pmdz examples.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "ad7124.h"
#include "ad7124_regs.h"

extern struct no_os_uart_init_param ad7124_uart_ip;
extern struct no_os_spi_init_param ad7124_spi_ip;
extern struct ad7124_init_param ad7124_ip;

#endif /* COMMON_DATA_H_ */
