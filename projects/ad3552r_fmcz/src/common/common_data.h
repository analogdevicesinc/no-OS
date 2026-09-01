/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by iio examples.
 *   @author Angelo Dureghello (adureghello@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_util.h"
#include "ad3552r.h"

#define SAMPLES_PER_CHANNEL     SAMPLES_PER_CHANNEL_PLATFORM

extern struct no_os_gpio_init_param default_gpio_param;
extern struct no_os_uart_init_param uart_init_param;
extern struct axi_dac_init ad3552r_core_ip;
extern struct axi_clkgen_init clkgen_ip;
extern struct axi_dmac_init dmac_ip;
extern struct no_os_gpio_init_param gpio_ldac_param;
extern struct no_os_gpio_init_param gpio_reset_param;

#endif /* __COMMON_DATA_H__ */
