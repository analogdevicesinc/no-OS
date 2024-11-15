/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-adxrs290-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct aducm_spi_init_param adxrs290_spi_extra_ip = {
	.continuous_mode = true,
	.dma = false,
	.half_duplex = false,
	.master_mode = MASTER
};

struct aducm_timer_init_param adxrs290_timer_extra_ip = {
	.source_freq = PCLK_DIV256,
};
