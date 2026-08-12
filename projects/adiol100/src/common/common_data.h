/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  ADIOL100 project common data declarations.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "adiol100.h"

extern struct no_os_uart_init_param adiol100_uart_ip;
extern struct no_os_spi_init_param  adiol100_spi_ip;
extern struct adiol100_init_param   adiol100_ip;

#endif
