/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example for K (AD9081K) project.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "jesd204.h"
#include "xil_cache.h"
#include "xil_io.h"

static uint32_t dac_buffer_dma[DAC_BUFFER_SAMPLES] __attribute__((aligned(1024)));

/*
 * 512-bit base sequence (MSW..LSW):
 *   0x00000001 FFFFFFFF EEEEEEEE DDDDDDDD
 *   CCCCCCCC BBBBBBBB AAAAAAAA 99999999
 *   88888888 77777777 66666666 55555555
 *   44444444 33333333 22222222 11111111
 *
 * Shifted left by 2 as a 512-bit integer, then bit 0 set to 1.
 * test_vector[15] = MSW, test_vector[0] = LSW.
 */
static uint32_t test_vector[16] = {
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

/* 512-bit test vector: all 0xFFs */
static uint32_t test_vector_allf[16] = {
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
};

/* 64-bit test vector: 0x0000000002540100 */
static uint32_t test_vector_2[2] = {
	0x02540100, /* [0] LSW */
	0x00000000, /* [1] MSW */
};


int example_main()
{
	static struct si5391_init_param si5391_param = { 0 };
	struct si5391_dev *si5391_device = NULL;
	static struct ad9081k_init_param ad9081k_param = { 0 };
	struct ad9081k_dev *ad9081k_device = NULL;
	struct jesd204_topology *topology = NULL;
	struct axi_jesd204_rx *rx_jesd = NULL;
	struct axi_jesd204_tx *tx_jesd = NULL;
	struct adxcvr *tx_adxcvr = NULL;
	struct adxcvr *rx_adxcvr = NULL;
	struct axi_adc *rx_adc = NULL;
	struct axi_dac *tx_dac = NULL;
	struct axi_dmac *rx_dmac = NULL;
	struct axi_dmac *tx_dmac = NULL;
	struct axi_dmac *rx_cmd_dmac = NULL;
	struct axi_dmac *tx_cmd_dmac = NULL;
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
	pr_info("Si5391 locked and running. press enter to continue....\n");
	getchar();

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
	 * AXI DAC core initialization (TX TPL).
	 * ----------------------------------------------------------------
	 */
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.num_channels = AD9081K_TX_JESD_CONVS_PER_DEVICE,
		.channels = NULL,
		.rate = 1,
	};

	ret = axi_dac_init(&tx_dac, &tx_dac_init);
	if (ret) {
		pr_err("axi_dac_init() failed: %d\n", ret);
		goto error_rx_jesd;
	}
	pr_info("AXI DAC initialized (%d channels)\n",
		tx_dac->num_channels);

	ret = axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
	if (ret)
		pr_err("axi_dac_set_datasel(DDS) failed: %d\n", ret);

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
		goto error_tx_dac;
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

	printf("Press Enter to start cmd transfer...\n");
		getchar();

	/* Set bit 1 at AXI DAC base + 0xC8 (read-modify-write) */
	{
		uint32_t reg = Xil_In32(TX_CORE_BASEADDR + 0xC8);
		Xil_Out32(TX_CORE_BASEADDR + 0xC8, reg | (1 << 1));
	}

	/*
	 * ----------------------------------------------------------------
	 * TX CMD DMA transfer (test_vector_2).
	 * ----------------------------------------------------------------
	 */
	{
		static uint32_t cmd_buffer_dma[DAC_BUFFER_SAMPLES]
			__attribute__((aligned(1024)));

		axi_dac_load_custom_data(tx_dac, test_vector_2,
					 NO_OS_ARRAY_SIZE(test_vector_2),
					 (uintptr_t)cmd_buffer_dma);

		struct axi_dma_transfer cmd_transfer = {
			.size = sizeof(test_vector_2),
			.transfer_done = 0,
			.cyclic = NO,
			.src_addr = (uintptr_t)cmd_buffer_dma,
			.dest_addr = 0,
		};

		Xil_DCacheFlush();

		ret = axi_dmac_transfer_start(tx_cmd_dmac, &cmd_transfer);
		if (ret)
			pr_err("axi_dmac_transfer_start(tx_cmd) failed: %d\n", ret);
		else
			pr_info("TX CMD DMA transfer started\n");
	}

	/* Clear bit 1 at AXI DAC base + 0xC8 (restore) */
	{
		uint32_t reg = Xil_In32(TX_CORE_BASEADDR + 0xC8);
		Xil_Out32(TX_CORE_BASEADDR + 0xC8, reg & ~(1 << 1));
	}

	/*
	 * ----------------------------------------------------------------
	 * TX DMA transfer.
	 * ----------------------------------------------------------------
	 */
#ifdef DMA_CYCLIC_PASS
	test_vector[0] = 0x44444447;
	test_vector[15] = 0x00000007;
#endif

	axi_dac_load_custom_data(tx_dac, test_vector_allf,
				 NO_OS_ARRAY_SIZE(test_vector_allf),
				 (uintptr_t)dac_buffer_dma);

	ret = axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	if (ret)
		pr_err("axi_dac_set_datasel(DMA) failed: %d\n", ret);

	struct axi_dma_transfer transfer = {
		.size = sizeof(test_vector_allf),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = (uintptr_t)dac_buffer_dma,
		.dest_addr = 0,
	};

	printf("Press Enter to start stream transfer...\n");
	getchar();

	pr_info("DMA_EXAMPLE Tx: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(uintptr_t)dac_buffer_dma, NO_OS_ARRAY_SIZE(test_vector_allf),
		tx_dac->num_channels,
		8 * sizeof(test_vector_allf[0]));

	ret = axi_dmac_transfer_start(tx_dmac, &transfer);
	if (ret)
		pr_err("axi_dmac_transfer_start(tx) failed: %d\n", ret);
	else
		pr_info("TX DMA transfer started\n");

	Xil_DCacheFlush();

#ifdef DMA_CYCLIC_PASS
	/* RX DMA read — same length as TX transfer */
	uint32_t adc_buffer_dma[NO_OS_ARRAY_SIZE(test_vector)]
		__attribute__((aligned(1024)));

	struct axi_dma_transfer read_transfer = {
		.size = sizeof(test_vector),
		.transfer_done = 0,
		.cyclic = NO,
		.src_addr = 0,
		.dest_addr = (uintptr_t)adc_buffer_dma,
	};

	printf("Press Enter to start Rx transfer...\n");
	getchar();

	ret = axi_dmac_transfer_start(rx_dmac, &read_transfer);
	if (ret) {
		pr_err("axi_dmac_transfer_start(rx) failed: %d\n", ret);
	} else {
		ret = axi_dmac_transfer_wait_completion(rx_dmac, 2000);
		if (ret)
			pr_err("RX DMA transfer timeout: %d\n", ret);
	}

	Xil_DCacheInvalidateRange((uintptr_t)adc_buffer_dma,
				  sizeof(adc_buffer_dma));

	pr_info("DMA_EXAMPLE Rx: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(uintptr_t)adc_buffer_dma, NO_OS_ARRAY_SIZE(adc_buffer_dma),
		rx_adc->num_channels,
		8 * sizeof(adc_buffer_dma[0]));

	for (i = 0; i < NO_OS_ARRAY_SIZE(adc_buffer_dma); i++)
		printf("  adc_buffer_dma[%2u] = 0x%08lX\n", i,
		       (unsigned long)adc_buffer_dma[i]);
#endif

	pr_info("Done. Halting.\n");
	while (1)
		;

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
error_tx_dac:
	axi_dac_remove(tx_dac);
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
