/***************************************************************************//**
 *   @file   adt7420-pmdz/src/common/common_data.h
 *   @brief  Defines common data to be used by adt7420-pmdz examples.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "adt7420.h"
#ifdef IIO_SUPPORT
#include "iio_adt7420.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param uip;

extern const struct no_os_i2c_init_param iip;
extern struct adt7420_init_param adt7420_user_init;

#endif /* __COMMON_DATA_H__ */
