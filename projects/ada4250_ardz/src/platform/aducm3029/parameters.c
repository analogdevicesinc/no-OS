/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of ADuCM3029 platform data used by ada4250_ardz project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct aducm_spi_init_param ada4250_spi_extra_ip = {
	.continuous_mode = true,
	.dma = false,
	.half_duplex = false,
	.master_mode = MASTER
};
