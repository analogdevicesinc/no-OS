/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines the common data used for examples eval-adf4377 project
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "adf4377.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_util.h"

extern struct no_os_uart_init_param	adf4377_uart_ip;
extern struct no_os_spi_init_param	adf4377_spi_ip;
extern struct adf4377_init_param 	adf4377_ip;

#endif /* __COMMON_DATA_H__ */
