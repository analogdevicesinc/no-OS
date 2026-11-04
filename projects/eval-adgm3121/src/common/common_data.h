/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-adgm3121 example.
 *   @author Ciobanu Radu (Radu-rares.Ciobanu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "adgm3121.h"
#include "no_os_uart.h"

extern struct no_os_uart_init_param adgm3121_uart_ip;
extern struct adgm3121_init_param adgm3121_ip;

#endif /* COMMON_DATA_H_ */
