/***************************************************************************//**
 *   @file   chibios/chibios_uart.h
 *   @brief  Header file of UART driver for chibiOS
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
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
