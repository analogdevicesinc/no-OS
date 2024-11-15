/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by adt75 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
struct max_uart_init_param adt75_uart_extra_ip = {
	.flow = UART_FLOW_DIS,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_i2c_init_param adt75_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
