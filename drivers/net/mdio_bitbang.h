/***************************************************************************//**
 *   @file   mdio_bitbang.h
 *   @brief  Header file for MDIO implementation using gpio bit banging.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MDIO_BITBANG_H_
#define _MDIO_BITBANG_H_

#include "no_os_gpio.h"

struct mdio_bitbang_init_param {
	struct no_os_gpio_init_param mdc;
	struct no_os_gpio_init_param mdio;
};

extern struct no_os_mdio_ops mdio_bitbang_ops;

#endif
