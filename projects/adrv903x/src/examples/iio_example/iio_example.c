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
#include "ad9528.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adxcvr.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "jesd204.h"
#include "no_os_axi_io.h"
#include "xilinx_uart.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "iio_app.h"
#include <string.h>
#include <xil_cache.h>

/*
 * TX_NUM_CHANNELS: JESD204 M parameter for the TX deframer (M_TX = 4).
 *   Results in 2 complex TX channels (TX 1, TX 2) in IIO Oscilloscope.
 * RX_NUM_CHANNELS: limited to 4 (first 4 of M_RX = 8 converters).
 *   The hardware framer has 8 converters (4 RX paths x I+Q), but the
 *   no-OS iio_axi_adc driver does not support complex_modified channels.
 *   Exposing 4 channels matches Linux (which groups I+Q into complex
 *   pairs) and shows voltage0-voltage3 in IIO Oscilloscope, covering
 *   RX0 (I/Q) and RX1 (I/Q). Converters 4-7 (RX2, RX3) are not captured.
 */
#define TX_NUM_CHANNELS		4
#define RX_NUM_CHANNELS		4

/* DMA buffers — 1024-byte aligned for Zynq MPSoC cache line compatibility */
uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__((aligned(1024)));
uint16_t adc_buffer[ADC_BUFFER_SAMPLES *
				       RX_NUM_CHANNELS] __attribute__((aligned(1024)));

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
#define AXI_DO_REG_TRANSFER_LENGTH	0x001C
#define AXI_DO_REG_RESETN_OFFLOAD	0x0084
#define AXI_DO_REG_CONTROL		0x0088

/*
 * read_dev wrapper: configure normal-mode store length then reset the offload
 * before each IIO DMA capture.  XFER_LENGTH is written before the RESETN
 * pulse so the value persists through the reset cycle.  The offload then
 * stores exactly 'bytes' from the ADC into BRAM and forwards them to the
 * DMAC — glitch-free from sample 0.
 */
static int32_t (*g_orig_adc_read_dev)(void *dev, void *buff,
				      uint32_t nb_samples);
static int32_t adrv903x_adc_read_dev(void *dev, void *buff, uint32_t nb_samples)
{
	struct iio_axi_adc_desc *adc_desc = (struct iio_axi_adc_desc *)dev;
	uint32_t bytes = nb_samples * no_os_hweight32(adc_desc->mask) * 2;

	/* Set transfer length (units: 64 bytes) before reset so it persists */
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_TRANSFER_LENGTH, bytes >> 6);
	/* Abort any in-progress store/forward cycle */
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0x0);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0x1);
	return g_orig_adc_read_dev(dev, buff, nb_samples);
}

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * Brings up the full JESD204 link and starts the IIO application loop:
 *   1.  AD9528 clock synthesizer setup (DEVCLK + SYSREF)
 *   2.  SYSREF_REQ GPIO configuration
 *   3.  ADXCVR initialization (TX and RX)
 *   4.  AXI JESD204 TX and RX controller initialization
 *   5.  ADRV903X initialization (firmware load up to PreMcsInit_NonBroadcast)
 *   6.  AXI clkgen setup (lane_rate / 66 for JESD204C)
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
	struct no_os_gpio_init_param sysref_gip = { 0 };
	struct no_os_gpio_desc *sysref_gpio = NULL;
	struct adrv903x_init_param init_param = { 0 };
	struct ad9528_platform_data ad9528_pdata = { 0 };
	struct ad9528_channel_spec ad9528_channels[14];
	struct ad9528_init_param ad9528_param = { 0 };
	struct ad9528_dev *ad9528_device = NULL;
	struct jesd204_topology *topology = NULL;
	struct adrv903x_rf_phy *phy = NULL;
	struct axi_clkgen *rx_clkgen = NULL;
	struct axi_clkgen *tx_clkgen = NULL;
	struct axi_jesd204_rx *rx_jesd = NULL;
	struct axi_jesd204_tx *tx_jesd = NULL;
	struct adxcvr *tx_adxcvr = NULL;
	struct adxcvr *rx_adxcvr = NULL;
	struct axi_adc *rx_adc = NULL;
	struct axi_dac *tx_dac = NULL;
	struct axi_dmac *rx_dmac = NULL;
	struct axi_dmac *tx_dmac = NULL;
	int ret;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	pr_info("ADRV903X IIO example\n");

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
		goto error_ad9528;
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

	/*
	 * ----------------------------------------------------------------
	 * AXI JESD204 TX and RX controller initialization
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

	tx_jesd_init.lane_clk = tx_adxcvr->clk_out;
	rx_jesd_init.lane_clk = rx_adxcvr->clk_out;

	ret = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (ret) {
		pr_err("axi_jesd204_tx_init() failed: %d\n", ret);
		goto error_rx_adxcvr;
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
	init_param.dev_clk = ad9528_device->clk_desc[1];
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

	/*
	 * ----------------------------------------------------------------
	 * AXI clkgen setup — JESD204C: lane_rate / 66
	 * ----------------------------------------------------------------
	 */
	ret = clkgen_setup(&rx_clkgen, &tx_clkgen);
	if (ret) {
		pr_err("clkgen_setup() failed: %d\n", ret);
		goto error_phy;
	}

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
			.jdev = ad9528_device->jdev,
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

	pr_info("ADRV903X JESD204 link up\n");

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	/*
	 * ----------------------------------------------------------------
	 * IIO application setup
	 * ----------------------------------------------------------------
	 */
	struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
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
	if (ret < 0)
		goto error_fsm;

	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buffer,
		.size = sizeof(adc_buffer),
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
	g_orig_adc_read_dev = adc_dev_desc->read_dev;
	adc_dev_desc->read_dev = adrv903x_adc_read_dev;

	struct iio_axi_dac_init_param iio_axi_dac_init_par = {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif
	};

	ret = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (ret < 0)
		goto error_iio_adc;

	struct iio_data_buffer write_buff = {
		.buff = (void *)dac_buffer,
		.size = sizeof(dac_buffer),
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

	/* Allow previous log messages to flush before IIO takes over UART */
	no_os_mdelay(100);

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto error_iio_dac;

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
	clkgen_remove(rx_clkgen, tx_clkgen);
error_phy:
	adrv903x_remove(phy);
error_rx_jesd:
	axi_jesd204_rx_remove(rx_jesd);
error_tx_jesd:
	axi_jesd204_tx_remove(tx_jesd);
error_rx_adxcvr:
	adxcvr_remove(rx_adxcvr);
error_tx_adxcvr:
	adxcvr_remove(tx_adxcvr);
error_sysref_gpio:
	if (!ad9528_device || ad9528_device->sysref_req_gpio != sysref_gpio)
		no_os_gpio_remove(sysref_gpio);
error_ad9528:
	ad9528_remove(ad9528_device);
	return ret;
}
