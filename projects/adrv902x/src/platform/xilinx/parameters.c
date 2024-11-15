/***************************************************************************//**
 *   @file   adrv902x/src/platform/xilinx/parameters.c
 *   @brief  Definition of XILINX platform data used by ad719x project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
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

struct xil_spi_init_param spi_extra = {
#ifdef PLATFORM_MB
	.type = SPI_PL,
#else
	.type = SPI_PS,
#endif
	.flags = 0
};

struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
	.type = GPIO_PL,
#else
	.type = GPIO_PS,
#endif
	.device_id = GPIO_DEVICE_ID,
};
