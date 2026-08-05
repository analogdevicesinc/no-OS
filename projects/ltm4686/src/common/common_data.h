/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltm4686 examples.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_i2c.h"
#include "ltm4686.h"

#define LTM4686_ADDRESS                         0x4F

extern struct no_os_uart_init_param ltm4686_uart_ip;
extern struct no_os_i2c_init_param ltm4686_i2c_ip;
extern struct ltm4686_init_param ltm4686_ip;

#endif /* __COMMON_DATA_H__ */
