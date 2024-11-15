/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by DC2903A project.
 *   @author JSanBuenaventura (jose.sanbuenaventura@analog.com)
 *   @author MSosa (marcpaolo.sosa@analog.com)
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
struct max_uart_init_param ltc2672_uart_extra_ip = {
	.flow = UART_FLOW_DIS,
};

struct max_spi_init_param ltc2672_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
