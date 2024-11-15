/***************************************************************************//**
 *   @file   app_jesd.h
 *   @brief  Application JESD initialization.
 *   @author Cristian Pop (cristian.pop@analog.com)
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
#include "jesd204_clk.h"
#include "axi_jesd204_rx.h"

/**
 * @struct app_jesd_init
 * @brief Structure holding the parameters for app jesd initialization.
 */
struct app_jesd_init {
	/* Uscase number */
	uint8_t uc;
	/* Lane rate */
	uint32_t lane_rate_khz;
};

/**
 * @struct app_jesd
 * @brief Structure holding jesd app descriptor.
 */
struct app_jesd {
	/* rx_jesd */
	struct axi_jesd204_rx *rx_jesd;
	/* rx_adxcvr */
	struct adxcvr *rx_adxcvr;
	/* rx_jesd_clk */
	struct jesd204_clk rx_jesd_clk;
	/* jesd_rx_hw */
	struct no_os_clk_hw jesd_rx_hw;
	/* jesd_rx_clk */
	struct no_os_clk jesd_rx_clk;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* @brief Application JESD initialization. */
int32_t app_jesd_init(struct app_jesd **app, struct app_jesd_init *init_param);

/* @brief Application JESD remove. */
int32_t app_jesd_remove(struct app_jesd *app);

/* @brief Application JESD get status. */
uint32_t app_jesd_status(struct app_jesd *app);

#endif
