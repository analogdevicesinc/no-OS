/***************************************************************************//**
* @file parameters.h
* @brief Configuration parameters for AD7746-EBZ demo project.
* @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H
#define __PARAMETERS_H

#include "aducm3029_irq.h"

#define UART_DEVICE_ID	0
#define INTC_DEVICE_ID	0
#define UART_IRQ_ID	ADUCM_UART_INT_ID
#define UART_BAUDRATE	115200
#define I2C_SPEED	100000

#endif
