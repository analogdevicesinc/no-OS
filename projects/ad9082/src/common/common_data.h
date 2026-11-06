/***************************************************************************//**
 *   @file   ad9082/src/common/common_data.h
 *   @brief  Shared bring-up (clocks + JESD204) for the AD9082 VPK180 project.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include <stdint.h>
#include "no_os_clk.h"
#include "app_config.h"
#include "hmc7044.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"

/* HMC7044 clock chip (SPI1) and the two AXI-JESD204 link cores. */
extern struct hmc7044_dev *hmc7044_dev;
extern struct axi_jesd204_rx *rx_jesd;
extern struct axi_jesd204_tx *tx_jesd;

/* Configure the on-board HMC7044 and export the MxFE device reference clock. */
int32_t app_clock_init(struct no_os_clk dev_refclk[MULTIDEVICE_INSTANCE_COUNT]);

/* Bring up the AXI-JESD204 RX/TX link cores (Versal: GT reset via GPIO). */
int32_t app_jesd_init(struct no_os_clk clk[2],
		      uint32_t reference_clk_khz,
		      uint32_t rx_device_clk_khz,
		      uint32_t tx_device_clk_khz,
		      uint32_t rx_lane_clk_khz,
		      uint32_t tx_lane_clk_khz);

/* Entry point implemented by the selected example (basic/dma/iio). */
int example_main(void);

#endif /* __COMMON_DATA_H__ */
