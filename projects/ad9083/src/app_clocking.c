/***************************************************************************//**
 *   @file   app_clocking.c
 *   @brief  Clock setup and initialization routines.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include "no_os_spi.h"
#include "xilinx_spi.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "no_os_error.h"
#include "app_ad9083.h"
#include "app_clocking.h"
#include "inttypes.h"
#include "no_os_print_log.h"
#include "parameters.h"
#include "uc_settings.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define FPGA_SYSREF_CLK	0
#define FPGA_GLBL_CLK	1
#define FPGA_REF_CLK	3
#define ADC_SYSREF_CLK	12
#define ADC_REF_CLK	13

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the clocking app.
 * @param app - Clocking app descriptor.
 * @param init_param - The structure that contains the clocking app initial
 * 		       parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t app_clocking_init(struct app_clocking **app,
			  struct app_clocking_init *init_param)
{
	struct app_clocking *app_clocking;
	uint64_t dev_ref_clk, fpga_ref_clk, fpga_glb_clk;
	uint32_t sys_ref_rate;
	uint16_t n;
	int32_t ret;
	struct uc_settings* uc_settings = get_uc_settings();
	uint64_t *clk_hz = uc_settings->clk_hz[init_param->uc];

	struct ad9528_channel_spec ad9528_channels[14];
	struct ad9528_init_param ad9528_param;
	struct ad9528_platform_data ad9528_pdata;

	app_clocking = (struct app_clocking *)no_os_calloc(1, sizeof(*app_clocking));
	if (!app_clocking)
		return -1;

	// ad9528 defaults
	ad9528_param.pdata = &ad9528_pdata;
	ad9528_param.pdata->num_channels = 14;
	ad9528_param.pdata->channels = &ad9528_channels[0];

	ret = ad9528_init(&ad9528_param);
	if(ret) {
		pr_err("error: %"PRId32" ad9528_init() \n", ret);
		goto error_0;
	}

	// ad9528 channel defaults
	for(uint8_t ch = 0; ch < ad9528_param.pdata->num_channels; ch++) {
		ad9528_channels[ch].channel_num = ch;
		ad9528_channels[ch].output_dis = 1;
	}

	// ad9528 channel specifics

	// SYSREF to FPGA
	ad9528_channels[FPGA_SYSREF_CLK].output_dis = 0;
	ad9528_channels[FPGA_SYSREF_CLK].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[FPGA_SYSREF_CLK].divider_phase = 0;
	ad9528_channels[FPGA_SYSREF_CLK].signal_source = SOURCE_SYSREF_VCO;

	// GLBLCLK to FPGA
	ad9528_channels[FPGA_GLBL_CLK].output_dis = 0;
	ad9528_channels[FPGA_GLBL_CLK].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[FPGA_GLBL_CLK].divider_phase = 0;
	ad9528_channels[FPGA_GLBL_CLK].signal_source = SOURCE_VCO;

	// REFCLK to FPGA
	ad9528_channels[FPGA_REF_CLK].output_dis = 0;
	ad9528_channels[FPGA_REF_CLK].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[FPGA_REF_CLK].divider_phase = 0;
	ad9528_channels[FPGA_REF_CLK].signal_source = SOURCE_VCO;

	// SYSREF to ADC
	ad9528_channels[ADC_SYSREF_CLK].output_dis = 0;
	ad9528_channels[ADC_SYSREF_CLK].driver_mode = DRIVER_MODE_HSTL;
	ad9528_channels[ADC_SYSREF_CLK].divider_phase = 0;
	ad9528_channels[ADC_SYSREF_CLK].signal_source = SOURCE_SYSREF_VCO;

	// ADC CLK reference, used for ADC sample clock and JESD
	ad9528_channels[ADC_REF_CLK].output_dis = 0;
	ad9528_channels[ADC_REF_CLK].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[ADC_REF_CLK].divider_phase = 0;
	ad9528_channels[ADC_REF_CLK].signal_source = SOURCE_VCO;

	// ad9528 settings
	ad9528_param.pdata->spi3wire = 1;
	ad9528_param.pdata->vcxo_freq = 100000000;
	ad9528_param.pdata->refa_en = 1;
	ad9528_param.pdata->refa_diff_rcv_en = 1;
	ad9528_param.pdata->refa_r_div = 1;
	ad9528_param.pdata->osc_in_cmos_neg_inp_en = 1;
	ad9528_param.pdata->pll1_feedback_div = 4;
	ad9528_param.pdata->pll1_feedback_src_vcxo = 0; /* VCO */
	ad9528_param.pdata->pll1_charge_pump_current_nA = 5000;
	ad9528_param.pdata->pll1_bypass_en = 0;
	ad9528_param.pdata->pll2_vco_div_m1 = 4;
	ad9528_param.pdata->pll2_n2_div = 10;
	ad9528_param.pdata->pll2_r1_div = 1;
	ad9528_param.pdata->pll2_charge_pump_current_nA = 805000;
	ad9528_param.pdata->pll2_bypass_en = false;
	ad9528_param.pdata->sysref_src = SYSREF_SRC_INTERNAL;
	ad9528_param.pdata->sysref_pattern_mode = SYSREF_PATTERN_NSHOT;
	ad9528_param.pdata->sysref_nshot_mode = SYSREF_NSHOT_1_PULSE;
	ad9528_param.pdata->sysref_req_trigger_mode = SYSREF_LEVEL_HIGH;
	ad9528_param.pdata->rpole2 = RPOLE2_900_OHM;
	ad9528_param.pdata->rzero = RZERO_1850_OHM;
	ad9528_param.pdata->cpole1 = CPOLE1_16_PF;
	/* Status pin function selection */
	ad9528_param.pdata->stat0_pin_func_sel = 0xFF; /* No funtion */
	ad9528_param.pdata->stat1_pin_func_sel = 0xFF; /* No function */

	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
	};

	/* clock chip spi settings */
	struct no_os_spi_init_param clkchip_spi_init_param = {
		.device_id = 0,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = CLK_AD9258_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};

	ad9528_param.spi_init = clkchip_spi_init_param;
	/* Reset pin is tied up to P3V3_CLK_PLL trough a pull up resistor */
	ad9528_param.gpio_resetb = NULL;

	ret = ad9528_setup(&app_clocking->clkchip_device, ad9528_param);
	if(ret < 0) {
		pr_err("error: %"PRId32" ad9528_setup()\n", ret);
		goto error_0;
	}

	fpga_glb_clk = ad9528_clk_round_rate(app_clocking->clkchip_device,
					     FPGA_GLBL_CLK, clk_hz[1] / 4);
	fpga_ref_clk = ad9528_clk_round_rate(app_clocking->clkchip_device, FPGA_REF_CLK,
					     clk_hz[1]);
	dev_ref_clk = ad9528_clk_round_rate(app_clocking->clkchip_device, ADC_REF_CLK,
					    clk_hz[0]);

	ret = ad9528_clk_set_rate(app_clocking->clkchip_device, FPGA_GLBL_CLK,
				  fpga_glb_clk);
	if(ret < 0)
		goto error_1;

	ret = ad9528_clk_set_rate(app_clocking->clkchip_device, FPGA_REF_CLK,
				  fpga_ref_clk);
	if(ret < 0)
		goto error_1;
	ret = ad9528_clk_set_rate(app_clocking->clkchip_device, ADC_REF_CLK,
				  dev_ref_clk);
	if(ret < 0)
		goto error_1;

	for (n = 64; n > 0; n--) {
		sys_ref_rate = ad9528_clk_round_rate(app_clocking->clkchip_device,
						     FPGA_SYSREF_CLK, init_param->lmfc_rate_hz / n);
		if (app_ad9083_check_sysref_rate(init_param->lmfc_rate_hz, sys_ref_rate))
			break;

		if (n == 0) {
			printf("Could not find suitable SYSREF rate for LMFC of %u\n",
			       init_param->lmfc_rate_hz);
			pr_err("error: Could not find suitable SYSREF rate for LMFC of %u\n",
			       init_param->lmfc_rate_hz);
			goto error_1;
		}
	}

	ret = ad9528_clk_set_rate(app_clocking->clkchip_device, FPGA_SYSREF_CLK,
				  sys_ref_rate);
	if(ret < 0)
		goto error_1;

	ret = ad9528_clk_set_rate(app_clocking->clkchip_device, ADC_SYSREF_CLK,
				  sys_ref_rate);
	if(ret < 0)
		goto error_1;
	*app = app_clocking;

	return 0;

error_1:
	ad9528_remove(app_clocking->clkchip_device);
error_0:
	no_os_free(app_clocking);

	return -1;
}

/**
 * @brief Free the resources allocated by app_clocking_init().
 * @param app - App descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t app_clocking_remove(struct app_clocking *app)
{
	int32_t ret;

	if (!app)
		return -1;

	ret = ad9528_remove(app->clkchip_device);
	if (ret < 0)
		return ret;

	no_os_free(app);

	return 0;
}
