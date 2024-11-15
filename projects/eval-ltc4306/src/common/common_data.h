/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-ltc4306-pmdz examples.
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "ltc4306.h"
#include "max538x.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param uip;

extern const struct no_os_i2c_init_param iip;
extern struct ltc4306_init_param ltc4306_user_init;
extern struct max538x_init_param max538x_user_init;

#endif /* __COMMON_DATA_H__ */
