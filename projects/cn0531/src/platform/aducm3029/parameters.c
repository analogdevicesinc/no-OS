/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of ADuCM3029 platform data used by cn0531 project.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct aducm_spi_init_param cn0531_spi_extra_ip = {
	.continuous_mode = true,
	.dma = false,
	.half_duplex = false,
	.master_mode = MASTER
};
