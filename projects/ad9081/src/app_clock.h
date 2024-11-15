/***************************************************************************//**
 *   @file   app_clock.h
 *   @brief  Application clocks initialization.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef APP_CLOCK_H_
#define APP_CLOCK_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_clk.h"
#include "app_config.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Application clocks initialization. */
int32_t app_clock_init(struct no_os_clk dev_refclk[MULTIDEVICE_INSTANCE_COUNT]);

/* Application clocks remove. */
int32_t app_clock_remove(void);

#endif
