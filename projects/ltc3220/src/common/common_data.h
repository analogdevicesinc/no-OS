/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by LTC3220 examples.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ltc3220.h"

extern const struct no_os_i2c_init_param iip;
extern const struct no_os_gpio_init_param gip;
extern struct ltc3220_init_param ltc3220_user_init;

#endif /* __COMMON_DATA_H__ */
