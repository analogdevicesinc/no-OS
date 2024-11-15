/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ad9545 example.
 *   @author Jonathan Santos (jonathan.santos@analog.com)
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
#include "ad9545.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param ad9545_uart_ip;
extern struct ad9545_init_param ad9545_ip;
extern const struct no_os_clk_platform_ops refb_clk_ops;
extern const struct no_os_clk_platform_ops refbb_clk_ops;
extern const struct no_os_clk_platform_ops refm1_clk_ops;

#define REFB_CLK_FREQUENCY 10000000
#define REFBB_CLK_FREQUENCY 1
#define REFM1_CLK_FREQUENCY 50000000

#endif /* __COMMON_DATA_H__ */
