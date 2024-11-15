/***************************************************************************//**
 *   @file   iio_aducm3029/src/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "iio_aducm3029.h"
#include "aducm3029_irq.h"

#define MAX_SIZE_BASE_ADDR		3000
#define UART_DEVICE_ID			0
#define INTC_DEVICE_ID			0
#define UART_IRQ_ID			ADUCM_UART_INT_ID
#define UART_BAUDRATE			115200
#define DEFAULT_SAMPLES			400
#define ADC_BUFF_SIZE			(ADUCM3029_ADC_NUM_CH * DEFAULT_SAMPLES)

#endif
