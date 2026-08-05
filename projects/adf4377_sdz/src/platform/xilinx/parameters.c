/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Xilinx platform data used by adf4377 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct xil_spi_init_param spi_extra = {
	.type = SPI_PS,
	.flags = 0U,
#ifdef SDT
	.base_addr = XPAR_XSPIPS_0_BASEADDR,
#endif
};

struct xil_gpio_init_param xil_gpio_init = {
	.device_id = GPIO_DEVICE_ID,
	.type = GPIO_PS,
#ifdef SDT
	.base_addr = XPAR_XGPIOPS_0_BASEADDR,
#endif
};
