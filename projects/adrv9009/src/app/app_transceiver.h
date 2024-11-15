/***************************************************************************//**
 *   @file   app_transceiver.h
 *   @brief  FPGA XCVR setup and initialization routines.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __APP_TRANSCEIVER_H
#define __APP_TRANSCEIVER_H

#include <stdint.h>
#include "adi_hal.h"

adiHalErr_t fpga_xcvr_init(uint32_t rx_lane_rate_khz,
			   uint32_t tx_lane_rate_khz,
			   uint32_t rx_os_lane_rate_khz,
			   uint32_t device_clock);
void fpga_xcvr_deinit(void);

#endif /* __APP_TRANSCEIVER_H */