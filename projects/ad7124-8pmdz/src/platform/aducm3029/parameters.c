/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definitions specific to aducm3029 platform used by ad7124-8pmdz
 *           project.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct aducm_spi_init_param ad7124_spi_extra_ip = {
	.continuous_mode = true,
	.dma = false,
	.half_duplex = false,
	.master_mode = MASTER,
};
