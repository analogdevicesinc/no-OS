/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max22017 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "max22017.h"

extern struct no_os_uart_init_param max22017_uart_ip;
extern struct no_os_spi_init_param max22017_spi_ip;
extern struct no_os_gpio_init_param max22017_rstb_ip;
extern struct max22017_init_param max22017_ip;

#endif /* COMMON_DATA_H_ */
