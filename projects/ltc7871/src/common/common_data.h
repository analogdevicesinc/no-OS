/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltc7871 examples.
 *   @author Aldrin Abacan (aldrin.abacan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "ltc7871.h"

extern struct no_os_uart_init_param ltc7871_uart_ip;
extern struct no_os_spi_init_param ltc7871_spi_ip;
extern struct no_os_gpio_init_param ltc7871_pwmen_ip;
extern struct ltc7871_init_param ltc7871_ip;
#endif /* COMMON_DATA_H_ */
