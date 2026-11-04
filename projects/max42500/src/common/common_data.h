/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max42500 examples.
 *   @author Joshua Maniti (Joshua.Maniti@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "max42500.h"

extern struct no_os_uart_init_param uart_ip;

extern struct max42500_init_param max42500_ip;

#endif /* COMMON_DATA_H_ */
