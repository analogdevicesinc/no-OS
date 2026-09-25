/***************************************************************************//**
 *   @file   ltm3360b/src/common/common_data.h
 *   @brief  Defines common data to be used by LTM3360B examples.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_i2c.h"
#include "ltm3360b.h"

extern struct no_os_uart_init_param ltm3360b_uart_ip;
extern struct no_os_i2c_init_param ltm3360b_i2c_ip;
extern struct ltm3360b_init_param ltm3360b_ip;

#endif /* __COMMON_DATA_H__ */
