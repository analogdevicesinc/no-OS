/***************************************************************************//**
 *   @file   app_jesd.h
 *   @brief  Application JESD initialization.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#ifndef APP_JESD_H_
#define APP_JESD_H_

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

/* @brief Application JESD initialization. */
int32_t app_jesd_init(struct app_jesd **app, struct app_jesd_init *init_param);

/* @brief Application JESD remove. */
int32_t app_jesd_remove(struct app_jesd *app);

/* @brief Application JESD get status. */
uint32_t app_jesd_status(struct app_jesd *app);

#endif
