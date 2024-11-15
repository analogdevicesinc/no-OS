/***************************************************************************//**
 * @file dac_buffer.h
 * @brief Header file of DAC LUT driver.
 * @author Istvan Csomortani (istvan.csomortani@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DAC_BUFFER_H_
#define DAC_BUFFER_H_

#include "dac_core.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

uint32_t dac_buffer_load(dac_core core, uint32_t start_address);

#endif

