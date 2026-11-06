/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by DC2903A examples.
 *   @author JSanBuenaventura (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "ltc2672.h"

extern struct no_os_uart_init_param ltc2672_uart_ip;
extern struct no_os_spi_init_param ltc2672_spi_ip;
extern struct ltc2672_init_param ltc2672_ip;

#endif /* COMMON_DATA_H_ */
