/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltc7841 examples.
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "ltc7841.h"
#include "parameters.h"

extern struct no_os_uart_init_param uart_ip;

extern struct ltc7841_init_param ltc7841_ip;

#endif /* COMMON_DATA_H_ */
