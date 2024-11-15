/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by iio_demo_freeRTOS project.
 *   @author Robert Budai (robert.budai@analog.com)
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
struct max_uart_init_param iio_demo_uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct max_gpio_init_param gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
