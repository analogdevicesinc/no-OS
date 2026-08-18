/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by the ADIN1140 examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "no_os_spi.h"
#include "no_os_uart.h"

extern struct no_os_uart_init_param adin1140_uart_ip;
extern const struct no_os_spi_init_param adin1140_spi_ip;

#endif /* COMMON_DATA_H_ */
