/***************************************************************************//**
 *   @file   parameters.c
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
	.type = SPI_PS,
	.flags = 0U
};

struct xil_gpio_init_param gpio_extra = {
	.type = GPIO_PS,
	.device_id = GPIO_DEVICE_ID
};
