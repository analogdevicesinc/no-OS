/***************************************************************************//**
 *   @file   ADXRS453.h
 *   @brief  Header file of ADXRS453 Driver.
 *   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ADXRS453_H_
#define ADXRS453_H_

#include "xparameters.h"
#include "system_config.h"

int ADXRS453_SendData(int txData);
void ADXRS453_Init(void);
int ADXRS453_ReadDegrees(void);
void delay_ms(u32 ms_count);
void PrintDegreesPos(int x);
void DemoProgram(void);

#endif /* ADXRS453_H_ */
