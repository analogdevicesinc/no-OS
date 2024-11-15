/***************************************************************************//**
 *   @file   ad796x_fmcz/src/common/common_data.h
 *   @brief  Defines common data to be used by iio examples.
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
#include "no_os_util.h"
#include "ad796x.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SAMPLES_PER_CHANNEL     SAMPLES_PER_CHANNEL_PLATFORM
extern struct no_os_uart_init_param iio_uart_ip;
extern struct ad796x_init_param ad796x_init_param;

#ifdef IIO_EXAMPLE
extern struct no_os_uart_init_param iio_uart_ip;
#endif

#endif /* __COMMON_DATA_H__ */
