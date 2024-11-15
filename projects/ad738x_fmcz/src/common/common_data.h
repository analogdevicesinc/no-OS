/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-ad738x examples.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "ad738x.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADC_DDR_BASEADDR	in_buff
extern uint8_t in_buff[];
extern struct no_os_uart_init_param ad738x_uart_ip;
extern struct ad738x_init_param ad738x_init_param;
extern struct no_os_uart_desc *uart_desc;

#endif /* __COMMON_DATA_H__ */
