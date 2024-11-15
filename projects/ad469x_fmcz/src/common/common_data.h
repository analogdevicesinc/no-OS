/***************************************************************************//**
 *   @file   ad469x_fmcz/src/common/common_data.h
 *   @brief  Defines common data to be used by ad469x_fmcz examples.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "ad469x.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param iio_uart_ip;
extern struct ad469x_init_param ad469x_init_param;

#endif /* __COMMON_DATA_H__ */
