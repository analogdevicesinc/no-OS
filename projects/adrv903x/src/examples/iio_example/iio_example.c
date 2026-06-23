/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for the ADRV903X project.
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
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "iio_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"
#include "initdata.h"
#include "adrv903x.h"
#include "clkgen_routines.h"
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#ifdef PLATFORM_VERSAL
#include "hmc7044.h"
#else
#include "ad9528.h"
#endif
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adxcvr.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "jesd204.h"
#include "adi_adrv903x_datainterface.h"
#include "no_os_axi_io.h"
#include "xilinx_uart.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "iio_app.h"
#include <string.h>
#include <xil_cache.h>

/*
 * 8T8R profile: 16 converters per direction (8 antennas × I + Q).
 * Matches DMA example and ADRV903X_TX_JESD_CONVS_PER_DEVICE in app_config.h.
 */
#define TX_NUM_CHANNELS		ADRV903X_TX_JESD_CONVS_PER_DEVICE
#define RX_NUM_CHANNELS		ADRV903X_TX_JESD_CONVS_PER_DEVICE

/* DMA buffers — placed at fixed high-DDR addresses (past the application)
 * to avoid Versal PMC AXI errors when DMA targets .bss addresses. */
uint32_t *dac_buffer = (uint32_t *)(DDR_MEM_BASEADDR + 0x1000000UL); /* 16MB */
uint16_t *adc_buffer = (uint16_t *)(DDR_MEM_BASEADDR + 0x2000000UL); /* 32MB */

/*
 * AXI data offload register map (axi_data_offload IP):
 *   0x001C  AXI_DO_REG_TRANSFER_LENGTH — store length in units of 64 bytes
 *   0x0084  AXI_DO_REG_RESETN_OFFLOAD  — bit 0: 1 = out of reset
 *   0x0088  AXI_DO_REG_CONTROL         — bit 0: 1 = bypass, 0 = normal (store+fwd)
 *
 * Normal mode (CONTROL=0): ADC -> BRAM -> DMAC (store-and-forward).
 * Bypass mode (CONTROL=1): ADC -> CDC FIFO -> DMAC; the K=64-deep CDC
 * FIFO takes 64 samples to prime after each RESETN, causing a glitch at
 * capture start.  Normal mode has no such artifact.
 */
#define AXI_DO_REG_MEMORY_SIZE_LSB	0x0014
#define AXI_DO_REG_MEMORY_SIZE_MSB	0x0018
#define AXI_DO_REG_RESETN_OFFLOAD	0x0084
#define AXI_DO_REG_CONTROL		0x0088

/*
 * read_dev wrapper: configure the data offload for each IIO DMA capture.
 *
 * Matches the DMA example sequence:
 *   1. Set CONTROL = 0x0 (normal store-and-forward mode)
 *   2. Pulse RESETN to start a fresh capture cycle
 *   3. Run the DMA transfer (capped to BRAM size)
 *
 * If the client requests more samples than the BRAM can hold, the DMA
 * transfer is capped and the remainder of the buffer is zero-filled.
 * This prevents the DMA from stalling (the offload has no more data to
 * forward) and avoids exposing stale memory to the IIO client.
 */
static uint64_t g_rx_do_mem_size;

/* DMA-safe destination in high DDR, same as dma_example */
#define RX_DMA_ADDR	(DDR_MEM_BASEADDR + 0x2000000UL)

static int32_t adrv903x_adc_read_dev(void *dev, void *buff, uint32_t nb_samples)
{
	struct iio_axi_adc_desc *adc_desc = (struct iio_axi_adc_desc *)dev;
	uint32_t bytes_per_sample = no_os_hweight32(adc_desc->mask) * 2;
	int32_t ret;

	/* Always DMA the full BRAM — matches the DMA example exactly. */
	uint32_t bram_samples = nb_samples;
	if (g_rx_do_mem_size)
		bram_samples = (uint32_t)(g_rx_do_mem_size / bytes_per_sample);

	uint32_t dma_bytes = bram_samples * bytes_per_sample;

	/* Data offload reset sequence — matches the DMA example */
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_CONTROL, 0x0);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0x0);
	no_os_mdelay(1);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0x1);

	/* DMA to a safe high-DDR address to avoid PMC AXI errors */
	struct axi_dma_transfer transfer = {
		.size = dma_bytes,
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = RX_DMA_ADDR,
	};

	ret = axi_dmac_transfer_start(adc_desc->dmac, &transfer);
	if (ret)
		return ret;

	ret = axi_dmac_transfer_wait_completion(adc_desc->dmac, 5000);
	if (ret)
		return ret;

	if (adc_desc->dcache_invalidate_range)
		adc_desc->dcache_invalidate_range(RX_DMA_ADDR, dma_bytes);

	/* Copy captured data into the IIO circular buffer */
	uint32_t copy_bytes = nb_samples * bytes_per_sample;
	if (copy_bytes > dma_bytes)
		copy_bytes = dma_bytes;
	memcpy(buff, (void *)RX_DMA_ADDR, copy_bytes);

	/* Zero-fill remainder if client asked for more than BRAM holds */
	if (copy_bytes < nb_samples * bytes_per_sample)
		memset((uint8_t *)buff + copy_bytes, 0,
		       nb_samples * bytes_per_sample - copy_bytes);

	return 0;
}

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

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * Brings up the full JESD204 link and starts the IIO application loop:
 *   1.  Clock synthesizer setup (HMC7044 on Versal, AD9528 on ZynqMP)
 *   2.  SYSREF_REQ GPIO configuration (ZynqMP only)
 *   3.  ADXCVR initialization (TX and RX, ZynqMP only)
 *   4.  AXI JESD204 TX and RX controller initialization
 *   5.  ADRV903X initialization (firmware load up to PreMcsInit_NonBroadcast)
 *   6.  AXI clkgen setup (lane_rate / 66 for JESD204C, ZynqMP only)
 *   7.  AXI DAC core init — DDS mode (IIO Oscilloscope controls tones)
 *   8.  AXI ADC core init — reset TPL + IQ correction
 *   9.  RX data offload — normal mode + per-capture XFER_LENGTH + RESETN
 *   10. TX and RX DMAC initialization
 *   11. JESD204 topology initialization and FSM start
 *   12. IIO application init and run (blocking)
 *
 * @return 0 on success, negative error code on failure.
*******************************************************************************/
int example_main()
{
	struct adrv903x_init_param init_param = { 0 };
	struct jesd204_topology *topology = NULL;
	struct adrv903x_rf_phy *phy = NULL;
	struct axi_jesd204_rx *rx_jesd = NULL;
	struct axi_jesd204_tx *tx_jesd = NULL;
	struct axi_adc *rx_adc = NULL;
	struct axi_dac *tx_dac = NULL;
	struct axi_dmac *rx_dmac = NULL;
	struct axi_dmac *tx_dmac = NULL;
	int ret;

#ifdef PLATFORM_VERSAL
	struct no_os_gpio_desc *hmc7044_reset_gpio = NULL;
	struct hmc7044_chan_spec hmc7044_channels[5];
	struct hmc7044_init_param hmc7044_param = { 0 };
	struct hmc7044_dev *hmc7044_device = NULL;
	struct no_os_clk_desc *tx_lane_clk = NULL;
	struct no_os_clk_desc *rx_lane_clk = NULL;
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

	Xil_ICacheEnable();
	Xil_DCacheEnable();
	pr_info("hello\n");
	pr_info("ADRV903X IIO example\n");

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
	 * AD9528 clock synthesizer setup.
	 * ----------------------------------------------------------------
	 */
	memset(ad9528_channels, 0, sizeof(ad9528_channels));

	ad9528_param.spi_init = ad9528_spi_param;
	ad9528_param.export_no_os_clk = true;
	ad9528_param.gpio_resetb = &clkchip_gpio_init_param;
	ad9528_param.pdata = &ad9528_pdata;
	ad9528_param.pdata->num_channels = 14;
	ad9528_param.pdata->channels = &ad9528_channels[0];

	ad9528_channels[0].channel_num = 0;
	strcpy((char *)ad9528_channels[0].extended_name, "DEV_SYSREF");
	ad9528_channels[0].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[0].channel_divider = AD9528_DEV_SYSREF_DIV;
	ad9528_channels[0].signal_source = SOURCE_SYSREF_VCO;

	ad9528_channels[1].channel_num = 1;
	strcpy((char *)ad9528_channels[1].extended_name, "DEV_CLK");
	ad9528_channels[1].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[1].channel_divider = AD9528_DEV_CLK_DIV;
	ad9528_channels[1].signal_source = SOURCE_VCO;

	ad9528_channels[3].channel_num = 3;
	strcpy((char *)ad9528_channels[3].extended_name, "CORE_CLK");
	ad9528_channels[3].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[3].channel_divider = AD9528_CORE_CLK_DIV;
	ad9528_channels[3].signal_source = SOURCE_VCO;

	ad9528_channels[11].channel_num = 11;
	strcpy((char *)ad9528_channels[11].extended_name, "REF_CLK1");
	ad9528_channels[11].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[11].channel_divider = AD9528_REF_CLK1_DIV;
	ad9528_channels[11].signal_source = SOURCE_VCO;

	ad9528_channels[12].channel_num = 12;
	strcpy((char *)ad9528_channels[12].extended_name, "FPGA_SYSREF");
	ad9528_channels[12].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[12].channel_divider = AD9528_FPGA_SYSREF_DIV;
	ad9528_channels[12].signal_source = SOURCE_SYSREF_VCO;

	ad9528_channels[13].channel_num = 13;
	strcpy((char *)ad9528_channels[13].extended_name, "REF_CLK0");
	ad9528_channels[13].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[13].channel_divider = AD9528_REF_CLK0_DIV;
	ad9528_channels[13].signal_source = SOURCE_VCO;

	ad9528_param.pdata->vcxo_freq = AD9528_VCXO_FREQ_HZ;
	ad9528_param.pdata->refa_en = 1;
	ad9528_param.pdata->refa_diff_rcv_en = 1;
	ad9528_param.pdata->ref_mode = REF_MODE_EXT_REF;
	ad9528_param.pdata->pll1_feedback_div = AD9528_PLL1_FEEDBACK_DIV;
	ad9528_param.pdata->pll1_charge_pump_current_nA = AD9528_PLL1_CHARGE_PUMP_NA;
	ad9528_param.pdata->pll2_vco_div_m1 = AD9528_PLL2_VCO_DIV_M1_VAL;
	ad9528_param.pdata->pll2_r1_div = AD9528_PLL2_R1_DIV_VAL;
	ad9528_param.pdata->pll2_ndiv_a_cnt = AD9528_PLL2_NDIV_A;
	ad9528_param.pdata->pll2_ndiv_b_cnt = AD9528_PLL2_NDIV_B;
	ad9528_param.pdata->pll2_n2_div = AD9528_PLL2_N2_DIV_VAL;
	ad9528_param.pdata->pll2_freq_doubler_en = 1;
	ad9528_param.pdata->pll2_charge_pump_current_nA = AD9528_PLL2_CHARGE_PUMP_NA;
	ad9528_param.pdata->sysref_src = SYSREF_SRC_INTERNAL;
	ad9528_param.pdata->sysref_k_div = AD9528_SYSREF_K_DIV_VAL;
	ad9528_param.pdata->sysref_pattern_mode = SYSREF_PATTERN_NSHOT;
	ad9528_param.pdata->sysref_nshot_mode = SYSREF_NSHOT_8_PULSES;
	ad9528_param.pdata->sysref_req_trigger_mode = SYSREF_LEVEL_HIGH;
	ad9528_param.pdata->sysref_req_en = true;
	ad9528_param.pdata->jdev_desired_sysref_freq = 3840000;
	ad9528_param.pdata->rpole2 = RPOLE2_900_OHM;
	ad9528_param.pdata->rzero = RZERO_1850_OHM;
	ad9528_param.pdata->cpole1 = CPOLE1_16_PF;
	ad9528_param.pdata->stat0_pin_func_sel = 0x9;
	ad9528_param.pdata->stat1_pin_func_sel = 3;

	ret = ad9528_setup(&ad9528_device, ad9528_param);
	if (ret) {
		pr_err("ad9528_setup() failed: %d\n", ret);
		return ret;
	}

	/*
	 * ----------------------------------------------------------------
	 * SYSREF_REQ GPIO
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

	ad9528_device->sysref_req_gpio = sysref_gpio;

	/*
	 * ----------------------------------------------------------------
	 * ADXCVR initialization
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
	 * On Versal, lane_clk is a stub (GT managed by Versal fabric).
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

	/* GT reset GPIOs — wired into JESD FSM callbacks */
	struct no_os_gpio_init_param gt_gpio_base = {
		.platform_ops = clkchip_gpio_init_param.platform_ops,
		.extra = clkchip_gpio_init_param.extra,
	};
	struct no_os_gpio_init_param tx_gt_pll_rst = gt_gpio_base;
	tx_gt_pll_rst.number = TX_GT_PLL_RESET;
	struct no_os_gpio_init_param tx_gt_dp_rst = gt_gpio_base;
	tx_gt_dp_rst.number = TX_GT_DP_RESET;
	struct no_os_gpio_init_param tx_gt_done = gt_gpio_base;
	tx_gt_done.number = TX_GT_RESET_DONE;

	struct no_os_gpio_init_param rx_gt_pll_rst = gt_gpio_base;
	rx_gt_pll_rst.number = RX_GT_PLL_RESET;
	struct no_os_gpio_init_param rx_gt_dp_rst = gt_gpio_base;
	rx_gt_dp_rst.number = RX_GT_DP_RESET;
	struct no_os_gpio_init_param rx_gt_done = gt_gpio_base;
	rx_gt_done.number = RX_GT_RESET_DONE;

	tx_jesd_init.gt_reset_pll = &tx_gt_pll_rst;
	tx_jesd_init.gt_reset_dp = &tx_gt_dp_rst;
	tx_jesd_init.gt_reset_done = &tx_gt_done;

	rx_jesd_init.gt_reset_pll = &rx_gt_pll_rst;
	rx_jesd_init.gt_reset_dp = &rx_gt_dp_rst;
	rx_jesd_init.gt_reset_done = &rx_gt_done;
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

	/*
	 * ----------------------------------------------------------------
	 * ADRV903X initialization
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
		goto error_rx_jesd;
	}

#ifndef PLATFORM_VERSAL
	/*
	 * ----------------------------------------------------------------
	 * AXI clkgen setup — JESD204C: lane_rate / 66 (ZynqMP only)
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
	 * AXI DAC core initialization — DDS mode.
	 * IIO Oscilloscope controls the DDS tone generators directly.
	 * ----------------------------------------------------------------
	 */
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.num_channels = TX_NUM_CHANNELS,
		.channels = NULL,
		.rate = 0,	/* AXI DAC RATECNTRL: 0 = 1:1 clock-to-sample ratio */
	};

	ret = axi_dac_init(&tx_dac, &tx_dac_init);
	if (ret) {
		pr_err("axi_dac_init() failed: %d\n", ret);
		goto error_clkgen;
	}

	ret = axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
	if (ret) {
		pr_err("axi_dac_set_datasel() failed: %d\n", ret);
		goto error_tx_dac;
	}

	/*
	 * ----------------------------------------------------------------
	 * AXI ADC core initialization.
	 * Reset the TPL ADC core, then apply IQ correction writes matching
	 * Linux adrv903x_conv.c adrv903x_post_setup().
	 * ----------------------------------------------------------------
	 */
	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = RX_NUM_CHANNELS,
	};

	ret = axi_adc_init_begin(&rx_adc, &rx_adc_init);
	if (ret) {
		pr_err("axi_adc_init_begin() failed: %d\n", ret);
		goto error_tx_dac;
	}

	axi_adc_write(rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(rx_adc, AXI_ADC_REG_RSTN, AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	axi_adc_write(rx_adc, AXI_ADC_REG_CNTRL, 0);
	for (uint8_t ch = 0; ch < rx_adc->num_channels; ch++) {
		axi_adc_write(rx_adc, AXI_ADC_REG_CHAN_CNTRL_1(ch),
			      AXI_ADC_DCFILT_OFFSET(0));
		axi_adc_write(rx_adc, AXI_ADC_REG_CHAN_CNTRL_2(ch),
			      (ch & 1) ? 0x00004000 : 0x40000000);
		axi_adc_write(rx_adc, AXI_ADC_REG_CHAN_CNTRL(ch),
			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
			      AXI_ADC_ENABLE | AXI_ADC_IQCOR_ENB);
	}

	/*
	 * ----------------------------------------------------------------
	 * RX data offload — normal (store-and-forward) mode.
	 * CONTROL=0 routes ADC samples through BRAM then to the DMAC.
	 * XFER_LENGTH is set per-capture in adrv903x_adc_read_dev() before
	 * each RESETN pulse, so the offload stores exactly the requested
	 * number of bytes and then forwards them — no bypass FIFO, no K=64
	 * sample priming glitch.
	 * ----------------------------------------------------------------
	 */
	{
		uint32_t do_mem_lsb, do_mem_msb;
		no_os_axi_io_read(RX_DATA_OFFLOAD_BASEADDR,
				  AXI_DO_REG_MEMORY_SIZE_LSB, &do_mem_lsb);
		no_os_axi_io_read(RX_DATA_OFFLOAD_BASEADDR,
				  AXI_DO_REG_MEMORY_SIZE_MSB, &do_mem_msb);
		g_rx_do_mem_size = (uint64_t)do_mem_lsb |
				   ((uint64_t)(do_mem_msb & 0x3) << 32);
		pr_info("RX data offload: BRAM size = %lu bytes\n",
			(unsigned long)g_rx_do_mem_size);
	}
	/* Normal mode (store-and-forward, same as DMA example) */
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_CONTROL, 0x0);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0x0);
	no_os_mdelay(1);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0x1);
	pr_info("RX data offload: normal mode (store+forward)\n");

	/*
	 * ----------------------------------------------------------------
	 * TX and RX DMAC initialization
	 * ----------------------------------------------------------------
	 */
	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		IRQ_DISABLED
	};

	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		IRQ_DISABLED
	};

	ret = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (ret) {
		pr_err("axi_dmac_init() tx failed: %d\n", ret);
		goto error_rx_adc;
	}

	ret = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (ret) {
		pr_err("axi_dmac_init() rx failed: %d\n", ret);
		goto error_tx_dmac;
	}

	/*
	 * ----------------------------------------------------------------
	 * JESD204 topology initialization and FSM start.
	 * ----------------------------------------------------------------
	 */
	struct jesd204_topology_dev devs[] = {
		{
#ifdef PLATFORM_VERSAL
			.jdev = hmc7044_device->jdev,
#else
			.jdev = ad9528_device->jdev,
#endif
			.link_ids = { DEFRAMER0_LINK_TX, FRAMER0_LINK_RX },
			.links_number = 2,
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
		{
			.jdev = phy->jdev,
			.link_ids = { DEFRAMER0_LINK_TX, FRAMER0_LINK_RX },
			.links_number = 2,
			.is_top_device = true,
		},
	};

	ret = jesd204_topology_init(&topology, devs, NO_OS_ARRAY_SIZE(devs));
	if (ret) {
		pr_err("jesd204_topology_init() failed: %d\n", ret);
		goto error_rx_dmac;
	}

	ret = jesd204_fsm_start(topology, JESD204_LINKS_ALL);
	if (ret) {
		pr_err("jesd204_fsm_start() failed: %d\n", ret);
		goto error_fsm;
	}

	/* Fast TX deframer retry: if the ADRV903x deframer didn't sync,
	 * pulse TX GT DP reset and re-enable the TX JESD link. */
#ifdef PLATFORM_VERSAL
	if (tx_jesd->gt_reset_dp) {
		adi_adrv903x_DeframerStatus_v2_t dfrmSt = { 0 };
		int tx_retry;

		for (tx_retry = 0; tx_retry < 3; tx_retry++) {
			adi_adrv903x_DeframerStatusGet_v2(phy->palmaDevice,
				ADI_ADRV903X_DEFRAMER_0, &dfrmSt);
			if (dfrmSt.linkState & 0x2)
				break;
			pr_warning("TX deframer not synced (0x%X), retry %d\n",
				   dfrmSt.linkState, tx_retry + 1);
			axi_jesd204_tx_lane_clk_disable(tx_jesd);
			no_os_gpio_set_value(tx_jesd->gt_reset_dp,
					     NO_OS_GPIO_HIGH);
			no_os_udelay(20);
			no_os_gpio_set_value(tx_jesd->gt_reset_dp,
					     NO_OS_GPIO_LOW);
			for (int w = 0; w < 32; w++) {
				uint8_t dv;
				no_os_mdelay(1);
				no_os_gpio_get_value(tx_jesd->gt_reset_done,
						     &dv);
				if (dv)
					break;
			}
			axi_jesd204_tx_lane_clk_enable(tx_jesd);
			no_os_mdelay(20);
		}
		adi_adrv903x_DeframerStatusGet_v2(phy->palmaDevice,
			ADI_ADRV903X_DEFRAMER_0, &dfrmSt);
		pr_info("TX deframer linkState 0x%X after %d retries\n",
			dfrmSt.linkState, tx_retry);
		for (int i = 0; i < 4; i++)
			pr_info("TX deframer lane%d status 0x%X\n",
				i, dfrmSt.laneStatus[i]);
		if (!(dfrmSt.linkState & 0x2))
			pr_err("TX deframer failed after %d retries\n",
			       tx_retry);
	}
#endif

	pr_info("ADRV903X JESD204 link up\n");

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	/*
	 * TX data offload — bypass mode.
	 * Bypass lets DMA buffer transfers flow through the CDC FIFO.
	 * In DDS mode no data traverses the offload, so bypass is harmless.
	 */
	no_os_axi_io_write(TX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_CONTROL, 0x1); /* bypass */
	no_os_axi_io_write(TX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0x0);
	no_os_mdelay(1);
	no_os_axi_io_write(TX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0x1);

	pr_info("Setting up IIO application...\n");

	/*
	 * ----------------------------------------------------------------
	 * IIO application setup
	 * ----------------------------------------------------------------
	 */
	struct xil_uart_init_param platform_uart_init_par = {
#if defined(XPAR_XUARTPS_NUM_INSTANCES) || defined(XPAR_XUARTPSV_NUM_INSTANCES)
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
#else
		.type = UART_PL,
#endif
	};

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_axi_dac_desc *iio_axi_dac_desc;
	struct iio_device *adc_dev_desc, *dac_dev_desc;

	struct iio_axi_adc_init_param iio_axi_adc_init_par = {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange,
#endif
	};

	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (ret < 0) {
		pr_err("iio_axi_adc_init() failed: %d\n", ret);
		goto error_fsm;
	}

	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buffer,
		.size = ADC_BUFFER_SAMPLES * RX_NUM_CHANNELS * sizeof(uint16_t),
	};
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);

	/*
	 * Wrap the ADC submit callback to pulse RX offload RESETN before each
	 * DMA capture. In normal (store-and-forward) mode, the BRAM may hold
	 * stale data from a previous fill cycle. Pulsing RESETN triggers a
	 * fresh BRAM fill from the current ADC phase; the DMA stalls under
	 * back-pressure until the fill completes (~100 µs), then reads clean
	 * data. This eliminates the K=64-sample glitch seen in bypass mode.
	 * The wrapper and its saved pointer are file-scope statics (no nested
	 * functions, which require GCC trampolines and an executable stack).
	 */
	adc_dev_desc->read_dev = adrv903x_adc_read_dev;

	struct iio_axi_dac_init_param iio_axi_dac_init_par = {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif
	};

	ret = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (ret < 0) {
		pr_err("iio_axi_dac_init() failed: %d\n", ret);
		goto error_iio_adc;
	}

	struct iio_data_buffer write_buff = {
		.buff = (void *)dac_buffer,
		.size = DAC_BUFFER_SAMPLES * sizeof(uint32_t),
	};
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("cf-adrv903x-lpc", iio_axi_adc_desc,
			       adc_dev_desc, &read_buff, NULL, NULL),
		IIO_APP_DEVICE("cf-adrv903x-dds-core-lpc", iio_axi_dac_desc,
			       dac_dev_desc, NULL, &write_buff, NULL),
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	pr_info("iio_app_init — UART switches to 921600 after this\n");
	/* Allow previous log messages to flush before IIO takes over UART */
	no_os_mdelay(100);

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_err("iio_app_init() failed: %d\n", ret);
		goto error_iio_dac;
	}

	/* NOTE: UART is now at 921600 — no more pr_info() from this point,
	 * any output would appear as garbled chars on the 115200 terminal. */

	/* iio_app_run blocks indefinitely; cleanup on return is unreachable
	 * in normal operation and follows the no-OS IIO example convention. */
	return iio_app_run(app);

error_iio_dac:
	iio_axi_dac_remove(iio_axi_dac_desc);
error_iio_adc:
	iio_axi_adc_remove(iio_axi_adc_desc);
error_fsm:
	jesd204_fsm_stop(topology, JESD204_LINKS_ALL);
	/* TODO: jesd204_topology_remove(topology) once the API is available */
error_rx_dmac:
	axi_dmac_remove(rx_dmac);
error_tx_dmac:
	axi_dmac_remove(tx_dmac);
error_rx_adc:
	axi_adc_remove(rx_adc);
error_tx_dac:
	axi_dac_remove(tx_dac);
error_clkgen:
#ifndef PLATFORM_VERSAL
	clkgen_remove(rx_clkgen, tx_clkgen);
error_phy:
#endif
	adrv903x_remove(phy);
error_rx_jesd:
	axi_jesd204_rx_remove(rx_jesd);
error_tx_jesd:
	axi_jesd204_tx_remove(tx_jesd);
#ifdef PLATFORM_VERSAL
	no_os_clk_remove(rx_lane_clk);
	no_os_clk_remove(tx_lane_clk);
#else
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
	return ret;
}
