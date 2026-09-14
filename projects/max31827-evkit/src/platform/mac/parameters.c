/*******************************************************************************
 *   @file   parameters.c
 *   @brief  Definition of mac platform data used by max31827 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
 *******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#include "parameters.h"

struct ftd2xx_i2c_init max31827_i2c_extra = {
	.channel_config = false,
	.extra_pins = true,
	.extra_pins_dir = 0xFF,
	.extra_pins_val = 0xBE
};
