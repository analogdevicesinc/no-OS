/***************************************************************************//**
 *   @file   app_jesd.h
 *   @brief  Application JESD initialization.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef APP_JESD_H_
#define APP_JESD_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_clk.h"

enum jesd_clocks {
	JESD_RX,
	JESD_TX,
	NUM_JESD_CLKS
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* @brief Application JESD initialization. */
int32_t app_jesd_init(struct no_os_clk clk[2],
		      uint32_t reference_clk_khz,
		      uint32_t rx_device_clk_khz,
		      uint32_t tx_device_clk_khz,
		      uint32_t rx_lane_clk_khz,
		      uint32_t tx_lane_clk_khz);

#endif
