/***************************************************************************//**
*   @file   common_data.h
*   @brief  Defines common data to be used by lt3074 examples.
*   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "no_os_i2c.h"
#include "parameters.h"
#include "lt3074.h"

#define LT3074_PMBUS_59K_261K_ADDRESS                   0x6E

extern struct no_os_uart_init_param lt3074_uart_ip;
extern struct no_os_i2c_init_param lt3074_i2c_ip;
extern struct lt3074_init_param lt3074_ip;

#endif /* __COMMON_DATA_H__ */
