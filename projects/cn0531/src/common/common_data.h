/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by cn0531 examples.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "ad5791.h"
#include "iio_ad5791.h"
#include "no_os_uart.h"

extern struct no_os_uart_init_param cn0531_uart_ip;
extern struct ad5791_init_param cn0531_ad5791_ip;
extern struct ad5791_iio_init_param cn0531_ad5791_iio_ip;

#endif /* COMMON_DATA_H_ */
