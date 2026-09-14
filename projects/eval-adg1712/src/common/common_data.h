/***************************************************************************//**
 *   @file   eval-adg1712/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adg1712 examples.
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_uart.h"
#include "adg1712.h"

extern struct no_os_uart_init_param adg1712_uart_ip;
extern struct adg1712_init_param adg1712_init_param;

#endif /* __COMMON_DATA_H__ */
