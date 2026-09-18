/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Common data definitions for ADAR300X project.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "platform_includes.h"
#include "adar300x.h"
#include "no_os_uart.h"

extern struct no_os_uart_init_param adar300x_uart_ip;
extern struct adar300x_init_param adar300x_ip;

#endif /* COMMON_DATA_H_ */
