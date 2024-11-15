/***************************************************************************//**
 *   @file   aducm3029_uart_stdio.h
 *   @brief  Header file for UART driver stdout/stdin redirection.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _ADUCM3029_UART_STDIO_H_
#define _ADUCM3029_UART_STDIO_H_

#include "no_os_uart.h"

void no_os_uart_stdio(struct no_os_uart_desc *desc);

#endif //_ADUCM3029_UART_STDIO_H_
