/*******************************************************************************
 *   @file   parameters.c
 *   @brief  Definition of linux-userspace platform parameters for max31827-evkit.
 *   @author Radu Sabau (radu.sabau@analog.com)
 *******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#include "parameters.h"

#ifdef CONFIG_FTD2XX
struct ftd2xx_i2c_init max31827_i2c_extra = {
	.channel_config = false,
	.extra_pins = true,
	.extra_pins_dir = 0xFF,
	.extra_pins_val = 0xBE
};
#endif
