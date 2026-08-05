/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ades1754 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ades1754.h"

extern struct no_os_uart_init_param ades1754_uart_ip;
extern struct no_os_uart_init_param ades1754_comm_ip;
extern struct ades1754_init_param ades1754_ip;

#endif /* __COMMON_DATA_H__ */
