/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-adxl37x examples.
 *   @author Raul Georgian (Raul.Georgian@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "adxl37x.h"

/* Specific for sample handling*/

/*
 * When reading data from multiple axes from the FIFO,
 * to ensure that data is not overwritten and stored out
 * of order, at least one sample set must be left in the
 * FIFO after every read.
 */

#define AXES_USED 3
#define BYTES_PER_SAMPLE (2 * AXES_USED)

/* The maximum number of samples in the FIFO, according to the number of
 * axes used.
 */
#if AXES_USED == 3
#define MAX_SAMPLES 170
#elif AXES_USED == 2
#define MAX_SAMPLES 256
#elif AXES_USED == 1
#define MAX_SAMPLES 512
#endif

/* At least 1 sample left in the FIFO for sample integrity. */
#define EXTRACTED_SAMPLES (MAX_SAMPLES - 1)

extern struct no_os_uart_init_param adxl37x_uart_ip;
extern struct adxl37x_init_param adxl37x_init_param;

#endif /* __COMMON_DATA_H__ */
