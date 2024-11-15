/***************************************************************************//**
 *   @file   chibios/chibios_uart.h
 *   @brief  Header file of UART driver for chibiOS
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _CHIBIOS_UART_H_
#define _CHIBIOS_UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_uart.h"
#include "hal.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

#if (HAL_USE_UART==TRUE)

/**
 * @struct chibios_uart_init_param
 * @brief Additional UART config parameters
 */
struct chibios_uart_init_param {
	/** UART instance */
	SerialDriver *huart;
	/** Serial Driver configuration*/
	SerialConfig *sdconfig;
};

/**
 * @struct chibios_uart_desc
 * @brief chibios platform specific UART descriptor
 */
struct chibios_uart_desc {
	/** Serial Driver instance */
	SerialDriver *huart;
	/** Serial Driver configuration*/
	SerialConfig *sdconfig;
};

/**
 * @brief chibios specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops chibios_uart_ops;

#endif //HAL_USE_UART==TRUE

#endif //_CHIBIOS_UART_H_
