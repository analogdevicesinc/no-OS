/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ad463x-fmcz examples.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "ad463x.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADC_DDR_BASEADDR	in_buff

#define SAMPLES_PER_CHANNEL     SAMPLES_PER_CHANNEL_PLATFORM
extern uint8_t in_buff[];
extern struct ad463x_init_param ad463x_init_param;
extern struct no_os_uart_init_param ad463x_uart_ip;
#ifndef USE_STANDARD_SPI
extern struct spi_engine_offload_init_param spi_engine_offload_init_param;
#endif
#endif /* __COMMON_DATA_H__ */
