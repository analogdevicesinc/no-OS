/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of MAXIM platform data used by adt7420-pmdz project.
 *   @author RNechita (ramona.nechita@analog.com)
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
struct max_uart_init_param xuip = {
	.flow = UART_FLOW_DIS,
};

struct max_i2c_init_param adt7420_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
