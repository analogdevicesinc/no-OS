/***************************************************************************//**
 *   @file   eval-adxl355-pmdz/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adxl355-pmdz examples.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "adxl355.h"

extern struct no_os_uart_init_param adxl355_uart_ip;
extern struct no_os_spi_init_param adxl355_spi_ip;
extern struct adxl355_init_param adxl355_ip;

#endif /* __COMMON_DATA_H__ */
