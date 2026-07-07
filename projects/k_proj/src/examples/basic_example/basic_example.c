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
#include "no_os_axi_io.h"

#include "lwip_xemacps.h"
#include <stdint.h>
#define IIOD
#define DMA_CYCLIC_PASS
#include "xilinx_uart.h"
#ifdef IIOD
#include "iio_axi_adc.h"
#include "iio_ad9081k.h"
#include "iio_app.h"
#endif


static void do_arm_oneshot(uint32_t base, uint32_t tlen)
{
	no_os_axi_io_write(base, 0x084, 0x00);
	no_os_mdelay(1);
	no_os_axi_io_write(base, 0x088, 0x02);
	no_os_axi_io_write(base, 0x01C, tlen);
	no_os_axi_io_write(base, 0x104, 0x00);
	no_os_axi_io_write(base, 0x084, 0x01);
}

static void do_poll_fsm_idle(uint32_t base, const char *label, int timeout_ms)
{
	uint32_t do_fsm;

	do {
		no_os_mdelay(1);
		no_os_axi_io_read(base, 0x200, &do_fsm);
	} while (((do_fsm & 0x1F) != 1 ||
		  ((do_fsm >> 8) & 0xF) != 1) && --timeout_ms);
	printf("%s: wr=%lu rd=%lu\n", label,
	       (unsigned long)(do_fsm & 0x1F),
	       (unsigned long)((do_fsm >> 8) & 0xF));
}

/*
 * 512-bit test VEC stream (MSW..LSW):
 *
 * tx_vector[15] = MSW, tx_vector[0] = LSW.
 * bits[1:0] of word[0] = 0b01 → streamid=1
 */
static uint32_t tx_vector[16] = {
	0x89ABCDE1, /* [0]  lane0[31: 0] from 0x44444445, */
	0x01234567, /* [1]  lane0[63:32] from 0x88888888, */
	0xABCDEF89, /* [2]  lane1[31: 0] from 0xCCCCCCCC, */
	0x12345678, /* [3]  lane1[63:32] from 0x11111110, */
	0xABCDEF01, /* [4]  lane2[31: 0] from 0x55555555, */
	0x23456789, /* [5]  lane2[63:32] from 0x99999999, */
	0xBCDEF012, /* [6]  lane3[31: 0] from 0xDDDDDDDD, */
	0x3456789A, /* [7]  lane3[63:32] from 0x22222221, */
	0xCDEF0123, /* [8]  lane4[31: 0] from 0x66666666, */
	0x456789AB, /* [9]  lane4[63:32] from 0xAAAAAAAA, */
	0xDEF01234, /* [10] lane5[31: 0] from 0xEEEEEEEE, */
	0x56789ABC, /* [11] lane5[63:32] from 0x33333332, */
	0xEF012345, /* [12] lane6[31: 0] from 0x77777777, */
	0x6789ABCD, /* [13] lane6[63:32] from 0xBBBBBBBB, */
	0xF0123456, /* [14] lane7[31: 0] from 0xFFFFFFFF, */
	0x789ABCDE, /* [15] lane7[63:32] from 0x00000005, */
};

/* 512-bit test PASS stream (MSW..LSW):
 *
 * tx_passthrough[15] = MSW, tx_passthrough[0] = LSW.
 * bits[1:0] of word[0] = 0b11 → streamid=3
 */
static uint32_t tx_passthrough[16] = {
	0xDEAD0003, /* [0]  lane0[31: 0] */
	0xBEEF0001, /* [1]  lane0[63:32] */
	0xCAFE0002, /* [2]  lane1[31: 0] */
	0xFACE0003, /* [3]  lane1[63:32] */
	0x12340004, /* [4]  lane2[31: 0] */
	0x56780005, /* [5]  lane2[63:32] */
	0x9ABC0006, /* [6]  lane3[31: 0] */
	0xDEF00007, /* [7]  lane3[63:32] */
	0xA5A50008, /* [8]  lane4[31: 0] */
	0x5A5A0009, /* [9]  lane4[63:32] */
	0xC0DE000A, /* [10] lane5[31: 0] */
	0xF00D000B, /* [11] lane5[63:32] */
	0xBABE000C, /* [12] lane6[31: 0] */
	0xD00D000D, /* [13] lane6[63:32] */
	0xFEED000E, /* [14] lane7[31: 0] */
	0xF00F000F, /* [15] lane7[63:32] */
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
#ifndef ONLY_IIOD
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
	printf("TX DMA: dir=%d src_w=%lu dst_w=%lu max_len=%lu\n",
	       tx_dmac->direction,
	       (unsigned long)tx_dmac->width_src,
	       (unsigned long)tx_dmac->width_dst,
	       (unsigned long)tx_dmac->max_length);

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
	printf("TX CMD DMA: dir=%d src_w=%lu dst_w=%lu max_len=%lu\n",
	       tx_cmd_dmac->direction,
	       (unsigned long)tx_cmd_dmac->width_src,
	       (unsigned long)tx_cmd_dmac->width_dst,
	       (unsigned long)tx_cmd_dmac->max_length);

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
	 * Configure TX data offload for oneshot mode.
	 * Default is cyclic (replays forever after first DMA transfer,
	 * blocking all subsequent transfers through the same data offload).
	 * Oneshot sends each transfer once, then returns to IDLE so the
	 * next DMA transfer can proceed.
	 * Reg 0x084 = Reset (bit0 = sw_resetn)
	 * Reg 0x088 = Control (bit1 = oneshot, bit0 = bypass)
	 * Reg 0x200 = FSM debug (bits[4:0]=wr_fsm, bits[11:8]=rd_fsm)
	 * ----------------------------------------------------------------
	 */
	do_arm_oneshot(TX_DATA_OFFLOAD_BASEADDR,
		       sizeof(tx_passthrough) - 1);
	{
		uint32_t do_ver, do_cfg, do_mem_lo, do_mem_hi;
		uint32_t do_tlen, do_sync_cfg;

		no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, 0x000, &do_ver);
		no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, 0x010, &do_cfg);
		no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, 0x014, &do_mem_lo);
		no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, 0x018, &do_mem_hi);
		no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, 0x01C, &do_tlen);
		no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, 0x104,
				  &do_sync_cfg);

		printf("TX data offload: ver=0x%08lX cfg=0x%lX "
		       "mem=%lu bytes tlen=0x%lX sync_cfg=%lu\n",
		       (unsigned long)do_ver, (unsigned long)do_cfg,
		       (unsigned long)do_mem_lo |
		       ((unsigned long)do_mem_hi << 32),
		       (unsigned long)do_tlen,
		       (unsigned long)do_sync_cfg);
	}
	do_poll_fsm_idle(TX_DATA_OFFLOAD_BASEADDR,
			 "TX data offload init", 100);

	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR, 0x084, 0x00);
	no_os_mdelay(1);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR, 0x088, 0x01);
	no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR, 0x084, 0x01);
	{
		uint32_t do_ver, do_cfg, do_mem_lo, do_mem_hi;
		uint32_t do_tlen, do_sync_cfg;

		no_os_axi_io_read(RX_DATA_OFFLOAD_BASEADDR, 0x000, &do_ver);
		no_os_axi_io_read(RX_DATA_OFFLOAD_BASEADDR, 0x010, &do_cfg);
		no_os_axi_io_read(RX_DATA_OFFLOAD_BASEADDR, 0x014, &do_mem_lo);
		no_os_axi_io_read(RX_DATA_OFFLOAD_BASEADDR, 0x018, &do_mem_hi);
		no_os_axi_io_read(RX_DATA_OFFLOAD_BASEADDR, 0x01C, &do_tlen);
		no_os_axi_io_read(RX_DATA_OFFLOAD_BASEADDR, 0x104,
				  &do_sync_cfg);

		printf("RX data offload: ver=0x%08lX cfg=0x%lX "
		       "mem=%lu bytes tlen=0x%lX sync_cfg=%lu\n",
		       (unsigned long)do_ver, (unsigned long)do_cfg,
		       (unsigned long)do_mem_lo |
		       ((unsigned long)do_mem_hi << 32),
		       (unsigned long)do_tlen,
		       (unsigned long)do_sync_cfg);
	}

	/*
	 * ----------------------------------------------------------------
	 * TX CMD DMA transfer (tx_cmd_read).
	 * ----------------------------------------------------------------
	 */

	tx_generator_cmd_set_stream_source(tx_generator, true);

	printf("Press Enter to start TX CMD DMA transfer of CMD READ command...\n");
	getchar();

	{
		volatile uint32_t *rx_cmd_buf =
			(volatile uint32_t *)(uintptr_t)RX_CMD_DDR_BASEADDR;
		int ch;

		for (i = 0; i < NO_OS_ARRAY_SIZE(tx_cmd_read); i++)
			no_os_axi_io_write(TX_CMD_DDR_BASEADDR,
					   i * sizeof(uint32_t),
					   tx_cmd_read[i]);

		struct axi_dma_transfer cmd_transfer = {
			.size = sizeof(tx_cmd_read),
			.transfer_done = 0,
			.cyclic = NO,
			.src_addr = TX_CMD_DDR_BASEADDR,
			.dest_addr = 0,
		};

		Xil_DCacheFlush();

		struct axi_dma_transfer read_cmd_transfer = {
			.size = sizeof(tx_cmd_read),
			.transfer_done = 0,
			.cyclic = NO,
			.src_addr = 0,
			.dest_addr = RX_CMD_DDR_BASEADDR,
		};

		ret = axi_dmac_transfer_start(rx_cmd_dmac, &read_cmd_transfer);
		if (ret)
			pr_err("axi_dmac_transfer_start(rx_cmd) failed: %d\n", ret);

		ret = axi_dmac_transfer_start(tx_cmd_dmac, &cmd_transfer);
		if (ret)
			pr_err("axi_dmac_transfer_start(tx_cmd_read) failed: %d\n", ret);
		else
			pr_info("TX CMD DMA transfer of CMD READ started\n");

		ret = axi_dmac_transfer_wait_completion(rx_cmd_dmac, 2000);
		if (ret)
			pr_err("RX CMD DMA transfer timeout: %d\n", ret);

		Xil_DCacheInvalidateRange(RX_CMD_DDR_BASEADDR,
					  sizeof(tx_cmd_read));

		for (i = 0; i < NO_OS_ARRAY_SIZE(tx_cmd_read); i++)
			printf("  rx_cmd[%2u] = 0x%08lX\n", i,
			       (unsigned long)rx_cmd_buf[i]);

		printf("\nRX CMD capture loop — Enter to exit, any key + Enter to re-capture.\n");
		ch = getchar();
		while (ch != '\n' && ch != '\r') {
			read_cmd_transfer.transfer_done = 0;

			ret = axi_dmac_transfer_start(rx_cmd_dmac, &read_cmd_transfer);
			if (ret) {
				pr_err("axi_dmac_transfer_start(rx_cmd) failed: %d\n", ret);
			} else {
				ret = axi_dmac_transfer_wait_completion(rx_cmd_dmac, 2000);
				if (ret)
					pr_err("RX CMD DMA transfer timeout: %d\n", ret);
			}

			Xil_DCacheInvalidateRange(RX_CMD_DDR_BASEADDR,
						  sizeof(tx_cmd_read));

			for (i = 0; i < NO_OS_ARRAY_SIZE(tx_cmd_read); i++)
				printf("  rx_cmd[%2u] = 0x%08lX\n", i,
				       (unsigned long)rx_cmd_buf[i]);

			printf("\n");
			ch = getchar();
		}
	}

	/* Read CMD response from rx_analyzer saved registers */
	{
		uint32_t cmd_data, cmd_addr_op;

		axi_adc_read(rx_adc, 0x0C0, &cmd_data);
		axi_adc_read(rx_adc, 0x0C4, &cmd_addr_op);

		printf("RX analyzer CMD response registers:\n");
		printf("  data    = 0x%08lX\n", (unsigned long)cmd_data);
		printf("  addr    = 0x%04lX\n",
		       (unsigned long)((cmd_addr_op >> 16) & 0xFFFF));
		printf("  op      = %lu\n",
		       (unsigned long)((cmd_addr_op >> 8) & 0x3));
		printf("  fifo    = pass_full=%lu vec_full=%lu cmd_full=%lu\n",
		       (unsigned long)((cmd_addr_op >> 4) & 1),
		       (unsigned long)((cmd_addr_op >> 3) & 1),
		       (unsigned long)((cmd_addr_op >> 2) & 1));
		printf("  stream  = 0x%lX\n",
		       (unsigned long)(cmd_addr_op & 0x3));
	}

	/*
	 * ----------------------------------------------------------------
	 * TX CMD DMA transfer (tx_cmd_streaming_read).
	 * ----------------------------------------------------------------
	 */

	no_os_mdelay(100);
	printf("Press Enter to start TX CMD DMA transfer of CMD STREAMING READ command...\n");
	getchar();

	{
		for (i = 0; i < NO_OS_ARRAY_SIZE(tx_cmd_streaming_read); i++)
			no_os_axi_io_write(TX_CMD_DDR_BASEADDR,
					   i * sizeof(uint32_t),
					   tx_cmd_streaming_read[i]);

		struct axi_dma_transfer cmd_transfer = {
			.size = sizeof(tx_cmd_streaming_read),
			.transfer_done = 0,
			.cyclic = NO,
			.src_addr = TX_CMD_DDR_BASEADDR,
			.dest_addr = 0,
		};

		Xil_DCacheFlush();

		ret = axi_dmac_transfer_start(tx_cmd_dmac, &cmd_transfer);
		if (ret)
			pr_err("axi_dmac_transfer_start(tx_cmd_streaming_read) failed: %d\n", ret);
		else
			pr_info("TX CMD DMA transfer of CMD STREAMING READ started\n");
	}

	/*
	 * ----------------------------------------------------------------
	 * TX CMD DMA transfer (tx_cmd_write).
	 * ----------------------------------------------------------------
	 */

	no_os_mdelay(100);
	printf("Press Enter to start TX CMD DMA transfer of CMD WRITE command...\n");
	getchar();

	{
		for (i = 0; i < NO_OS_ARRAY_SIZE(tx_cmd_write); i++)
			no_os_axi_io_write(TX_CMD_DDR_BASEADDR,
					   i * sizeof(uint32_t),
					   tx_cmd_write[i]);

		struct axi_dma_transfer cmd_transfer = {
			.size = sizeof(tx_cmd_write),
			.transfer_done = 0,
			.cyclic = NO,
			.src_addr = TX_CMD_DDR_BASEADDR,
			.dest_addr = 0,
		};

		Xil_DCacheFlush();

		ret = axi_dmac_transfer_start(tx_cmd_dmac, &cmd_transfer);
		if (ret)
			pr_err("axi_dmac_transfer_start(tx_cmd_write) failed: %d\n", ret);
		else
			pr_info("TX CMD DMA transfer of CMD WRITE started\n");
	}

	/*
	 * ----------------------------------------------------------------
	 * TX DMA transfer (tx_passthrough, streamid=3).
	 * ----------------------------------------------------------------
	 */

	no_os_mdelay(100);

	printf("Press Enter to start TX DMA transfer of PASS stream...\n");
	getchar();

	for (i = 0; i < NO_OS_ARRAY_SIZE(tx_passthrough); i++)
		no_os_axi_io_write(TX_DDR_BASEADDR,
				   i * sizeof(uint32_t),
				   tx_passthrough[i]);

	{
		uint32_t readback;

		printf("TX PASS DDR readback:\n");
		for (i = 0; i < NO_OS_ARRAY_SIZE(tx_passthrough); i++) {
			no_os_axi_io_read(TX_DDR_BASEADDR,
					  i * sizeof(uint32_t), &readback);
			printf("  [%2u] wrote=0x%08lX read=0x%08lX %s\n",
			       i, (unsigned long)tx_passthrough[i],
			       (unsigned long)readback,
			       readback == tx_passthrough[i] ? "OK" : "MISMATCH");
		}
	}

	struct axi_dma_transfer transfer = {
		.size = sizeof(tx_passthrough),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = TX_DDR_BASEADDR,
		.dest_addr = 0,
	};

	{
		uint32_t do_fsm;
		no_os_axi_io_read(TX_DATA_OFFLOAD_BASEADDR, 0x200, &do_fsm);
		printf("Data offload before PASS DMA: wr=%lu rd=%lu\n",
		       (unsigned long)(do_fsm & 0x1F),
		       (unsigned long)((do_fsm >> 8) & 0xF));
	}

	pr_info("DMA_EXAMPLE Tx PASS: address=%#x size=%lu\n",
		TX_DDR_BASEADDR, (unsigned long)sizeof(tx_passthrough));

	Xil_DCacheFlush();

#ifdef DMA_CYCLIC_PASS
	{
	volatile uint32_t *rx_pass_buf =
		(volatile uint32_t *)(uintptr_t)RX_DDR_BASEADDR;

	struct axi_dma_transfer read_pass_transfer = {
		.size = sizeof(tx_passthrough),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = RX_DDR_BASEADDR,
	};

	ret = axi_dmac_transfer_start(rx_dmac, &read_pass_transfer);
	if (ret)
		pr_err("axi_dmac_transfer_start(rx_pass) failed: %d\n", ret);
#endif

	ret = axi_dmac_transfer_start(tx_dmac, &transfer);
	if (ret) {
		pr_err("axi_dmac_transfer_start(tx_passthrough) failed: %d\n", ret);
	} else {
		pr_info("TX DMA transfer of PASS stream started\n");
		ret = axi_dmac_transfer_wait_completion(tx_dmac, 2000);
		if (ret)
			pr_err("TX DMA PASS transfer timeout\n");
		else
			pr_info("TX DMA PASS transfer completed\n");
	}

	do_poll_fsm_idle(TX_DATA_OFFLOAD_BASEADDR,
			 "TX data offload after PASS", 200);

#ifdef DMA_CYCLIC_PASS
	ret = axi_dmac_transfer_wait_completion(rx_dmac, 2000);
	if (ret)
		pr_err("RX PASS DMA transfer timeout\n");

	Xil_DCacheInvalidateRange(RX_DDR_BASEADDR, sizeof(tx_passthrough));

	pr_info("DMA_EXAMPLE Rx: address=%#x samples=%lu channels=%u bits=%lu\n",
		RX_DDR_BASEADDR, (unsigned long)NO_OS_ARRAY_SIZE(tx_passthrough),
		rx_adc->num_channels,
		8 * sizeof(tx_passthrough[0]));

	for (i = 0; i < NO_OS_ARRAY_SIZE(tx_passthrough); i++)
		printf("  rx_pass[%2u] = 0x%08lX\n", i,
		       (unsigned long)rx_pass_buf[i]);
	}
#endif

	/*
	 * ----------------------------------------------------------------
	 * TX DMA transfer (tx_vector, streamid=1).
	 * Wait for data offload to finish the pass oneshot cycle,
	 * then start the vec transfer normally.
	 * ----------------------------------------------------------------
	 */
	do_arm_oneshot(TX_DATA_OFFLOAD_BASEADDR,
		       sizeof(tx_vector) - 1);
	do_poll_fsm_idle(TX_DATA_OFFLOAD_BASEADDR,
			 "TX data offload before VEC", 200);

	for (i = 0; i < NO_OS_ARRAY_SIZE(tx_vector); i++)
		no_os_axi_io_write(TX_DDR_BASEADDR,
				   i * sizeof(uint32_t),
				   tx_vector[i]);

	struct axi_dma_transfer vec_transfer = {
		.size = sizeof(tx_vector),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = TX_DDR_BASEADDR,
		.dest_addr = 0,
	};

	printf("Press Enter to start TX DMA transfer of VEC stream...\n");
	getchar();

	pr_info("DMA_EXAMPLE Tx VEC: address=%#x size=%lu\n",
		TX_DDR_BASEADDR, (unsigned long)sizeof(tx_vector));

	Xil_DCacheFlush();

	ret = axi_dmac_transfer_start(tx_dmac, &vec_transfer);
	if (ret) {
		pr_err("axi_dmac_transfer_start(tx_vector) failed: %d\n", ret);
	} else {
		pr_info("TX DMA transfer of VEC stream started\n");
		ret = axi_dmac_transfer_wait_completion(tx_dmac, 2000);
		if (ret)
			pr_err("TX DMA VEC transfer timeout\n");
		else
			pr_info("TX DMA VEC transfer completed\n");
	}

	do_poll_fsm_idle(TX_DATA_OFFLOAD_BASEADDR,
			 "TX data offload after VEC", 200);

	/*
	 * ----------------------------------------------------------------
	 * RX DMA transfer, receiving what DT is sending (streamid=1/2).
	 * Configure DT to send PATT/ASYNC data, then hit enter to capture.
	 * ----------------------------------------------------------------
	 */
	printf("Now configure the DT to be sending PATT/ASYNC. Press Enter after that, to continue...\n");
	getchar();
	printf("Press Enter to start RX DMA transfer of PATT/ASYNC stream from DT...\n");
	getchar();

#ifdef DMA_CYCLIC_PASS
	{
	volatile uint32_t *rx_stream =
		(volatile uint32_t *)(uintptr_t)RX_DDR_BASEADDR;

	struct axi_dma_transfer read_rx_stream_transfer = {
		.size = sizeof(tx_vector),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = RX_DDR_BASEADDR,
	};

	ret = axi_dmac_transfer_start(rx_dmac, &read_rx_stream_transfer);
	if (ret)
		pr_err("axi_dmac_transfer_start(rx_stream) failed: %d\n", ret);

	ret = axi_dmac_transfer_wait_completion(rx_dmac, 2000);
	if (ret)
		pr_err("RX stream DMA transfer timeout\n");

	Xil_DCacheInvalidateRange(RX_DDR_BASEADDR, sizeof(tx_vector));

	pr_info("DMA_EXAMPLE Rx: address=%#x samples=%lu channels=%u bits=%lu\n",
		RX_DDR_BASEADDR, (unsigned long)NO_OS_ARRAY_SIZE(tx_vector),
		rx_adc->num_channels,
		8 * sizeof(tx_vector[0]));

	if ((rx_stream[0] & 0x3) == 1) {
		printf("RX PATT stream detected (streamID=0x1)\n");
	} else if ((rx_stream[0] & 0x3) == 2) {
		printf("RX ASYNC stream detected (streamID=0x2)\n");
	}
	for (i = 0; i < NO_OS_ARRAY_SIZE(tx_vector); i++)
		printf("  rx_stream[%2u] = 0x%08lX\n", i,
		       (unsigned long)rx_stream[i]);
	}
#endif

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

#ifndef ONLY_IIOD
	ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (ret) {
		pr_err("iio_axi_adc_init() failed: %d\n", ret);
		goto error_fsm;
	}
	struct iio_data_buffer read_buff = {
		.buff = (void *)(uintptr_t)ADC_DDR_BASEADDR,
		.size = ADC_BUFFER_SAMPLES * ADC_CHANNELS * sizeof(uint16_t),
	};
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);

	ret = iio_ad9081k_init(&iio_ad9081k_desc);
	if (ret) {
		pr_err("iio_ad9081k_init() failed: %d\n", ret);
		goto error_iio_adc;
	}
	iio_ad9081k_get_dev_descriptor(iio_ad9081k_desc, &ad9081k_dev_desc);
#endif

	struct iio_app_device iio_devices[] = {
#ifndef ONLY_IIOD
		IIO_APP_DEVICE("cf-ad9081k-lpc", iio_axi_adc_desc,
			       adc_dev_desc, &read_buff, NULL, NULL),
		IIO_APP_DEVICE("ad9081k", iio_ad9081k_desc,
			       ad9081k_dev_desc, NULL, NULL, NULL),
#endif
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
