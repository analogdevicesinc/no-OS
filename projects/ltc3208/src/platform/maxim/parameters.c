/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by LTC3208 project
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_i2c_init_param ltc3208_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct max_gpio_init_param ltc3208_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
