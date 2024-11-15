/***************************************************************************//**
 *   @file   platform_init.h
 *   @brief  ADuCM3029 platform initialization header.
 *   @author Andrei Drimbarean (Andrei.Drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_INIT_H_
#define PLATFORM_INIT_H_

/* Initialize the power controller and set the core and peripherals clock
 * divider. */
int32_t platform_init();

#endif /* PLATFORM_INIT_H_ */
