/***************************************************************************//**
 *   @file   app_jesd.h
 *   @brief  JESD setup and initialization routines.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __APP_JESD_H
#define __APP_JESD_H

#include <stdint.h>
#include "adi_hal.h"

extern struct axi_jesd204_rx *rx_jesd;
extern struct axi_jesd204_tx *tx_jesd;
extern struct axi_jesd204_rx *rx_os_jesd;

adiHalErr_t jesd_init(uint32_t rx_div40_rate_hz,
		      uint32_t tx_div40_rate_hz,
		      uint32_t rx_os_div40_rate_hz);

void jesd_deinit(void);
void jesd_status(void);
void jesd_rx_watchdog(void);

#endif /* __APP_JESD_H */