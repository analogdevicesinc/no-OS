/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-adxl38x examples.
 *   @author BRajendran (balarupini.rajendran@analog.com)
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
#include "adxl38x.h"

#ifdef MAXIM_PLATFORM
#include "maxim/parameters.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param adxl38x_uart_ip;
extern struct no_os_spi_init_param adxl38x_spi_ip;
extern struct adxl38x_init_param adxl38x_ip;

#endif /* __COMMON_DATA_H__ */
