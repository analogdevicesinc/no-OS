/***************************************************************************//**
 *   @file   dma_example.c
 *   @brief  DMA example for the ADRV903X project.
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
#include "dma_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"
#include "adrv903x.h"
#include "adi_adrv903x_radioctrl.h"
#include "adi_adrv903x_tx.h"
#include "adi_adrv903x_rx.h"
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
#include <string.h>
#include "no_os_axi_io.h"
#include <xil_cache.h>

/*
 * Must match the FPGA HDL TPL channel count (8 for ADRV9032R on ZCU102).
 * Using 4 produces scrambled data: the TPL interleaves 8 streams in the
 * DMA path even though Linux IIO exposes only 4 IIO channels.
 */
#define DMA_NUM_CHANNELS	8

/* DMA buffers — 1024-byte aligned for Zynq MPSoC cache line compatibility */
uint32_t dac_buffer_dma[DAC_BUFFER_SAMPLES] __attribute__((aligned(1024)));
uint16_t adc_buffer_dma[ADC_BUFFER_SAMPLES *
			DMA_NUM_CHANNELS] __attribute__((aligned(1024)));

/**
 * @brief DMA example main function.
 *
 * Brings up the full JESD204 link and demonstrates DMA data transfer:
 *   1.  AD9528 clock synthesizer setup (DEVCLK + SYSREF)
 *   2.  SYSREF_REQ GPIO configuration
 *   3.  ADXCVR initialization (TX and RX)
 *   4.  AXI JESD204 TX and RX controller initialization
 *   5.  ADRV903X initialization (firmware load up to PreMcsInit_NonBroadcast)
 *   6.  AXI clkgen setup (lane_rate / 66 for JESD204C)
 *   7.  AXI DAC core init — load sine wave LUT into TX DMA buffer
 *   8.  AXI ADC core init — reset TPL ADC core
 *   9.  TX and RX DMAC initialization
 *   10. JESD204 topology initialization and FSM start
 *       FSM drives: MCS (LINK_SETUP/OPT_SETUP_STAGE1/2) + link enable
 *   11. Start TX DMA (continuous sine wave to DAC)
 *   12. Wait 1 s then capture RX DMA samples
 *   13. Print buffer addresses for inspection
 *
 * @return 0 on success, negative error code on failure.
 */
int dma_example_main(void)
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

	pr_info("ADRV903X DMA example\n");

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
		.link_enable_delay_ms = ADRV903X_RX_JESD_LINK_ENABLE_DELAY_MS
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
	 * AXI DAC core initialization.
	 * Load the built-in sine wave LUT (sine_lut_iq) into the DMA
	 * buffer so the DAC continuously transmits a tone once DMA starts.
	 * ----------------------------------------------------------------
	 */
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.num_channels = DMA_NUM_CHANNELS,
		.channels = NULL,
		.rate = 3,
	};

	ret = axi_dac_init(&tx_dac, &tx_dac_init);
	if (ret) {
		pr_err("axi_dac_init() failed: %d\n", ret);
		goto error_clkgen;
	}

	ret = axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	if (ret) {
		pr_err("axi_dac_set_datasel() failed: %d\n", ret);
		goto error_tx_dac;
	}

	axi_dac_load_custom_data(tx_dac, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq),
				 (uintptr_t)dac_buffer_dma);

	/*
	 * Replicate the sine pattern to fill the full 256KB TX offload BRAM.
	 * axi_dac_load_custom_data() writes one block of 16384 bytes (1024
	 * samples × 4 IQ pairs × 4 bytes).  The TX data offload XFER_LENGTH
	 * power-on default is 0x1000 (256KB).  Filling the entire BRAM
	 * prevents the offload from replaying uninitialized memory as noise.
	 */
	uint32_t tx_dma_size = sizeof(sine_lut_iq) * (DMA_NUM_CHANNELS / 2) * 16;
	uint32_t one_rep = tx_dma_size / 16;
	for (int rep = 1; rep < 16; rep++)
		memcpy((uint8_t *)dac_buffer_dma + rep * one_rep,
		       dac_buffer_dma, one_rep);

	/*
	 * ----------------------------------------------------------------
	 * AXI ADC core initialization.
	 * Reset the TPL ADC core (MMCM + data path) to clear any stale
	 * state before the JESD link comes up.
	 * ----------------------------------------------------------------
	 */
	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = DMA_NUM_CHANNELS,
	};

	ret = axi_adc_init_begin(&rx_adc, &rx_adc_init);
	if (ret) {
		pr_err("axi_adc_init_begin() failed: %d\n", ret);
		goto error_tx_dac;
	}

	axi_adc_write(rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(rx_adc, AXI_ADC_REG_RSTN, AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	/*
	 * ADC post_setup: IQ correction matching adrv904x_post_setup()
	 * and Linux adrv903x_conv.c adrv903x_post_setup().
	 * Without these writes the ADC data path produces noise.
	 */
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
	 * RX data offload — bypass mode.
	 * The axi_data_offload_rx IP sits between the ADC TPL and the DMA.
	 * Without initialization it captures only its internal memory depth
	 * (262144 bytes) and then signals EOT, leaving the rest of the DMA
	 * buffer as zeros.  Enabling bypass makes it transparent.
	 *
	 * Register map (from linux/drivers/misc/adi-axi-data-offload.c):
	 *   0x0084  AXI_DO_REG_RESETN_OFFLOAD — bit 0: 1 = out of reset
	 *   0x0088  AXI_DO_REG_CONTROL        — bit 0: 1 = bypass enable
	 * ----------------------------------------------------------------
	 */
#define AXI_DO_REG_RESETN_OFFLOAD	0x0084
#define AXI_DO_REG_CONTROL		0x0088

	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_RESETN_OFFLOAD, 0x1);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR,
			   AXI_DO_REG_CONTROL, 0x1);
	pr_info("RX data offload: bypass enabled\n");

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
	 * The driver's post_running_stage callback (JESD204_OP_OPT_POST_RUNNING_STAGE)
	 * already activated the signal chain and set TX attenuation to 6 dB.
	 * Override to 0 dB here for maximum loopback signal level.
	 */
	adi_adrv903x_TxAtten_t txAtten[] = {
		{ .txChannelMask = ADI_ADRV903X_TX0, .txAttenuation_mdB = 0 },
		{ .txChannelMask = ADI_ADRV903X_TX4, .txAttenuation_mdB = 0 },
	};
	ret = (int)adi_adrv903x_TxAttenSet(phy->palmaDevice, txAtten,
					    NO_OS_ARRAY_SIZE(txAtten));
	if (ret)
		pr_err("TxAttenSet failed: %d\n", ret);

	/* Set RX gain to maximum for best loopback SNR */
	adi_adrv903x_RxGain_t rxGain[] = {
		{ .rxChannelMask = ADI_ADRV903X_RX0, .gainIndex = 255 },
		{ .rxChannelMask = ADI_ADRV903X_RX4, .gainIndex = 255 },
	};
	ret = (int)adi_adrv903x_RxGainSet(phy->palmaDevice, rxGain,
					   NO_OS_ARRAY_SIZE(rxGain));
	if (ret)
		pr_err("RxGainSet failed: %d\n", ret);

	/*
	 * ----------------------------------------------------------------
	 * TX DMA: push the sine wave LUT to the DAC.
	 * ----------------------------------------------------------------
	 */
	struct axi_dma_transfer tx_transfer = {
		.size = tx_dma_size,
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = (uintptr_t)dac_buffer_dma,
		.dest_addr = 0
	};

	pr_info("DMA TX: address=0x%08lx size=%lu\n",
		(uintptr_t)dac_buffer_dma,
		(unsigned long)tx_transfer.size);

	Xil_DCacheFlushRange((uintptr_t)dac_buffer_dma, tx_dma_size);
	axi_dmac_transfer_start(tx_dmac, &tx_transfer);

	/* Allow the tone to settle for 1 s before capturing RX data */
	no_os_mdelay(1000);

	/*
	 * ----------------------------------------------------------------
	 * RX DMA: capture one buffer of ADC samples.
	 * ----------------------------------------------------------------
	 */
	struct axi_dma_transfer rx_transfer = {
		.size = sizeof(adc_buffer_dma),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = (uintptr_t)adc_buffer_dma
	};

	ret = axi_dmac_transfer_start(rx_dmac, &rx_transfer);
	if (ret) {
		pr_err("RX DMA transfer_start failed: %d\n", ret);
		goto error_fsm;
	}

	ret = axi_dmac_transfer_wait_completion(rx_dmac, 1000);
	if (ret) {
		pr_err("RX DMA wait_completion failed: %d\n", ret);
		goto error_fsm;
	}

	Xil_DCacheInvalidateRange((uintptr_t)adc_buffer_dma,
				  sizeof(adc_buffer_dma));

	pr_info("DMA RX: address=0x%08lx samples=%lu channels=%u bits=%lu\n",
		(uintptr_t)adc_buffer_dma,
		NO_OS_ARRAY_SIZE(adc_buffer_dma),
		rx_adc_init.num_channels,
		8 * sizeof(adc_buffer_dma[0]));

	pr_info("ADRV903X DMA example complete\n");

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
