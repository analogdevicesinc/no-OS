/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example header for ad7091r8-sdz project
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "basic_example.h"
#include "common_data.h"
#include "ad9545.h"
#include "no_os_clk.h"
#include "no_os_clk_fixed.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include <stdbool.h>

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int basic_example_main()
{
	struct ad9545_dev *ad9545_dev;
	struct no_os_clk_desc *ref_a_clk;
	const struct no_os_clk_fixed_param input_refa = {
		.name = "Ref-A",
		.clk_frequency = 10000000
	};
	int ret;

	/* Ref. Clock */
	ad9545_ip.sys_clk.sys_clk_crystal = true;
	ad9545_ip.sys_clk.ref_freq_hz = 52000000;
	/* NCO params */
	ad9545_ip.aux_nco_clks[AD9545_NCO0].nco_used = true;
	ad9545_ip.aux_nco_clks[AD9545_NCO0].address = AD9545_NCO0;
	ad9545_ip.aux_nco_clks[AD9545_NCO0].freq_thresh_ps = 16000000;
	ad9545_ip.aux_nco_clks[AD9545_NCO0].phase_thresh_ps = 16000000;
	/* PLL params */
	/* PLL1 */
	ad9545_ip.pll_clks[AD9545_PLL1].pll_used = true;
	ad9545_ip.pll_clks[AD9545_PLL1].address = AD9545_PLL1;
	/* PLL1 Profiles */
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].en = 1;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].address = 0;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].tdc_source = 4;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].priority = 0;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].loop_bw_uhz = 200000000;
	/* Output Clocks params */
	/* OUT Q1A */
	ad9545_ip.out_clks[AD9545_Q1A].output_used = true;
	ad9545_ip.out_clks[AD9545_Q1A].address = AD9545_Q1A;
	ad9545_ip.out_clks[AD9545_Q1A].output_mode = DRIVER_MODE_DUAL_DIV;
	ad9545_ip.out_clks[AD9545_Q1A].source_ua = 15000;
	/* OUT Q1B */
	ad9545_ip.out_clks[AD9545_Q1B].output_used = true;
	ad9545_ip.out_clks[AD9545_Q1B].address = AD9545_Q1B;
	ad9545_ip.out_clks[AD9545_Q1B].output_mode = DRIVER_MODE_DUAL_DIV;
	ad9545_ip.out_clks[AD9545_Q1B].source_ua = 15000;

	/* Input Reference Clock */
	ad9545_ip.ref_in_clks[0].address = 0;
	ad9545_ip.ref_in_clks[0].mode = AD9545_SINGLE_ENDED;
	ad9545_ip.ref_in_clks[0].r_div_ratio = 200;
	ad9545_ip.ref_in_clks[0].d_tol_ppb = 10000000;
	ad9545_ip.ref_in_clks[0].monitor_hyst_scale = 87500;
	ad9545_ip.ref_in_clks[0].valid_t_ms = 1;
	ad9545_ip.ref_in_clks[0].freq_thresh_ps = 0xFFFFFF;
	ad9545_ip.ref_in_clks[0].phase_thresh_ps = 0xFFFFFF;
	ad9545_ip.ref_in_clks[0].freq_lock_fill_rate = 20;
	ad9545_ip.ref_in_clks[0].freq_lock_drain_rate = 20;
	ad9545_ip.ref_in_clks[0].phase_lock_fill_rate = 20;
	ad9545_ip.ref_in_clks[0].phase_lock_drain_rate = 20;

	/* Initialize Fixed clock to use as input reference */
	ret = no_os_clk_fixed_init(&ref_a_clk, &input_refa);
	if (ret)
		return ret;

	ad9545_ip.ref_in_clks[0].parent_clk = ref_a_clk;

	printf("AD9545 basic_example -\n\r");

	ret = ad9545_init(&ad9545_dev, &ad9545_ip);
	if (ret)
		return ret;

	ret = ad9545_setup(ad9545_dev);
	if (ret)
		goto error;

	/* Set Clocks' rate */
	ret = no_os_clk_recalc_rate(ad9545_dev->clks[AD9545_CLK_NCO][AD9545_NCO0],
				    10000);
	if (ret)
		goto error;
	ret = no_os_clk_recalc_rate(ad9545_dev->clks[AD9545_CLK_PLL][AD9545_PLL1],
				    1875000000);
	if (ret)
		goto error;
	ret = no_os_clk_recalc_rate(ad9545_dev->clks[AD9545_CLK_OUT][AD9545_Q1A],
				    156250000);
	if (ret)
		goto error;
	ret = no_os_clk_recalc_rate(ad9545_dev->clks[AD9545_CLK_OUT][AD9545_Q1A],
				    156250000);
	if (ret)
		goto error;

	while(1) {
		no_os_mdelay(1000);
	}

error:
	printf("Error on AD9545 basic_example: %d\n\r", ret);
	ad9545_remove(ad9545_dev);
	return ret;
}
