/*******************************************************************************
 *   @file   ftd2xx_uart.h
 *   @brief  Implementation of UART driver for ftd2xx platform.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef FTD2XX_UART_H_
#define FTD2XX_UART_H_

#include "no_os_uart.h"

/**
 * @brief ftd2xx specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops ftd2xx_uart_ops;

#endif
