/***************************************************************************//**
 *   @file   adrv904x/src/platform/xilinx/parameters.c
 *   @brief  Definition of XILINX platform data used by adrv904x project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

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
#ifdef SDT
	.base_addr = XPAR_XSPI_0_BASEADDR,
#endif
#else
	.type = SPI_PS,
#ifdef SDT
	.base_addr = XPAR_XSPIPS_0_BASEADDR,
#endif
#endif
	.flags = 0,
};

struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
	.type = GPIO_PL,
#ifdef SDT
	.base_addr = XPAR_XGPIO_0_BASEADDR,
#endif
#else
	.type = GPIO_PS,
#ifdef SDT
	.base_addr = XPAR_XGPIOPS_0_BASEADDR,
#endif
#endif
	.device_id = GPIO_DEVICE_ID,
};
