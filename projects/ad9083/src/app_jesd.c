/***************************************************************************//**
 *   @file   app_jesd.c
 *   @brief  Application JESD setup.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include "axi_jesd204_rx.h"
#include "axi_adxcvr.h"
#include "jesd204_clk.h"
#include "no_os_error.h"
#include "app_jesd.h"
#include "adi_cms_api_common.h"
#include "axi_jesd204_rx.h"
#include "no_os_print_log.h"
#include "parameters.h"
#include "uc_settings.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Application JESD setup.
 * @param app - JESD app descriptor.
 * @param init_param - The structure that contains the JESD app initial
 * 		       parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t app_jesd_init(struct app_jesd **app, struct app_jesd_init *init_param)
{
	int32_t status;
	struct app_jesd *app_jesd;
	struct uc_settings* uc_settings = get_uc_settings();
	adi_cms_jesd_param_t *jtx_param = &uc_settings->jtx_param[init_param->uc];
	uint64_t *clk_hz = uc_settings->clk_hz[init_param->uc];

	app_jesd = (struct app_jesd *)no_os_calloc(1, sizeof(*app_jesd));
	if (!app_jesd)
		return -1;

	struct jesd204_rx_init rx_jesd_init = {
		.name = "rx_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = jtx_param->jesd_f,
		.frames_per_multiframe = jtx_param->jesd_k,
		.subclass = jtx_param->jesd_subclass,
		.device_clk_khz = init_param->lane_rate_khz / 40,	/* (lane_clk_khz / 40) */
		.lane_clk_khz = init_param->lane_rate_khz,		/* LaneRate = (M/L)*NP*(10/8)*DataRate */
	};

	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK_DIV2,
		.lpm_enable = 1,
		.lane_rate_khz = init_param->lane_rate_khz,	/* LaneRate = (M/L)*NP*(10/8)*DataRate */
		.ref_rate_khz = clk_hz[1] / 1000,		/* FPGA_CLK ref */
	};

	status = axi_jesd204_rx_init_legacy(&app_jesd->rx_jesd, &rx_jesd_init);
	if (status != 0) {
		pr_err("error: %s: axi_jesd204_rx_init_legacy() failed\n", rx_jesd_init.name);
		goto error_0;
	}

	status = adxcvr_init(&app_jesd->rx_adxcvr, &rx_adxcvr_init);
	if (status != 0) {
		pr_err("error: %s: adxcvr_init() failed\n", rx_adxcvr_init.name);
		goto error_1;
	}

	app_jesd->rx_jesd_clk.xcvr = app_jesd->rx_adxcvr;
	app_jesd->rx_jesd_clk.jesd_rx = app_jesd->rx_jesd;
	app_jesd->jesd_rx_hw.dev = &app_jesd->rx_jesd_clk;
	app_jesd->jesd_rx_hw.dev_clk_enable = jesd204_clk_enable;
	app_jesd->jesd_rx_hw.dev_clk_disable = jesd204_clk_disable;
	app_jesd->jesd_rx_hw.dev_clk_set_rate = jesd204_clk_set_rate;
	app_jesd->jesd_rx_clk.name = "jesd_rx";
	app_jesd->jesd_rx_clk.hw = &app_jesd->jesd_rx_hw;

	*app = app_jesd;

	return 0;

error_1:
	axi_jesd204_rx_remove(app_jesd->rx_jesd);
error_0:
	no_os_free(app_jesd);

	return -1;
}

uint32_t app_jesd_status(struct app_jesd *app)
{
	return axi_jesd204_rx_status_read(app->rx_jesd);
}

/**
 * @brief Free the resources allocated by app_jesd_init().
 * @param app - App descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t app_jesd_remove(struct app_jesd *app)
{
	int32_t ret;

	if (!app)
		return -1;

	ret = axi_jesd204_rx_remove(app->rx_jesd);
	if (ret < 0)
		return ret;

	ret = adxcvr_remove(app->rx_adxcvr);
	if (ret < 0)
		return ret;

	no_os_free(app);

	return 0;
}
