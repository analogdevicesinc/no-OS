/***************************************************************************//**
 *   @file   axi_adxcvr.h
 *   @brief  Driver for the ADI AXI-ADXCVR Module.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AXI_ADXCVR_H_
#define AXI_ADXCVR_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "xilinx_transceiver.h"

/******************************************************************************/
/********************** Macros and Types Declarations *************************/
/******************************************************************************/

// Selection of PLL reference clock source to drive the RXOUTCLK
#define ADXCVR_SYS_CLK_CPLL			0x00
#define ADXCVR_SYS_CLK_QPLL1		0x02
#define ADXCVR_SYS_CLK_QPLL0		0x03

// adi,out-clk-select
#define ADXCVR_OUTCLK_PCS		1
#define ADXCVR_OUTCLK_PMA		2
#define ADXCVR_REFCLK		3
#define ADXCVR_REFCLK_DIV2	4
#define ADXCVR_PROGDIV_CLK	5 /* GTHE3, GTHE4, GTYE4 only */

/**
 * @struct adxcvr
 * @brief ADI JESD204B/C AXI_ADXCVR Highspeed Transceiver Device structure.
 */
struct adxcvr {
	/** Device Name */
	const char *name;
	/** Base address */
	uint32_t base;
	/** Enable CPLL for the transceiver */
	bool cpll_enable;
	/** Enable QPLL for the transceiver */
	bool qpll_enable;
	/** TX Enable */
	bool tx_enable;
	/** Enable LPM mode for the transceiver. Otherwise use DFE. */
	bool lpm_enable;
	/** Number of lanes */
	uint32_t num_lanes;
	/** Lane rate in KHz */
	uint32_t lane_rate_khz;
	/** Reference Clock rate */
	uint32_t ref_rate_khz;
	/** Select the PLL reference clock source to be forwarded to the OUTCLK
	 * MUX: 0-CPLL, 3-QPLL0.
	 */
	uint32_t sys_clk_sel;
	/** Controls the OUTCLKSEL multiplexer, controlling what will be
	 * forwarded to OUTCLK pin.
	 */
	uint32_t out_clk_sel;
	/** Structure holding the configuration of the Xilinx Transceiver. */
	struct xilinx_xcvr xlx_xcvr;
	/** Exported no-OS output clock */
	struct no_os_clk_desc *clk_out;
};

/**
 * @struct adxcvr
 * @brief ADI JESD204B/C AXI_ADXCVR Highspeed Transceiver Initialization
 *        structure.
 */
struct adxcvr_init {
	/** Device Name */
	const char *name;
	/** Base address */
	uint32_t base;
	/** Select the PLL reference clock source to be forwarded to the OUTCLK
	 * MUX: 0-CPLL, 2-QPLL1 (GTH and GTY), 3-QPLL0.
	 */
	uint32_t sys_clk_sel;
	/** Controls the OUTCLKSEL multiplexer, controlling what will be
	 * forwarded to OUTCLK pin.
	 */
	uint32_t out_clk_sel;
	/** Enable LPM mode for the transceiver. Otherwise use DFE. */
	bool lpm_enable;
	/** Lane rate in KHz */
	uint32_t lane_rate_khz;
	/** Reference Clock rate */
	uint32_t ref_rate_khz;
	/** Export no-OS output clock */
	bool export_no_os_clk;
};

/**
 * @brief adxcvr clock ops
 */
extern const struct no_os_clk_platform_ops adxcvr_clk_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/** AXI ADXCVR DPR Port Read */
int adxcvr_drp_read(struct adxcvr *xcvr,
		    unsigned int drp_port,
		    unsigned int reg,
		    unsigned int *val);
/** AXI ADXCVR DPR Port Write */
int adxcvr_drp_write(struct adxcvr *xcvr,
		     unsigned int drp_port,
		     unsigned int reg,
		     unsigned int val);
/** AXI ADXCVRS Status Read */
int32_t adxcvr_status_error(struct adxcvr *xcvr);
/** AXI ADXCVR Clock Enable */
int adxcvr_clk_enable(struct adxcvr *xcvr);
/** AXI ADXCVR Clock Disable */
int adxcvr_clk_disable(struct adxcvr *xcvr);
/** AXI ADXCVR Device Initialization */
int32_t adxcvr_init(struct adxcvr **ad_xcvr,
		    const struct adxcvr_init *init);
/** AXI ADXCVR Resources Deallocation */
int32_t adxcvr_remove(struct adxcvr *xcvr);
/** AXI ADXCVR Clock Set Rate */
int adxcvr_clk_set_rate(struct adxcvr *xcvr,
			unsigned long rate,
			unsigned long parent_rate);
/** AXI ADXCVR Write */
int32_t adxcvr_write(struct adxcvr *xcvr, uint32_t reg_addr, uint32_t reg_val);
/** AXI ADXCVR Read */
int32_t adxcvr_read(struct adxcvr *xcvr, uint32_t reg_addr, uint32_t *reg_val);
#endif
