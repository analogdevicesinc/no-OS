/***************************************************************************//**
 *   @file   adgs6414d/src/common/common_data.h
 *   @brief  Defines common data to be used by adgs6414d examples.
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "adgs6414d.h"

extern struct no_os_uart_init_param adgs6414d_uart_ip;
extern struct no_os_spi_init_param adgs6414d_spi_ip;
extern struct adgs6414d_init_param adgs6414d_ip;

#endif /* __COMMON_DATA_H__ */
