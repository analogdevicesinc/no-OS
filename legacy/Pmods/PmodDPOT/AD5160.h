/***************************************************************************//**
 *   @file   AD5160.h
 *   @brief  Header File for AD5160 Driver.
 *   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "system_config.h"

#ifndef AD5160_H_
#define AD5160_H_

void AD5160_DisplayMainMenu(void);
void AD5160_DisplayMenu(void);
int AD5160_Set(void);
void AD5160_CalibrateRab(void);
void AD5160_CalibrateRwp(void);
int AD5160_GetValue(void);
void DemoProgram(void);

#endif /* AD5160_H_ */
