/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-adg2404 project.
 *   @author Raul Geo <Raul.Georgian@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "adg2404.h"
#include "no_os_uart.h"
#include "parameters.h"

extern struct adg2404_init_param adg2404_ip;
extern struct no_os_uart_init_param adg2404_uart_ip;

#endif /* __COMMON_DATA_H__ */
