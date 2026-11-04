/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ADRV903X examples.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "platform_includes.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

extern const struct no_os_spi_init_param ad9528_spi_param;
extern struct no_os_gpio_init_param clkchip_gpio_init_param;

#endif /* COMMON_DATA_H_ */
