/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by adf4368 examples.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "adf4368.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_util.h"

extern struct no_os_uart_init_param	adf4368_uart_ip;
extern struct no_os_spi_init_param	adf4368_spi_ip;
extern struct adf4368_init_param 	adf4368_ip;

#endif /* __COMMON_DATA_H__ */
