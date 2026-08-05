/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by iio_demo project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param iio_demo_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
};

#ifdef CONFIG_USB_UART_MAXIM
struct max_usb_uart_init_param iio_demo_usb_uart_extra_ip = {
	.vid = 0x0B6B,
	.pid = 0x003C
};
#endif

