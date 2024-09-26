/***************************************************************************//**
 *   @file   app_clocking.h
 *   @brief  Clock setup and initialization routines.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
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