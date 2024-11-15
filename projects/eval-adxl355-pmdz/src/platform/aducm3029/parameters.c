/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of aducm3029 platform data used by eval-adxl355-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
struct aducm_spi_init_param adxl355_spi_extra_ip = {
	.continuous_mode = true,
	.dma = false,
	.half_duplex = false,
	.master_mode = MASTER
};

#ifdef IIO_TRIGGER_EXAMPLE
struct no_os_gpio_init_param adxl355_gpio_drdy_ip = {
	.port = GPIO_DRDY_PORT_NUM,
	.number = GPIO_DRDY_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
};
#endif
