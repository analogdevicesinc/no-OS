/***************************************************************************//**
 *   @file   jesd204_clk.h
 *   @brief  Analog Devices JESD204 clock.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef JESD204_CLK_H_
#define JESD204_CLK_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "axi_adxcvr.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct jesd204_clk {
	struct adxcvr *xcvr;
	struct axi_jesd204_rx *jesd_rx;
	struct axi_jesd204_tx *jesd_tx;
};

/**
 * @brief jesd204 specific CLK platform ops structure
 */
extern const struct no_os_clk_platform_ops jesd204_clk_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Start the clock. */
int32_t jesd204_clk_enable(struct jesd204_clk *clk);
/* Stop the clock. */
int32_t jesd204_clk_disable(struct jesd204_clk *clk);
/* Change the frequency of the clock. */
int32_t jesd204_clk_set_rate(struct jesd204_clk *clk, uint32_t chan,
			     uint32_t rate);
#endif
