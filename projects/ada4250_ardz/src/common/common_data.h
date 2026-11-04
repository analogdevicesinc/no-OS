/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ada4250_ardz examples.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "ada4250.h"
#include "no_os_spi.h"
#include "no_os_uart.h"

extern struct no_os_uart_init_param ada4250_uart_ip;
extern struct no_os_spi_init_param ada4250_spi_ip;
extern struct ada4250_init_param ada4250_ip;

#endif /* COMMON_DATA_H_ */
