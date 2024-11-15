/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example header for ad9545 project
 *   @author Jonathan Santos (jonathan.santos@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "basic_example.h"
#include "common_data.h"
#include "ad9545.h"
#include "no_os_clk.h"
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
	struct no_os_clk_desc *ref_b_clk;
	struct no_os_clk_desc *ref_bb_clk;
	struct no_os_clk_desc *ref_m1_clk;
	int ret;

	struct no_os_clk_init_param refb_clk_ip = {
		.name = "Ref-B",
		.platform_ops = &refb_clk_ops
	};

	struct no_os_clk_init_param refbb_clk_ip = {
		.name = "Ref-BB",
		.platform_ops = &refbb_clk_ops
	};

	struct no_os_clk_init_param refm1_clk_ip = {
		.name = "Ref-M1",
		.platform_ops = &refm1_clk_ops
	};

	/* Ref. Clock */
	ad9545_ip.sys_clk.sys_clk_crystal = true;
	ad9545_ip.sys_clk.ref_freq_hz = 52000000;
	/* NCO params */
	ad9545_ip.aux_nco_clks[AD9545_NCO0].nco_used = true;
	ad9545_ip.aux_nco_clks[AD9545_NCO0].address = AD9545_NCO0;
	ad9545_ip.aux_nco_clks[AD9545_NCO0].freq_thresh_ps = 0xFFFFFF;
	ad9545_ip.aux_nco_clks[AD9545_NCO0].phase_thresh_ps = 0xFFFFFF;
	/* PLL params */
	/* 	PLL0 */
	ad9545_ip.pll_clks[AD9545_PLL0].pll_used = true;
	ad9545_ip.pll_clks[AD9545_PLL0].address = AD9545_PLL0;
	/* 		PLL0 Profiles */
	/* 			PLL0 Profile 0 */
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[0].en = 1;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[0].address = 0;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[0].tdc_source = 3;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[0].priority = 5;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[0].loop_bw_uhz = 50000;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[0].fast_acq_excess_bw = 8;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[0].fast_acq_timeout_ms = 10000;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[0].fast_acq_settle_ms = 1000;
	/* 			PLL0 Profile 1 */
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[1].en = 1;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[1].address = 1;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[1].tdc_source = 2;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[1].priority = 15;
	ad9545_ip.pll_clks[AD9545_PLL0].profiles[1].loop_bw_uhz = 200000000;
	/* 	PLL1 */
	ad9545_ip.pll_clks[AD9545_PLL1].pll_used = true;
	ad9545_ip.pll_clks[AD9545_PLL1].address = AD9545_PLL1;
	/* 		PLL1 Profiles */
	/* 			PLL1 Profile 0 */
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].en = 1;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].address = 0;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].tdc_source = 3;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].priority = 5;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].loop_bw_uhz = 50000;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].fast_acq_excess_bw = 8;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].fast_acq_timeout_ms = 10000;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[0].fast_acq_settle_ms = 1000;
	/* 			PLL1 Profile 1 */
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[1].en = 1;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[1].address = 1;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[1].tdc_source = 2;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[1].priority = 15;
	ad9545_ip.pll_clks[AD9545_PLL1].profiles[1].loop_bw_uhz = 200000000;
	/* Output Clocks params */
	/* 	OUT Q0A */
	ad9545_ip.out_clks[AD9545_Q0A].output_used = true;
	ad9545_ip.out_clks[AD9545_Q0A].address = AD9545_Q0A;
	ad9545_ip.out_clks[AD9545_Q0A].output_mode = DRIVER_MODE_SINGLE_DIV_DIF;
	ad9545_ip.out_clks[AD9545_Q0A].source_ua = 15000;
	/* 	OUT Q0B */
	ad9545_ip.out_clks[AD9545_Q0B].output_used = true;
	ad9545_ip.out_clks[AD9545_Q0B].address = AD9545_Q0B;
	ad9545_ip.out_clks[AD9545_Q0B].output_mode = DRIVER_MODE_SINGLE_DIV_DIF;
	ad9545_ip.out_clks[AD9545_Q0B].source_ua = 15000;
	/* 	OUT Q1A */
	ad9545_ip.out_clks[AD9545_Q1A].output_used = true;
	ad9545_ip.out_clks[AD9545_Q1A].address = AD9545_Q1A;
	ad9545_ip.out_clks[AD9545_Q1A].output_mode = DRIVER_MODE_SINGLE_DIV_DIF;
	ad9545_ip.out_clks[AD9545_Q1A].source_ua = 15000;
	/* 	OUT Q1B */
	ad9545_ip.out_clks[AD9545_Q1B].output_used = true;
	ad9545_ip.out_clks[AD9545_Q1B].address = AD9545_Q1B;
	ad9545_ip.out_clks[AD9545_Q1B].output_mode = DRIVER_MODE_SINGLE_DIV_DIF;
	ad9545_ip.out_clks[AD9545_Q1B].source_ua = 15000;
	/* Aux TDC params */
	ad9545_ip.aux_tdc_clks[AD9545_CLK_AUX_TDC0].tdc_used = true;
	ad9545_ip.aux_tdc_clks[AD9545_CLK_AUX_TDC0].address = AD9545_CLK_AUX_TDC0;
	ad9545_ip.aux_tdc_clks[AD9545_CLK_AUX_TDC0].pin_nr = 1;
	/* Aux DPLL params */
	ad9545_ip.aux_dpll_clk.dpll_used = true;
	ad9545_ip.aux_dpll_clk.source = 4;
	ad9545_ip.aux_dpll_clk.loop_bw_mhz = 50000;
	/* Input Reference Clock */
	/* REF-B */
	ad9545_ip.ref_in_clks[2].ref_used = true;
	ad9545_ip.ref_in_clks[2].address = 2;
	ad9545_ip.ref_in_clks[2].mode = AD9545_SINGLE_ENDED;
	ad9545_ip.ref_in_clks[2].s_conf = DRIVER_MODE_AC_COUPLED_IF;
	ad9545_ip.ref_in_clks[2].r_div_ratio = 50;
	ad9545_ip.ref_in_clks[2].d_tol_ppb = 10000000;
	ad9545_ip.ref_in_clks[2].monitor_hyst_scale = 7; // 87500
	ad9545_ip.ref_in_clks[2].valid_t_ms = 10;
	ad9545_ip.ref_in_clks[2].freq_thresh_ps = 2000;
	ad9545_ip.ref_in_clks[2].phase_thresh_ps = 2000;
	ad9545_ip.ref_in_clks[2].freq_lock_fill_rate = 20;
	ad9545_ip.ref_in_clks[2].freq_lock_drain_rate = 20;
	ad9545_ip.ref_in_clks[2].phase_lock_fill_rate = 20;
	ad9545_ip.ref_in_clks[2].phase_lock_drain_rate = 20;
	/* REF-BB */
	ad9545_ip.ref_in_clks[3].ref_used = true;
	ad9545_ip.ref_in_clks[3].address = 3;
	ad9545_ip.ref_in_clks[3].mode = AD9545_SINGLE_ENDED;
	ad9545_ip.ref_in_clks[3].s_conf = DRIVER_MODE_DC_COUPLED_1V8;
	ad9545_ip.ref_in_clks[3].r_div_ratio = 1;
	ad9545_ip.ref_in_clks[3].d_tol_ppb = 100000;
	ad9545_ip.ref_in_clks[3].monitor_hyst_scale = 3; // 12500
	ad9545_ip.ref_in_clks[3].valid_t_ms = 1000;
	ad9545_ip.ref_in_clks[3].freq_thresh_ps = 2000;
	ad9545_ip.ref_in_clks[3].phase_thresh_ps = 2000;
	ad9545_ip.ref_in_clks[3].freq_lock_fill_rate = 100;
	ad9545_ip.ref_in_clks[3].freq_lock_drain_rate = 10;
	ad9545_ip.ref_in_clks[3].phase_lock_fill_rate = 100;
	ad9545_ip.ref_in_clks[3].phase_lock_drain_rate = 10;


	/* Initialize Fixed clocks to use as input references */
	ret = no_os_clk_init(&ref_b_clk, &refb_clk_ip);
	if (ret)
		return ret;

	ret = no_os_clk_init(&ref_bb_clk, &refbb_clk_ip);
	if (ret)
		return ret;

	ret = no_os_clk_init(&ref_m1_clk, &refm1_clk_ip);
	if (ret)
		return ret;

	ad9545_ip.ref_in_clks[3].parent_clk = ref_bb_clk;
	ad9545_ip.ref_in_clks[2].parent_clk = ref_b_clk;
	ad9545_ip.aux_tdc_clks[AD9545_CLK_AUX_TDC0].parent_clk = ref_m1_clk;

	printf("AD9545 basic_example\n\r");

	ret = ad9545_init(&ad9545_dev, &ad9545_ip);
	if (ret < 0)
		return ret;

	ret = ad9545_setup(ad9545_dev);
	if (ret < 0)
		goto error;

	/* Set Clocks' rate */
	ret = no_os_clk_set_rate(ad9545_dev->clks[AD9545_CLK_NCO][AD9545_NCO0],
				 10000);
	if (ret)
		goto error;

	ret = no_os_clk_set_rate(ad9545_dev->clks[AD9545_CLK_PLL][AD9545_PLL0],
				 1400000000);
	if (ret)
		goto error;

	ret = no_os_clk_set_rate(ad9545_dev->clks[AD9545_CLK_PLL][AD9545_PLL1],
				 1750000000);
	if (ret)
		goto error;

	ret = no_os_clk_set_rate(ad9545_dev->clks[AD9545_CLK_OUT][AD9545_Q0A],
				 10000000);
	if (ret)
		goto error;

	ret = no_os_clk_set_rate(ad9545_dev->clks[AD9545_CLK_OUT][AD9545_Q0B],
				 10000000);
	if (ret)
		goto error;

	ret = no_os_clk_set_rate(ad9545_dev->clks[AD9545_CLK_OUT][AD9545_Q1A],
				 25000000);
	if (ret)
		goto error;

	ret = no_os_clk_set_rate(ad9545_dev->clks[AD9545_CLK_OUT][AD9545_Q1B],
				 10000000);
	if (ret)
		goto error;

	ret = no_os_clk_set_rate(
		      ad9545_dev->clks[AD9545_CLK_AUX_TDC][AD9545_CLK_AUX_TDC0],
		      200000);
	if (ret)
		goto error;

	/* Recalibrate PLLs */
	ret = ad9545_calib_aplls(ad9545_dev);
	if (ret < 0)
		return ret;

	printf("Done.\n\r");
	goto out;

error:
	printf("Error on AD9545 basic_example: %d\n\r", ret);
out:
	return ad9545_remove(ad9545_dev);
}
