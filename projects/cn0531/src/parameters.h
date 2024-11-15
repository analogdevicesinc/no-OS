/***************************************************************************//**
 *   @file   cn0531/src/parameters.h
 *   @brief  Parameters Definitions.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#ifdef ADUCM_PLATFORM
#include "aducm3029_irq.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef ADUCM_PLATFORM

#define UART_DEVICE_ID	0
#define INTC_DEVICE_ID	0
#define UART_IRQ_ID		ADUCM_UART_INT_ID
#define UART_BAUDRATE	115200

#endif //ADUCM_PLATFORM

#define WIFI_SSID	"RouterSSID"
#define WIFI_PWD	"******"

#endif // __PARAMETERS_H__
