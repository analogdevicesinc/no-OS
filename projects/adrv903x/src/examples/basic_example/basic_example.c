/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for ADRV903X project.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders. This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "basic_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"
#include "initdata.h"
#include "adrv903x.h"
#include "clkgen_routines.h"
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#ifdef PLATFORM_VERSAL
#include "hmc7044.h"
#else
#include "ad9528.h"
#endif
#include "axi_adxcvr.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "jesd204.h"
#include <string.h>

#ifdef PLATFORM_VERSAL
/*
 * Versal GT lane clock stub — the GT transceiver is pre-configured by the
 * Versal fabric (PDI).  The JESD204 driver calls set_rate / enable on
 * lane_clk; these ops simply succeed without touching hardware.
 */
static int versal_lane_clk_enable(struct no_os_clk_desc *desc) { return 0; }
static int versal_lane_clk_disable(struct no_os_clk_desc *desc) { return 0; }
static int versal_lane_clk_set_rate(struct no_os_clk_desc *desc,
				    uint64_t rate) { return 0; }
static int versal_lane_clk_recalc_rate(struct no_os_clk_desc *desc,
				       uint64_t *rate)
{
	*rate = (uint64_t)ADRV903X_LANE_RATE_KHZ * 1000ULL;
	return 0;
}

static const struct no_os_clk_platform_ops versal_lane_clk_ops = {
	.clk_enable = versal_lane_clk_enable,
	.clk_disable = versal_lane_clk_disable,
	.clk_set_rate = versal_lane_clk_set_rate,
	.clk_recalc_rate = versal_lane_clk_recalc_rate,
};
#endif

/**
 * @brief Basic example main function.
 *
 * Brings up the full JESD204 link between the FPGA and the ADRV903X.
 *
 * Platform-specific differences:
 *   ZynqMP:       AD9528 clock + ADXCVR + AXI CLKGEN
 *   Versal Tetra: HMC7044 clock, no ADXCVR/CLKGEN (Versal GT in fabric)
 *
 * @return 0 on success, negative error code on failure.
 */
int example_main()
{
	struct adrv903x_init_param init_param = { 0 };
	struct jesd204_topology *topology = NULL;
	struct adrv903x_rf_phy *phy = NULL;
	struct axi_jesd204_rx *rx_jesd = NULL;
	struct axi_jesd204_tx *tx_jesd = NULL;
#ifdef ORX_JESD_BASEADDR
	struct axi_jesd204_rx *orx_jesd = NULL;
#endif
	int ret;

#ifdef PLATFORM_VERSAL
	struct no_os_gpio_desc *hmc7044_reset_gpio = NULL;
	struct hmc7044_chan_spec hmc7044_channels[5];
	struct hmc7044_init_param hmc7044_param = { 0 };
	struct hmc7044_dev *hmc7044_device = NULL;
	struct no_os_clk_desc *tx_lane_clk = NULL;
	struct no_os_clk_desc *rx_lane_clk = NULL;
#ifdef ORX_JESD_BASEADDR
	struct no_os_clk_desc *orx_lane_clk = NULL;
#endif
#else
	struct no_os_gpio_init_param sysref_gip = { 0 };
	struct no_os_gpio_desc *sysref_gpio = NULL;
	struct ad9528_platform_data ad9528_pdata = { 0 };
	struct ad9528_channel_spec ad9528_channels[14];
	struct ad9528_init_param ad9528_param = { 0 };
	struct ad9528_dev *ad9528_device = NULL;
	struct axi_clkgen *rx_clkgen = NULL;
	struct axi_clkgen *tx_clkgen = NULL;
	struct adxcvr *tx_adxcvr = NULL;
	struct adxcvr *rx_adxcvr = NULL;
#endif

	pr_info("ADRV903X basic example - JESD204 link bring-up\n");

#ifdef PLATFORM_VERSAL
	/*
	 * ----------------------------------------------------------------
	 * HMC7044 clock synthesizer setup (Versal Tetra).
	 * Provides DEVCLK (491.52 MHz on ch0) and SYSREF for MCS.
	 * Configuration matches DTS: versal-tetra-15mhz-nls.dts
	 * ----------------------------------------------------------------
	 */

	/* HMC7044 hardware reset pulse (active-high reset) */
	ret = no_os_gpio_get(&hmc7044_reset_gpio, &clkchip_gpio_init_param);
	if (ret) {
		pr_err("HMC7044 reset GPIO get failed: %d\n", ret);
		return ret;
	}
	/* Assert reset (drive HIGH) */
	ret = no_os_gpio_direction_output(hmc7044_reset_gpio, NO_OS_GPIO_HIGH);
	if (ret) {
		pr_err("HMC7044 reset GPIO set failed: %d\n", ret);
		no_os_gpio_remove(hmc7044_reset_gpio);
		return ret;
	}
	no_os_mdelay(10);
	/* De-assert reset (drive LOW) */
	ret = no_os_gpio_set_value(hmc7044_reset_gpio, NO_OS_GPIO_LOW);
	if (ret) {
		pr_err("HMC7044 reset de-assert failed: %d\n", ret);
		no_os_gpio_remove(hmc7044_reset_gpio);
		return ret;
	}
	no_os_mdelay(20);

	memset(hmc7044_channels, 0, sizeof(hmc7044_channels));

	/* Channel 0: DEV_CLK — ADRV903X reference clock (491.52 MHz) */
	hmc7044_channels[0].num = 0;
	hmc7044_channels[0].divider = HMC7044_DEV_CLK_DIV;
	hmc7044_channels[0].driver_mode = 2; /* LVDS */

	/* Channel 1: DEV_SYSREF */
	hmc7044_channels[1].num = 1;
	hmc7044_channels[1].divider = HMC7044_DEV_SYSREF_DIV;
	hmc7044_channels[1].driver_mode = 1; /* LVPECL */
	hmc7044_channels[1].is_sysref = true;
	hmc7044_channels[1].start_up_mode_dynamic_enable = true;
	hmc7044_channels[1].high_performance_mode_dis = true;

	/* Channel 6: FPGA_CORE_REFCLK */
	hmc7044_channels[2].num = 6;
	hmc7044_channels[2].divider = HMC7044_FPGA_CORE_REFCLK_DIV;
	hmc7044_channels[2].driver_mode = 2; /* LVDS */

	/* Channel 7: FPGA_CORE_SYSREF */
	hmc7044_channels[3].num = 7;
	hmc7044_channels[3].divider = HMC7044_FPGA_CORE_SYSREF_DIV;
	hmc7044_channels[3].driver_mode = 2; /* LVDS */

	/* Channel 12: FPGA_REFCLK */
	hmc7044_channels[4].num = 12;
	hmc7044_channels[4].divider = HMC7044_FPGA_REFCLK_DIV;
	hmc7044_channels[4].driver_mode = 2; /* LVDS */

	hmc7044_param.spi_init = (struct no_os_spi_init_param *)&hmc7044_spi_param;
	hmc7044_param.export_no_os_clk = true;
	/* PLL1: no external CLKIN references — VCXO only (DTS: all zeros) */
	hmc7044_param.vcxo_freq = HMC7044_VCXO_FREQ_HZ;
	hmc7044_param.pll2_freq = HMC7044_PLL2_FREQ_HZ;
	hmc7044_param.pll1_loop_bw = 200;
	hmc7044_param.pll1_ref_prio_ctrl = 0xe4;
	hmc7044_param.pll1_ref_autorevert_en = true;
	hmc7044_param.pll1_cp_current = 1920;
	hmc7044_param.pfd1_limit = HMC7044_PFD1_LIMIT_HZ;
	hmc7044_param.sysref_timer_div = HMC7044_SYSREF_TIMER_DIV;
	hmc7044_param.pulse_gen_mode = HMC7044_PULSE_GEN_8_PULSE;
	hmc7044_param.sync_pin_mode = HMC7044_SYNC_PIN_SYNC;
	hmc7044_param.high_performance_mode_clock_dist_en = false;
	hmc7044_param.in_buf_mode[0] = 0x06;
	hmc7044_param.in_buf_mode[1] = 0x06;
	hmc7044_param.in_buf_mode[4] = HMC7044_OSCIN_BUF_MODE;
	hmc7044_param.gpo_ctrl[0] = 0x37;
	hmc7044_param.gpo_ctrl[1] = 0x33;
	hmc7044_param.jesd204_sysref_provider = true;
	hmc7044_param.jesd204_max_sysref_frequency_hz = 2000000;
	hmc7044_param.num_channels = NO_OS_ARRAY_SIZE(hmc7044_channels);
	hmc7044_param.channels = hmc7044_channels;

	ret = hmc7044_init(&hmc7044_device, &hmc7044_param);
	if (ret) {
		pr_err("hmc7044_init() failed: %d\n", ret);
		no_os_gpio_remove(hmc7044_reset_gpio);
		return ret;
	}

	pr_info("HMC7044 init done, DEVCLK on channel 0\n");

	/*
	 * Reset the Versal GT PLL + datapath so the GT re-locks its PLL
	 * to the HMC7044 reference clock that just became available.
	 * Without this, the first cold boot often fails (GT CDR not locked).
	 */
	{
		struct no_os_gpio_desc *gt_rst = NULL;
		struct no_os_gpio_desc *gt_done = NULL;
		struct no_os_gpio_init_param gt_gpio_param = {
			.platform_ops = clkchip_gpio_init_param.platform_ops,
			.extra = clkchip_gpio_init_param.extra,
		};
		uint32_t gt_pll_gpios[] = { TX_GT_PLL_RESET, RX_GT_PLL_RESET };
		uint32_t gt_done_gpios[] = { TX_GT_RESET_DONE, RX_GT_RESET_DONE };
		uint8_t done_val;

		for (int i = 0; i < 2; i++) {
			gt_gpio_param.number = gt_pll_gpios[i];
			ret = no_os_gpio_get(&gt_rst, &gt_gpio_param);
			if (ret)
				continue;
			no_os_gpio_direction_output(gt_rst, NO_OS_GPIO_HIGH);
			no_os_mdelay(2);
			no_os_gpio_set_value(gt_rst, NO_OS_GPIO_LOW);
			no_os_gpio_remove(gt_rst);

			/* Poll reset-done GPIO */
			gt_gpio_param.number = gt_done_gpios[i];
			ret = no_os_gpio_get(&gt_done, &gt_gpio_param);
			if (!ret) {
				no_os_gpio_direction_input(gt_done);
				int timeout = 100;
				do {
					no_os_mdelay(5);
					no_os_gpio_get_value(gt_done, &done_val);
				} while (!done_val && --timeout);
				no_os_gpio_remove(gt_done);
			}
		}
		pr_info("GT PLL reset done\n");
	}

#else /* ZynqMP */
	/*
	 * ----------------------------------------------------------------
	 * AD9528 clock synthesizer setup
	 * Provides DEVCLK (245.76 MHz on ch1) and SYSREF for MCS.
	 * Configuration matches DTS:
	 *   zynqmp-zcu102-rev10-adrv9032r-nls.dts
	 * SYSREF is triggered externally via the SYSREF_REQ GPIO
	 * (SYSREF_LEVEL_HIGH + NSHOT_8_PULSES).
	 * ----------------------------------------------------------------
	 */
	memset(ad9528_channels, 0, sizeof(ad9528_channels));

	ad9528_param.spi_init = ad9528_spi_param;
	ad9528_param.export_no_os_clk = true;
	ad9528_param.gpio_resetb = &clkchip_gpio_init_param;
	ad9528_param.pdata = &ad9528_pdata;
	ad9528_param.pdata->num_channels = 14;
	ad9528_param.pdata->channels = &ad9528_channels[0];

	/* Channel 0: DEV_SYSREF */
	ad9528_channels[0].channel_num = 0;
	strcpy((char *)ad9528_channels[0].extended_name, "DEV_SYSREF");
	ad9528_channels[0].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[0].channel_divider = AD9528_DEV_SYSREF_DIV;
	ad9528_channels[0].signal_source = SOURCE_SYSREF_VCO;

	/* Channel 1: DEV_CLK — ADRV903X reference clock (245.76 MHz) */
	ad9528_channels[1].channel_num = 1;
	strcpy((char *)ad9528_channels[1].extended_name, "DEV_CLK");
	ad9528_channels[1].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[1].channel_divider = AD9528_DEV_CLK_DIV;
	ad9528_channels[1].signal_source = SOURCE_VCO;

	/* Channel 3: CORE_CLK */
	ad9528_channels[3].channel_num = 3;
	strcpy((char *)ad9528_channels[3].extended_name, "CORE_CLK");
	ad9528_channels[3].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[3].channel_divider = AD9528_CORE_CLK_DIV;
	ad9528_channels[3].signal_source = SOURCE_VCO;

	/* Channel 11: REF_CLK1 */
	ad9528_channels[11].channel_num = 11;
	strcpy((char *)ad9528_channels[11].extended_name, "REF_CLK1");
	ad9528_channels[11].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[11].channel_divider = AD9528_REF_CLK1_DIV;
	ad9528_channels[11].signal_source = SOURCE_VCO;

	/* Channel 12: FPGA_SYSREF */
	ad9528_channels[12].channel_num = 12;
	strcpy((char *)ad9528_channels[12].extended_name, "FPGA_SYSREF");
	ad9528_channels[12].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[12].channel_divider = AD9528_FPGA_SYSREF_DIV;
	ad9528_channels[12].signal_source = SOURCE_SYSREF_VCO;

	/* Channel 13: REF_CLK0 — ADXCVR reference (491.52 MHz = VCO/2) */
	ad9528_channels[13].channel_num = 13;
	strcpy((char *)ad9528_channels[13].extended_name, "REF_CLK0");
	ad9528_channels[13].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[13].channel_divider = AD9528_REF_CLK0_DIV;
	ad9528_channels[13].signal_source = SOURCE_VCO;

	/* VCXO and PLL configuration */
	ad9528_param.pdata->vcxo_freq = AD9528_VCXO_FREQ_HZ;
	ad9528_param.pdata->refa_en = 1;
	ad9528_param.pdata->refa_diff_rcv_en = 1;
	ad9528_param.pdata->ref_mode = REF_MODE_EXT_REF;
	/* PLL1 */
	ad9528_param.pdata->pll1_feedback_div = AD9528_PLL1_FEEDBACK_DIV;
	ad9528_param.pdata->pll1_charge_pump_current_nA = AD9528_PLL1_CHARGE_PUMP_NA;
	/* PLL2 */
	ad9528_param.pdata->pll2_vco_div_m1 = AD9528_PLL2_VCO_DIV_M1_VAL;
	ad9528_param.pdata->pll2_r1_div = AD9528_PLL2_R1_DIV_VAL;
	ad9528_param.pdata->pll2_ndiv_a_cnt = AD9528_PLL2_NDIV_A;
	ad9528_param.pdata->pll2_ndiv_b_cnt = AD9528_PLL2_NDIV_B;
	ad9528_param.pdata->pll2_n2_div = AD9528_PLL2_N2_DIV_VAL;
	ad9528_param.pdata->pll2_freq_doubler_en = 1;
	ad9528_param.pdata->pll2_charge_pump_current_nA = AD9528_PLL2_CHARGE_PUMP_NA;
	/* SYSREF — GPIO-triggered NSHOT bursts for MCS */
	ad9528_param.pdata->sysref_src = SYSREF_SRC_INTERNAL;
	ad9528_param.pdata->sysref_k_div = AD9528_SYSREF_K_DIV_VAL;
	ad9528_param.pdata->sysref_pattern_mode = SYSREF_PATTERN_NSHOT;
	ad9528_param.pdata->sysref_nshot_mode = SYSREF_NSHOT_8_PULSES;
	ad9528_param.pdata->sysref_req_trigger_mode = SYSREF_LEVEL_HIGH;
	ad9528_param.pdata->sysref_req_en = true;
	ad9528_param.pdata->jdev_desired_sysref_freq = 3840000;
	/* Loop filter */
	ad9528_param.pdata->rpole2 = RPOLE2_900_OHM;
	ad9528_param.pdata->rzero = RZERO_1850_OHM;
	ad9528_param.pdata->cpole1 = CPOLE1_16_PF;
	ad9528_param.pdata->stat0_pin_func_sel = 0x9; /* PLL1 in holdover */
	ad9528_param.pdata->stat1_pin_func_sel = 3;   /* PLL2 Locked */

	ret = ad9528_setup(&ad9528_device, ad9528_param);
	if (ret) {
		pr_err("ad9528_setup() failed: %d\n", ret);
		return ret;
	}

	pr_info("AD9528 locked, DEVCLK 245.76 MHz on channel 1\n");

	/*
	 * ----------------------------------------------------------------
	 * SYSREF_REQ GPIO — used to trigger AD9528 NSHOT bursts during MCS.
	 * Start LOW so no spurious SYSREF pulses are generated before MCS.
	 * ----------------------------------------------------------------
	 */
	sysref_gip.number = AD9528_SYSREF_REQ;
	sysref_gip.platform_ops = clkchip_gpio_init_param.platform_ops;
	sysref_gip.extra = clkchip_gpio_init_param.extra;

	ret = no_os_gpio_get(&sysref_gpio, &sysref_gip);
	if (ret) {
		pr_err("SYSREF_REQ gpio_get failed: %d\n", ret);
		goto error_clkchip;
	}

	ret = no_os_gpio_direction_output(sysref_gpio, NO_OS_GPIO_LOW);
	if (ret) {
		pr_err("SYSREF_REQ gpio_direction_output failed: %d\n", ret);
		goto error_sysref_gpio;
	}

	/*
	 * Share the SYSREF_REQ GPIO with the AD9528 driver so that the
	 * JESD204 FSM sysref callback (ad9528_jesd204_sysref) can toggle
	 * the pin when post_state_sysref fires after LINK_ENABLE.
	 */
	ad9528_device->sysref_req_gpio = sysref_gpio;

	/*
	 * ----------------------------------------------------------------
	 * ADXCVR initialization — GTH transceivers for TX and RX lanes.
	 * ref_rate_khz = 491520 (AD9528 ch13 REF_CLK0 = VCO/2).
	 * lane_rate_khz = 16220.16 Mbps (from UC101 4T4R1OR profile).
	 * QPLL0 selected; LPM equalization disabled (DFE mode).
	 * export_no_os_clk = true so lane_clk can be passed to JESD204 IP.
	 * ----------------------------------------------------------------
	 */
	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = ADRV903X_LANE_RATE_KHZ,
		.ref_rate_khz = ADRV903X_ADXCVR_REF_RATE_KHZ,
		.export_no_os_clk = true
	};

	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = ADRV903X_LANE_RATE_KHZ,
		.ref_rate_khz = ADRV903X_ADXCVR_REF_RATE_KHZ,
		.export_no_os_clk = true
	};

	ret = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (ret) {
		pr_err("tx_adxcvr_init() failed: %d\n", ret);
		goto error_sysref_gpio;
	}

	ret = adxcvr_init(&rx_adxcvr, &rx_adxcvr_init);
	if (ret) {
		pr_err("rx_adxcvr_init() failed: %d\n", ret);
		goto error_tx_adxcvr;
	}
#endif /* PLATFORM_VERSAL */

	/*
	 * ----------------------------------------------------------------
	 * AXI JESD204 TX and RX controller initialization.
	 * On ZynqMP, lane_clk comes from the ADXCVR clk_out.
	 * On Versal, lane_clk is NULL (GT managed by Versal fabric).
	 * ----------------------------------------------------------------
	 */
	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = ADRV903X_TX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = ADRV903X_TX_JESD_FRAMES_PER_MULTIFRAME,
		.converters_per_device = ADRV903X_TX_JESD_CONVS_PER_DEVICE,
		.converter_resolution = ADRV903X_TX_JESD_CONV_RESOLUTION,
		.bits_per_sample = ADRV903X_TX_JESD_BITS_PER_SAMPLE,
		.high_density = ADRV903X_TX_JESD_HIGH_DENSITY,
		.control_bits_per_sample = ADRV903X_TX_JESD_CTRL_BITS_PER_SAMPLE,
		.subclass = ADRV903X_TX_JESD_SUBCLASS,
		.device_clk_khz = ADRV903X_DEVICE_CLK_KHZ,
		.lane_clk_khz = ADRV903X_LANE_RATE_KHZ
	};

	struct jesd204_rx_init rx_jesd_init = {
		.name = "rx_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = ADRV903X_RX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = ADRV903X_RX_JESD_FRAMES_PER_MULTIFRAME,
		.subclass = ADRV903X_RX_JESD_SUBCLASS,
		.device_clk_khz = ADRV903X_DEVICE_CLK_KHZ,
		.lane_clk_khz = ADRV903X_LANE_RATE_KHZ,
	};

#ifdef ORX_JESD_BASEADDR
	struct jesd204_rx_init orx_jesd_init = {
		.name = "orx_jesd",
		.base = ORX_JESD_BASEADDR,
		.octets_per_frame = ADRV903X_ORX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = ADRV903X_ORX_JESD_FRAMES_PER_MULTIFRAME,
		.subclass = ADRV903X_ORX_JESD_SUBCLASS,
		.device_clk_khz = ADRV903X_DEVICE_CLK_KHZ,
		.lane_clk_khz = ADRV903X_LANE_RATE_KHZ,
	};
#endif

#ifdef PLATFORM_VERSAL
	struct no_os_clk_init_param tx_lane_clk_param = {
		.name = "tx_lane_clk",
		.platform_ops = &versal_lane_clk_ops,
	};
	struct no_os_clk_init_param rx_lane_clk_param = {
		.name = "rx_lane_clk",
		.platform_ops = &versal_lane_clk_ops,
	};

	ret = no_os_clk_init(&tx_lane_clk, &tx_lane_clk_param);
	if (ret) {
		pr_err("tx_lane_clk init failed: %d\n", ret);
		goto error_clkchip;
	}
	ret = no_os_clk_init(&rx_lane_clk, &rx_lane_clk_param);
	if (ret) {
		pr_err("rx_lane_clk init failed: %d\n", ret);
		no_os_clk_remove(tx_lane_clk);
		goto error_clkchip;
	}
	tx_jesd_init.lane_clk = tx_lane_clk;
	rx_jesd_init.lane_clk = rx_lane_clk;
#ifdef ORX_JESD_BASEADDR
	struct no_os_clk_init_param orx_lane_clk_param = {
		.name = "orx_lane_clk",
		.platform_ops = &versal_lane_clk_ops,
	};
	ret = no_os_clk_init(&orx_lane_clk, &orx_lane_clk_param);
	if (ret) {
		pr_err("orx_lane_clk init failed: %d\n", ret);
		no_os_clk_remove(rx_lane_clk);
		no_os_clk_remove(tx_lane_clk);
		goto error_clkchip;
	}
	orx_jesd_init.lane_clk = orx_lane_clk;
#endif
#else
	tx_jesd_init.lane_clk = tx_adxcvr->clk_out;
	rx_jesd_init.lane_clk = rx_adxcvr->clk_out;
#endif

	ret = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (ret) {
		pr_err("axi_jesd204_tx_init() failed: %d\n", ret);
#ifdef PLATFORM_VERSAL
		goto error_clkchip;
#else
		goto error_rx_adxcvr;
#endif
	}

	ret = axi_jesd204_rx_init(&rx_jesd, &rx_jesd_init);
	if (ret) {
		pr_err("axi_jesd204_rx_init() failed: %d\n", ret);
		goto error_tx_jesd;
	}

#ifdef ORX_JESD_BASEADDR
	ret = axi_jesd204_rx_init(&orx_jesd, &orx_jesd_init);
	if (ret) {
		pr_err("axi_jesd204_rx_init(orx) failed: %d\n", ret);
		goto error_rx_jesd;
	}
#endif

	/*
	 * ----------------------------------------------------------------
	 * ADRV903X initialization: firmware load up to PreMcsInit_NonBroadcast.
	 * MCS and PostMcsInit are handled by the JESD204 FSM callbacks.
	 * dev_clk source:
	 *   Versal:  HMC7044 channel 0 (DEV_CLK)
	 *   ZynqMP:  AD9528 channel 1 (DEV_CLK, 245.76 MHz)
	 * ----------------------------------------------------------------
	 */
#ifdef PLATFORM_VERSAL
	init_param.dev_clk = hmc7044_device->clk_desc[0];
#else
	init_param.dev_clk = ad9528_device->clk_desc[1];
#endif
	init_param.post_mcs_init = &utilityInit;
	init_param.profile_file = ADRV903X_PROFILE_FILE;
	init_param.cpu_fw_file = ADRV903X_CPU_FW_FILE;
	init_param.stream_file = ADRV903X_STREAM_FILE;
	init_param.rx_gain_table_file = ADRV903X_RX_GAIN_TABLE_FILE;
	init_param.rx_gain_table_mask = ADRV903X_RX_GAIN_TABLE_MASK;

	ret = adrv903x_init(&phy, &init_param);
	if (ret) {
		pr_err("adrv903x_init() failed: %d\n", ret);
#ifdef ORX_JESD_BASEADDR
		goto error_orx_jesd;
#else
		goto error_rx_jesd;
#endif
	}

	pr_info("ADRV903X initialized successfully\n");

#ifndef PLATFORM_VERSAL
	/*
	 * ----------------------------------------------------------------
	 * AXI clkgen setup — JESD204C: lane_rate / 66. (ZynqMP only)
	 * Must be called before jesd204_fsm_start() so the lane clocks
	 * are running when the FSM reaches CLOCKS_ENABLE.
	 * ----------------------------------------------------------------
	 */
	ret = clkgen_setup(&rx_clkgen, &tx_clkgen);
	if (ret) {
		pr_err("clkgen_setup() failed: %d\n", ret);
		goto error_phy;
	}
#endif

	/*
	 * ----------------------------------------------------------------
	 * JESD204 topology initialization and FSM start.
	 * The FSM coordinates the full bring-up sequence across all devices.
	 *
	 * Link IDs:
	 *   DEFRAMER0_LINK_TX (0) = FPGA TX → ADRV903X RX (deframer)
	 *   FRAMER0_LINK_RX   (2) = ADRV903X TX → FPGA RX (framer)
	 *   FRAMER1_LINK_RX   (3) = ADRV903X ORX → FPGA RX (framer, Versal)
	 * ----------------------------------------------------------------
	 */
	struct jesd204_topology_dev devs[] = {
		{
#ifdef PLATFORM_VERSAL
			.jdev = hmc7044_device->jdev,
#else
			.jdev = ad9528_device->jdev,
#endif
#ifdef ORX_JESD_BASEADDR
			.link_ids = { DEFRAMER0_LINK_TX, FRAMER0_LINK_RX,
				      FRAMER1_LINK_RX },
			.links_number = 3,
#else
			.link_ids = { DEFRAMER0_LINK_TX, FRAMER0_LINK_RX },
			.links_number = 2,
#endif
			.is_sysref_provider = true,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = { FRAMER0_LINK_RX },
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = { DEFRAMER0_LINK_TX },
			.links_number = 1,
		},
#ifdef ORX_JESD_BASEADDR
		{
			.jdev = orx_jesd->jdev,
			.link_ids = { FRAMER1_LINK_RX },
			.links_number = 1,
		},
#endif
		{
			.jdev = phy->jdev,
#ifdef ORX_JESD_BASEADDR
			.link_ids = { DEFRAMER0_LINK_TX, FRAMER0_LINK_RX,
				      FRAMER1_LINK_RX },
			.links_number = 3,
#else
			.link_ids = { DEFRAMER0_LINK_TX, FRAMER0_LINK_RX },
			.links_number = 2,
#endif
			.is_top_device = true,
		},
	};

	ret = jesd204_topology_init(&topology, devs, NO_OS_ARRAY_SIZE(devs));
	if (ret) {
		pr_err("jesd204_topology_init() failed: %d\n", ret);
		goto error_clkgen;
	}

	ret = jesd204_fsm_start(topology, JESD204_LINKS_ALL);
	if (ret) {
		pr_err("jesd204_fsm_start() failed: %d\n", ret);
		goto error_fsm;
	}

	pr_info("ADRV903X JESD204 link up\n");

	/* Read JESD204 link status */
	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);
#ifdef ORX_JESD_BASEADDR
	axi_jesd204_rx_status_read(orx_jesd);
#endif

	jesd204_fsm_stop(topology, JESD204_LINKS_ALL);
	/* TODO: jesd204_topology_remove(topology) once the API is available */
error_fsm:
error_clkgen:
#ifndef PLATFORM_VERSAL
	clkgen_remove(rx_clkgen, tx_clkgen);
error_phy:
#endif
	adrv903x_remove(phy);
#ifdef ORX_JESD_BASEADDR
error_orx_jesd:
	axi_jesd204_rx_remove(orx_jesd);
#endif
error_rx_jesd:
	axi_jesd204_rx_remove(rx_jesd);
error_tx_jesd:
	axi_jesd204_tx_remove(tx_jesd);
#ifndef PLATFORM_VERSAL
error_rx_adxcvr:
	adxcvr_remove(rx_adxcvr);
error_tx_adxcvr:
	adxcvr_remove(tx_adxcvr);
error_sysref_gpio:
	/*
	 * sysref_gpio ownership is transferred to ad9528_device after the
	 * handoff above; ad9528_remove() frees it.  Only free here if we
	 * jumped before the handoff (ad9528_device->sysref_req_gpio != gpio).
	 */
	if (!ad9528_device || ad9528_device->sysref_req_gpio != sysref_gpio)
		no_os_gpio_remove(sysref_gpio);
#endif
error_clkchip:
#ifdef PLATFORM_VERSAL
	hmc7044_remove(hmc7044_device);
	no_os_gpio_remove(hmc7044_reset_gpio);
#else
	ad9528_remove(ad9528_device);
#endif
	pr_info("example_main exiting with code %d\n", ret);
	return ret;
}
