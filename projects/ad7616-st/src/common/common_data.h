/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-ad7616 examples.
 *   @author Esteban Blanc (eblanc@baylibre.com)
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
#include "ad7616.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param ad7616_uart_ip;
extern struct no_os_spi_init_param ad7616_spi_ip;
extern struct no_os_gpio_init_param ad7616_gpio_reset_ip;
extern struct ad7616_init_param ad7616_ip;
extern struct no_os_uart_desc *uart_desc;

#endif /* __COMMON_DATA_H__ */
