/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for K (AD9081K) project.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright (C) 2026 Analog Devices, Inc.
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
// #define DMA_CYCLIC_PASS

#include "basic_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"
#include "ad9081k.h"
#include "si5391.h"
#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adxcvr.h"
#include "axi_adc_core.h"
#include "tx_generator.h"
#include "axi_dmac.h"
#include "jesd204.h"
#include "xil_cache.h"

#include "lwip_xemacps.h"
#define IIOD
#define DMA_CYCLIC_PASS
#include "xilinx_uart.h"
#ifdef IIOD
#include "iio_axi_adc.h"
#include "iio_ad9081k.h"
#include "iio_app.h"
#endif

static uint32_t dac_buffer_dma[DAC_BUFFER_SAMPLES] __attribute__((aligned(1024)));
static uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__((aligned(1024)));
static uint16_t adc_buffer[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__((
			aligned(1024)));

/*
 * 512-bit base sequence (MSW..LSW):
 *   0x00000001 FFFFFFFF EEEEEEEE DDDDDDDD
 *   CCCCCCCC BBBBBBBB AAAAAAAA 99999999
 *   88888888 77777777 66666666 55555555
 *   44444444 33333333 22222222 11111111
 *
 * Shifted left by 2 as a 512-bit integer, then bit 0 set to 1.
 * tx_vector[15] = MSW, tx_vector[0] = LSW.
 */
static uint32_t tx_vector[16] = {
	0x44444445, /* [0]  from 0x11111111 */
	0x88888888, /* [1]  from 0x22222222 */
	0xCCCCCCCC, /* [2]  from 0x33333333 */
	0x11111110, /* [3]  from 0x44444444 */
	0x55555555, /* [4]  from 0x55555555 */
	0x99999999, /* [5]  from 0x66666666 */
	0xDDDDDDDD, /* [6]  from 0x77777777 */
	0x22222221, /* [7]  from 0x88888888 */
	0x66666666, /* [8]  from 0x99999999 */
	0xAAAAAAAA, /* [9]  from 0xAAAAAAAA */
	0xEEEEEEEE, /* [10] from 0xBBBBBBBB */
	0x33333332, /* [11] from 0xCCCCCCCC */
	0x77777777, /* [12] from 0xDDDDDDDD */
	0xBBBBBBBB, /* [13] from 0xEEEEEEEE */
	0xFFFFFFFF, /* [14] from 0xFFFFFFFF */
	0x00000005, /* [15] from 0x00000001 */
};

/* 512-bit test PASS stream (bits[1:0] of word[0] = 0b11 → streamid=3): */
static uint32_t tx_passthrough[16] = {
	0xDEAD0003, 0xBEEF0001, 0xCAFE0002, 0xFACE0003,
	0x12340004, 0x56780005, 0x9ABC0006, 0xDEF00007,
	0xA5A50008, 0x5A5A0009, 0xC0DE000A, 0xF00D000B,
	0xBABE000C, 0xD00D000D, 0xFEED000E, 0xF00F000F,
};

/* 64-bit test CMD READ: 0x00000000_02580100 */
static uint32_t tx_cmd_read[2] = {
	0x02580100, /* [0] LSW */
	0x00000000, /* [1] MSW */
};

/* 
 * 64-bit test CMD STREAMING READ: 0x00000000_02540E00 
 * Read from NUMWORDS=3 registers starting from address 0x0254,
 * meaning 0x0254, 0x0258, 0x025C.
 * 3 streams should be expected on RX.
 */
static uint32_t tx_cmd_streaming_read[2] = {
	0x02540E00, /* [0] LSW */
	0x00000000, /* [1] MSW */
};

/* 
 * 64-bit test CMD WRITE: 0x12345678_00900300 
 * Write value 0x12345678 at address 0x0090.
 * Nothing should be expected on RX.
 */
static uint32_t tx_cmd_write[2] = {
	0x00900300, /* [0] LSW */
	0x12345678, /* [1] MSW */
};


int example_main()
{
	// Si5391
	static struct si5391_init_param si5391_param = { 0 };
	struct si5391_dev *si5391_device = NULL;

	// JESD204-related
	static struct ad9081k_init_param ad9081k_param = { 0 };
	struct ad9081k_dev *ad9081k_device = NULL;
	struct jesd204_topology *topology = NULL;

	// TX LINK layer
	struct axi_jesd204_tx *tx_jesd = NULL;
	// RX LINK layer
	struct axi_jesd204_rx *rx_jesd = NULL;

	// TX part of JESD204 PHY layer
	struct adxcvr *tx_adxcvr = NULL;
	// RX part of JESD204 PHY layer
	struct adxcvr *rx_adxcvr = NULL;

	// TX GENERATOR - TX TPL IP from HDL
	struct tx_generator_dev *tx_generator = NULL;
	// RX ANALYZER - RX TPL IP from HDL
	struct axi_adc *rx_adc = NULL;

	// TX DMA for VEC/PASS streams (L*64b wide)
	struct axi_dmac *tx_dmac = NULL;
	// RX DMA for PATT/ASYNC/PASS streams (L*64b wide)
	struct axi_dmac *rx_dmac = NULL;

	// TX CMD DMA for CMD streams (64b wide)
	struct axi_dmac *tx_cmd_dmac = NULL;
	// RX CMD DMA for CMD streams (64b wide)
	struct axi_dmac *rx_cmd_dmac = NULL;

	unsigned int i;
	int ret;

	printf("\nAD9081K (K) basic example - JESD204 link bring-up.\n");

	/*
	 * ----------------------------------------------------------------
	 * Deassert Si5391 RSTb (GPIO 133) — drive high to release reset.
	 * ----------------------------------------------------------------
	 */
	{
		struct no_os_gpio_desc *rstb_gpio;
		struct no_os_gpio_init_param rstb_param = {
			.number = AD9081K_RESET_B,
			.platform_ops = &xil_gpio_ops,
			.extra = &xil_gpio_param
		};

		ret = no_os_gpio_get(&rstb_gpio, &rstb_param);
		if (ret) {
			pr_err("GPIO get for RSTb failed: %d\n", ret);
			return ret;
		}
		no_os_gpio_direction_output(rstb_gpio, NO_OS_GPIO_LOW);
		no_os_mdelay(10);
		no_os_gpio_direction_output(rstb_gpio, NO_OS_GPIO_HIGH);
		no_os_mdelay(300);
		no_os_gpio_remove(rstb_gpio);
	}

	/*
	 * ----------------------------------------------------------------
	 * Select I2C mux channel for HPC1 (Si5391 behind PCA9548 @ 0x75).
	 * ----------------------------------------------------------------
	 */
	{
		struct no_os_i2c_desc *i2c_mux;
		struct no_os_i2c_init_param mux_param = si5391_i2c_param;
		uint8_t mux_ch = 0x02;

		mux_param.slave_address = 0x75;
		ret = no_os_i2c_init(&i2c_mux, &mux_param);
		if (ret) {
			pr_err("I2C mux init failed: %d\n", ret);
			return ret;
		}
		ret = no_os_i2c_write(i2c_mux, &mux_ch, 1, 1);
		if (ret) {
			pr_err("I2C mux select failed: %d\n", ret);
			no_os_i2c_remove(i2c_mux);
			return ret;
		}
		no_os_i2c_remove(i2c_mux);
	}

	/*
	 * ----------------------------------------------------------------
	 * Si5391 clock synthesizer setup via I2C.
	 * ----------------------------------------------------------------
	 */
	si5391_param.i2c_init = si5391_i2c_param;
	si5391_param.input_clk_freq = SI5391_INPUT_CLK_FREQ_HZ;
	si5391_param.active_input = SI5391_ACTIVE_INPUT;
	si5391_param.xaxb_ext_clk = false;
	si5391_param.iovdd_33 = true;
	si5391_param.pll_m_num = SI5391_PLL_M_NUM_VAL;
	si5391_param.pll_m_den = SI5391_PLL_M_DEN_VAL;
	si5391_param.reprogram = true;

	si5391_param.output_config[10].always_on = true;
	si5391_param.output_config[10].out_format_drv_bits = 0x09;
	si5391_param.output_config[10].out_cm_ampl_bits = 0x3E;
	si5391_param.output_config[10].vdd_sel_bits = 0x18;

	si5391_param.output_config[9].always_on = true;
	si5391_param.output_config[9].out_format_drv_bits = 0x09;
	si5391_param.output_config[9].out_cm_ampl_bits = 0x3E;
	si5391_param.output_config[9].vdd_sel_bits = 0x18;

	ret = si5391_setup(&si5391_device, &si5391_param);
	if (ret) {
		pr_err("si5391_setup() failed: %d\n", ret);
		return ret;
	}
	pr_info("Si5391 locked and running. Continuing....\n");
	//getchar();

	/*
	 * ----------------------------------------------------------------
	 * ADXCVR initialization — GTH transceivers for TX and RX lanes.
	 * ----------------------------------------------------------------
	 */
	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = AD9081K_LANE_RATE_KHZ,
		.ref_rate_khz = AD9081K_ADXCVR_REF_RATE_KHZ,
		.export_no_os_clk = true
	};

	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = AD9081K_LANE_RATE_KHZ,
		.ref_rate_khz = AD9081K_ADXCVR_REF_RATE_KHZ,
		.export_no_os_clk = true
	};

	ret = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (ret) {
		pr_err("tx_adxcvr_init() failed: %d\n", ret);
		goto error_si5391;
	}
	pr_info("TX ADXCVR initialized\n");

	ret = adxcvr_init(&rx_adxcvr, &rx_adxcvr_init);
	if (ret) {
		pr_err("rx_adxcvr_init() failed: %d\n", ret);
		goto error_tx_adxcvr;
	}
	pr_info("RX ADXCVR initialized\n");

	/*
	 * ----------------------------------------------------------------
	 * AXI JESD204 TX and RX controller initialization.
	 * ----------------------------------------------------------------
	 */
	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = AD9081K_TX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = AD9081K_TX_JESD_FRAMES_PER_MULTIFRAME,
		.converters_per_device = AD9081K_TX_JESD_CONVS_PER_DEVICE,
		.converter_resolution = AD9081K_TX_JESD_CONV_RESOLUTION,
		.bits_per_sample = AD9081K_TX_JESD_BITS_PER_SAMPLE,
		.high_density = AD9081K_TX_JESD_HIGH_DENSITY,
		.control_bits_per_sample = AD9081K_TX_JESD_CTRL_BITS_PER_SAMPLE,
		.subclass = AD9081K_TX_JESD_SUBCLASS,
		.device_clk_khz = AD9081K_DEVICE_CLK_KHZ,
		.lane_clk_khz = AD9081K_LANE_RATE_KHZ
	};

	struct jesd204_rx_init rx_jesd_init = {
		.name = "rx_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = AD9081K_RX_JESD_OCTETS_PER_FRAME,
		.frames_per_multiframe = AD9081K_RX_JESD_FRAMES_PER_MULTIFRAME,
		.subclass = AD9081K_RX_JESD_SUBCLASS,
		.device_clk_khz = AD9081K_DEVICE_CLK_KHZ,
		.lane_clk_khz = AD9081K_LANE_RATE_KHZ,
	};

	tx_jesd_init.lane_clk = tx_adxcvr->clk_out;
	rx_jesd_init.lane_clk = rx_adxcvr->clk_out;

	ret = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (ret) {
		pr_err("axi_jesd204_tx_init() failed: %d\n", ret);
		goto error_rx_adxcvr;
	}
	pr_info("TX JESD204 initialized\n");

	ret = axi_jesd204_rx_init(&rx_jesd, &rx_jesd_init);
	if (ret) {
		pr_err("axi_jesd204_rx_init() failed: %d\n", ret);
		goto error_tx_jesd;
	}
	pr_info("RX JESD204 initialized\n");

	/*
	 * ----------------------------------------------------------------
	 * TX generator initialization (TX TPL).
	 * ----------------------------------------------------------------
	 */
	struct tx_generator_init_param tx_gen_init = {
		.name = "tx_generator",
		.base = TX_CORE_BASEADDR,
		.num_channels = AD9081K_TX_JESD_CONVS_PER_DEVICE,
	};

	ret = tx_generator_init(&tx_generator, &tx_gen_init);
	if (ret) {
		pr_err("tx_generator_init() failed: %d\n", ret);
		goto error_rx_jesd;
	}
	pr_info("TX generator initialized (%d channels)\n",
		tx_generator->num_channels);

	/*
	 * ----------------------------------------------------------------
	 * AXI ADC core initialization (RX TPL).
	 * Uses init_begin + manual reset + post_setup (adrv904x pattern).
	 * ----------------------------------------------------------------
	 */
	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = ADC_CHANNELS,
	};

	ret = axi_adc_init_begin(&rx_adc, &rx_adc_init);
	if (ret) {
		pr_err("axi_adc_init_begin() failed: %d\n", ret);
		goto error_tx_gen;
	}

	axi_adc_write(rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	/* ADC post-setup: channel config per adrv904x_post_setup() */
	axi_adc_write(rx_adc, AXI_ADC_REG_CNTRL, 0);
	for (i = 0; i < rx_adc->num_channels; i++) {
		axi_adc_write(rx_adc, AXI_ADC_REG_CHAN_CNTRL_1(i),
			      AXI_ADC_DCFILT_OFFSET(0));
		axi_adc_write(rx_adc, AXI_ADC_REG_CHAN_CNTRL_2(i),
			      (i & 1) ? 0x00004000 : 0x40000000);
		axi_adc_write(rx_adc, AXI_ADC_REG_CHAN_CNTRL(i),
			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
			      AXI_ADC_ENABLE | AXI_ADC_IQCOR_ENB);
	}
	pr_info("AXI ADC initialized (%d channels)\n", rx_adc->num_channels);

	/*
	 * ----------------------------------------------------------------
	 * AXI DMA engine initialization.
	 * ----------------------------------------------------------------
	 */
	struct axi_dmac_init tx_dmac_init = {
		.name = "tx_dmac",
		.base = TX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED,
	};

	ret = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (ret) {
		pr_err("axi_dmac_init(tx) failed: %d\n", ret);
		goto error_rx_adc;
	}
	pr_info("TX DMA initialized\n");

	struct axi_dmac_init tx_cmd_dmac_init = {
		.name = "tx_cmd_dmac",
		.base = TX_CMD_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED,
	};

	ret = axi_dmac_init(&tx_cmd_dmac, &tx_cmd_dmac_init);
	if (ret) {
		pr_err("axi_dmac_init(tx_cmd) failed: %d\n", ret);
		goto error_tx_dmac;
	}
	pr_info("TX CMD DMA initialized\n");

	struct axi_dmac_init rx_dmac_init = {
		.name = "rx_dmac",
		.base = RX_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED,
	};

	ret = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (ret) {
		pr_err("axi_dmac_init(rx) failed: %d\n", ret);
		goto error_tx_cmd_dmac;
	}
	pr_info("RX DMA initialized\n");

	struct axi_dmac_init rx_cmd_dmac_init = {
		.name = "rx_cmd_dmac",
		.base = RX_CMD_DMA_BASEADDR,
		.irq_option = IRQ_DISABLED,
	};

	ret = axi_dmac_init(&rx_cmd_dmac, &rx_cmd_dmac_init);
	if (ret) {
		pr_err("axi_dmac_init(rx_cmd) failed: %d\n", ret);
		goto error_rx_dmac;
	}
	pr_info("RX CMD DMA initialized\n");

	/*
	 * ----------------------------------------------------------------
	 * AD9081K dummy TRX initialization.
	 * ----------------------------------------------------------------
	 */
	ret = ad9081k_init(&ad9081k_device, &ad9081k_param);
	if (ret) {
		pr_err("ad9081k_init() failed: %d\n", ret);
		goto error_rx_cmd_dmac;
	}
	pr_info("AD9081K dummy device initialized\n");

	/*
	 * ----------------------------------------------------------------
	 * JESD204 topology initialization and FSM start.
	 * ----------------------------------------------------------------
	 */
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = tx_jesd->jdev,
			.link_ids = { AD9081K_DEFRAMER0_LINK_TX },
			.links_number = 1,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = { AD9081K_FRAMER0_LINK_RX },
			.links_number = 1,
		},
		{
			.jdev = ad9081k_device->jdev,
			.link_ids = {AD9081K_DEFRAMER0_LINK_TX, AD9081K_FRAMER0_LINK_RX},
			.links_number = 2,
			.is_top_device = true,
		},
	};

	ret = jesd204_topology_init(&topology, devs, NO_OS_ARRAY_SIZE(devs));
	if (ret) {
		pr_err("jesd204_topology_init() failed: %d\n", ret);
		goto error_ad9081k;
	}
	pr_info("JESD204 topology initialized\n");

	ret = jesd204_fsm_start(topology, JESD204_LINKS_ALL);
	if (ret) {
		pr_err("jesd204_fsm_start() failed: %d\n", ret);
		goto error_fsm;
	}

	pr_info("AD9081K JESD204 link up\n");

	axi_jesd204_tx_status_read(tx_jesd);
	no_os_mdelay(100);
	axi_jesd204_rx_status_read(rx_jesd);
	no_os_mdelay(100);

	/*
	 * ----------------------------------------------------------------
	 * TX CMD DMA transfer (tx_cmd_read).
	 * ----------------------------------------------------------------
	 */

	printf("Press Enter to start TX CMD DMA transfer of CMD READ command...\n");
	getchar();

	tx_generator_cmd_set_stream_source(tx_generator, true);

	{
		static uint32_t cmd_buffer_dma[DAC_BUFFER_SAMPLES]
			__attribute__((aligned(1024)));

		/*
		 * Write CMD data word-by-word via no_os_axi_io_write
		 * instead of tx_generator_load_custom_data, which
		 * replicates each word across num_channels/2 lanes and
		 * corrupts the 64-bit CMD value.  We must use
		 * no_os_axi_io_write (not memcpy) because
		 * axi_dma_transfer.src_addr is uint32_t and truncates
		 * 64-bit pointers — no_os_axi_io_write truncates the
		 * same way, keeping write and DMA read consistent.
		 */
		for (i = 0; i < NO_OS_ARRAY_SIZE(tx_cmd_read); i++)
			no_os_axi_io_write((uintptr_t)cmd_buffer_dma,
					   i * sizeof(uint32_t),
					   tx_cmd_read[i]);

		struct axi_dma_transfer cmd_transfer = {
			.size = sizeof(tx_cmd_read),
			.transfer_done = 0,
			.cyclic = NO,
			.src_addr = (uintptr_t)cmd_buffer_dma,
			.dest_addr = 0,
		};

		Xil_DCacheFlush();

		ret = axi_dmac_transfer_start(tx_cmd_dmac, &cmd_transfer);
		if (ret)
			pr_err("axi_dmac_transfer_start(tx_cmd_read) failed: %d\n", ret);
		else
			pr_info("TX CMD DMA transfer of CMD READ started\n");
	}

	tx_generator_cmd_set_stream_source(tx_generator, false);

	/*
	 * ----------------------------------------------------------------
	 * TX CMD DMA transfer (tx_cmd_streaming_read).
	 * ----------------------------------------------------------------
	 */

	no_os_mdelay(100);
	printf("Press Enter to start TX CMD DMA transfer of CMD STREAMING READ command...\n");
	getchar();

	tx_generator_cmd_set_stream_source(tx_generator, true);

	{
		static uint32_t cmd_buffer_dma[DAC_BUFFER_SAMPLES]
			__attribute__((aligned(1024)));

		/* See CMD READ comment — same channel-replication issue. */
		for (i = 0; i < NO_OS_ARRAY_SIZE(tx_cmd_streaming_read); i++)
			no_os_axi_io_write((uintptr_t)cmd_buffer_dma,
					   i * sizeof(uint32_t),
					   tx_cmd_streaming_read[i]);

		struct axi_dma_transfer cmd_transfer = {
			.size = sizeof(tx_cmd_streaming_read),
			.transfer_done = 0,
			.cyclic = NO,
			.src_addr = (uintptr_t)cmd_buffer_dma,
			.dest_addr = 0,
		};

		Xil_DCacheFlush();

		ret = axi_dmac_transfer_start(tx_cmd_dmac, &cmd_transfer);
		if (ret)
			pr_err("axi_dmac_transfer_start(tx_cmd_streaming_read) failed: %d\n", ret);
		else
			pr_info("TX CMD DMA transfer of CMD STREAMING READ started\n");
	}

	tx_generator_cmd_set_stream_source(tx_generator, false);

	/*
	 * ----------------------------------------------------------------
	 * TX CMD DMA transfer (tx_cmd_write).
	 * ----------------------------------------------------------------
	 */
	
	no_os_mdelay(100);
	printf("Press Enter to start TX CMD DMA transfer of CMD WRITE command...\n");
	getchar();

	tx_generator_cmd_set_stream_source(tx_generator, true);

	{
		static uint32_t cmd_buffer_dma[DAC_BUFFER_SAMPLES]
			__attribute__((aligned(1024)));

		/* See CMD READ comment — same channel-replication issue. */
		for (i = 0; i < NO_OS_ARRAY_SIZE(tx_cmd_write); i++)
			no_os_axi_io_write((uintptr_t)cmd_buffer_dma,
					   i * sizeof(uint32_t),
					   tx_cmd_write[i]);

		struct axi_dma_transfer cmd_transfer = {
			.size = sizeof(tx_cmd_write),
			.transfer_done = 0,
			.cyclic = NO,
			.src_addr = (uintptr_t)cmd_buffer_dma,
			.dest_addr = 0,
		};

		Xil_DCacheFlush();

		ret = axi_dmac_transfer_start(tx_cmd_dmac, &cmd_transfer);
		if (ret)
			pr_err("axi_dmac_transfer_start(tx_cmd_write) failed: %d\n", ret);
		else
			pr_info("TX CMD DMA transfer of CMD WRITE started\n");
	}

	tx_generator_cmd_set_stream_source(tx_generator, false);

	/*
	 * ----------------------------------------------------------------
	 * TX DMA transfer (tx_passthrough, streamid=3).
	 * ----------------------------------------------------------------
	 */

	no_os_mdelay(100);
	printf("Press Enter to start TX DMA transfer of PASS stream...\n");
	getchar();

	tx_generator_load_custom_data(tx_generator, tx_passthrough,
				     NO_OS_ARRAY_SIZE(tx_passthrough),
				     (uintptr_t)dac_buffer_dma);

	struct axi_dma_transfer transfer = {
		.size = sizeof(tx_passthrough),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = (uintptr_t)dac_buffer_dma,
		.dest_addr = 0,
	};

	pr_info("DMA_EXAMPLE Tx PASS: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(uintptr_t)dac_buffer_dma, NO_OS_ARRAY_SIZE(tx_passthrough),
		tx_generator->num_channels,
		8 * sizeof(tx_passthrough[0]));

	Xil_DCacheFlush();

	ret = axi_dmac_transfer_start(tx_dmac, &transfer);
	if (ret)
		pr_err("axi_dmac_transfer_start(tx_passthrough) failed: %d\n", ret);
	else
		pr_info("TX DMA transfer of PASS stream started\n");

#ifdef DMA_CYCLIC_PASS
	/* RX DMA read — same length as TX transfer */
	uint32_t adc_pass_buffer_dma[NO_OS_ARRAY_SIZE(tx_vector)]
		__attribute__((aligned(1024)));

	struct axi_dma_transfer read_pass_transfer = {
		.size = sizeof(tx_vector),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = (uintptr_t)adc_pass_buffer_dma,
	};

	//printf("Press Enter to start Rx transfer...\n");
	//getchar();

	ret = axi_dmac_transfer_start(rx_dmac, &read_pass_transfer);
	if (ret) {
		pr_err("axi_dmac_transfer_start(rx) failed: %d\n", ret);
	} else {
		ret = axi_dmac_transfer_wait_completion(rx_dmac, 2000);
		if (ret)
			pr_err("RX DMA transfer timeout: %d\n", ret);
	}

	Xil_DCacheInvalidateRange((uintptr_t)adc_pass_buffer_dma,
				  sizeof(adc_pass_buffer_dma));

	pr_info("DMA_EXAMPLE Rx: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(uintptr_t)adc_pass_buffer_dma, NO_OS_ARRAY_SIZE(adc_pass_buffer_dma),
		rx_adc->num_channels,
		8 * sizeof(adc_pass_buffer_dma[0]));

	for (i = 0; i < NO_OS_ARRAY_SIZE(adc_pass_buffer_dma); i++)
		printf("  adc_pass_buffer_dma[%2u] = 0x%08lX\n", i,
		       (unsigned long)adc_pass_buffer_dma[i]);
#endif

	/*
	 * ----------------------------------------------------------------
	 * TX DMA transfer (tx_vector, streamid=1).
	 * ----------------------------------------------------------------
	 */
	tx_generator_load_custom_data(tx_generator, tx_vector,
				      NO_OS_ARRAY_SIZE(tx_vector),
				      (uintptr_t)dac_buffer_dma);

	struct axi_dma_transfer vec_transfer = {
		.size = sizeof(tx_vector),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = (uintptr_t)dac_buffer_dma,
		.dest_addr = 0,
	};

	printf("Press Enter to start TX DMA transfer of VEC stream...\n");
	getchar();

	pr_info("DMA_EXAMPLE Tx VEC: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(uintptr_t)dac_buffer_dma, NO_OS_ARRAY_SIZE(tx_vector),
		tx_generator->num_channels,
		8 * sizeof(tx_vector[0]));

	Xil_DCacheFlush();

	ret = axi_dmac_transfer_start(tx_dmac, &vec_transfer);
	if (ret)
		pr_err("axi_dmac_transfer_start(tx_vector) failed: %d\n", ret);
	else
		pr_info("TX DMA transfer of VEC stream started\n");

#ifdef DMA_CYCLIC_PASS
	/* RX DMA read — same length as TX transfer */
	uint32_t adc_vec_buffer_dma[NO_OS_ARRAY_SIZE(tx_vector)]
		__attribute__((aligned(1024)));

	struct axi_dma_transfer read_vec_transfer = {
		.size = sizeof(tx_vector),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = (uintptr_t)adc_vec_buffer_dma,
	};

	//printf("Press Enter to start Rx transfer...\n");
	//getchar();

	ret = axi_dmac_transfer_start(rx_dmac, &read_vec_transfer);
	if (ret) {
		pr_err("axi_dmac_transfer_start(rx) failed: %d\n", ret);
	} else {
		ret = axi_dmac_transfer_wait_completion(rx_dmac, 2000);
		if (ret)
			pr_err("RX DMA transfer timeout: %d\n", ret);
	}

	Xil_DCacheInvalidateRange((uintptr_t)adc_vec_buffer_dma,
				  sizeof(adc_vec_buffer_dma));

	pr_info("DMA_EXAMPLE Rx: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(uintptr_t)adc_vec_buffer_dma, NO_OS_ARRAY_SIZE(adc_vec_buffer_dma),
		rx_adc->num_channels,
		8 * sizeof(adc_vec_buffer_dma[0]));

	for (i = 0; i < NO_OS_ARRAY_SIZE(adc_vec_buffer_dma); i++)
		printf("  adc_vec_buffer_dma[%2u] = 0x%08lX\n", i,
		       (unsigned long)adc_vec_buffer_dma[i]);
#endif


#ifdef IIOD
	/*
	 * ----------------------------------------------------------------
	 * IIO application setup (UART transport).
	 * ----------------------------------------------------------------
	 */
	struct iio_app_desc *app;

	static uint8_t mac_addr[6] = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02};

	struct xemacps_init_param gem_ip = {
		.device_id = GEM_DEVICE_ID,
	};
	memcpy(gem_ip.hwaddr, mac_addr, sizeof(mac_addr));

	struct iio_app_init_param app_init_param = { 0 };

	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_ad9081k_desc *iio_ad9081k_desc;
	struct iio_device *adc_dev_desc, *ad9081k_dev_desc;

	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange,
#endif
	};

	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (ret) {
		pr_err("iio_axi_adc_init() failed: %d\n", ret);
		goto error_fsm;
	}

	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buffer,
		.size = sizeof(adc_buffer),
	};
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);

	ret = iio_ad9081k_init(&iio_ad9081k_desc);
	if (ret) {
		pr_err("iio_ad9081k_init() failed: %d\n", ret);
		goto error_iio_adc;
	}
	iio_ad9081k_get_dev_descriptor(iio_ad9081k_desc, &ad9081k_dev_desc);

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("cf-ad9081k-lpc", iio_axi_adc_desc,
			       adc_dev_desc, &read_buff, NULL, NULL),
		IIO_APP_DEVICE("ad9081k", iio_ad9081k_desc,
			       ad9081k_dev_desc, NULL, NULL, NULL),
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = xilinx_lwip_uart_ip;
	app_init_param.lwip_param.platform_ops = &xemacps_lwip_ops;
	app_init_param.lwip_param.mac_param    = &gem_ip;
	memcpy(app_init_param.lwip_param.hwaddr, mac_addr, sizeof(mac_addr));

	no_os_mdelay(100);

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_err("iio_app_init() failed: %d\n", ret);
		goto error_iio_ad9081k;
	}

	return iio_app_run(app);

error_iio_ad9081k:
	iio_ad9081k_remove(iio_ad9081k_desc);
error_iio_adc:
	iio_axi_adc_remove(iio_axi_adc_desc);
#endif /* IIOD */
error_fsm:
error_ad9081k:
	ad9081k_remove(ad9081k_device);
error_rx_cmd_dmac:
	axi_dmac_remove(rx_cmd_dmac);
error_rx_dmac:
	axi_dmac_remove(rx_dmac);
error_tx_cmd_dmac:
	axi_dmac_remove(tx_cmd_dmac);
error_tx_dmac:
	axi_dmac_remove(tx_dmac);
error_rx_adc:
	axi_adc_remove(rx_adc);
error_tx_gen:
	tx_generator_remove(tx_generator);
error_rx_jesd:
	axi_jesd204_rx_remove(rx_jesd);
error_tx_jesd:
	axi_jesd204_tx_remove(tx_jesd);
error_rx_adxcvr:
	adxcvr_remove(rx_adxcvr);
error_tx_adxcvr:
	adxcvr_remove(tx_adxcvr);
error_si5391:
	si5391_remove(si5391_device);

	return ret;
}
