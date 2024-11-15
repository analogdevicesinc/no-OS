/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  ADA4250_ardz Parameters Definitions.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "aducm3029_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define UART_DEVICE_ID	0
#define INTC_DEVICE_ID	0
#define UART_IRQ_ID	ADUCM_UART_INT_ID
#define UART_BAUDRATE	115200

#endif /* PARAMETERS_H_ */
