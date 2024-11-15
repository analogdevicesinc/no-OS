/***************************************************************************//**
 *   @file   app_clocking.h
 *   @brief  Clock setup and initialization routines.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __APP_CLOCKING_H
#define __APP_CLOCKING_H

#include <stdint.h>
#include "adi_hal.h"

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
#define DEV_REFCLK_A		0
#define DEV_SYSREF_A		1
#define DEV_REFCLK_B		2
#define DEV_SYSREF_B		3
#define JESD_REFCLK_TX_OBS_AB	4
#define JESD_REFCLK_RX_AB	5
#if defined(ZU11EG)
#define CORE_CLK_TX_OBS_AB	6
#define CORE_CLK_RX_AB		7
#define FPGA_SYSREF_TX_OBS_AB	8
#define FPGA_SYSREF_RX_AB	9
#elif defined(FMCOMMS8_ZCU102)
#define FPGA_SYSREF_TX_OBS_AB	6
#define FPGA_SYSREF_RX_AB	7
#define CORE_CLK_TX_OBS_AB	8
#define CORE_CLK_RX_AB		9
#endif
#else
#define DEV_CLK	13
#define FMC_CLK	1
#define DEV_SYSREF 12
#define FMC_SYSREF 3
#endif

adiHalErr_t clocking_init(uint32_t rx_div40_rate_hz,
			  uint32_t tx_div40_rate_hz,
			  uint32_t rx_os_div40_rate_hz,
			  uint32_t device_clock_khz,
			  uint32_t lmfc_rate_hz);

void clocking_deinit(void);

extern struct ad9528_dev* ad9528_device;

#endif /* __APP_CLOCKING_H */