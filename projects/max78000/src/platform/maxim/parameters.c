/* SPDX-License-Identifier: LicenseRef-ADI-BSD */

/**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by max78000 project.
 *   @author Victor Pascu (victor.pascu@analog.com)
 */

#include "parameters.h"

struct max_uart_init_param uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
