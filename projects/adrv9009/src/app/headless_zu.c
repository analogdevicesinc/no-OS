/**
 * \file adrv9009/src/app/headless_zu.c
 *
 * \brief Contains example code for user integration with their application. This file contains code for ZU11EG.
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 */

/****< Insert User Includes Here >***/

#include <stdio.h>
#include "talise_radioctrl.h"
#include "no_os_print_log.h"
#include "clk_axi_clkgen.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_dac_core.h"
#include "axi_adc_core.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "xilinx_gpio.h"
#include "no_os_gpio.h"
#include "parameters.h"
#include "no_os_util.h"
#include "xilinx_spi.h"
#include "axi_adxcvr.h"
#include "talise_rx.h"
#include "no_os_spi.h"
#include "xil_cache.h"
#include "axi_dmac.h"
#include "adi_hal.h"

#include "talise_config.h"
#include "app_config.h"
#include "adrv9009.h"
#include "hmc7044.h"
#include "talise.h"

uint32_t dac_buffer_dma[1024 * 16] __attribute__((aligned(1024)));

/***************************************************************************//**
 * @brief Function for obtaining SPI settings.
 *
 * @return SPI settings
*******************************************************************************/
taliseSpiSettings_t *adrv9009_spi_settings_get(void)
{
	return &talInit.spiSettings;
}

/***************************************************************************//**
 * @brief Function for obtaining initial settings.
 *
 * @return initial settings
*******************************************************************************/
taliseInit_t *adrv9009_initial_settings_get(void)
{
	return &talInit;
}


/**********************************************************/
/**********************************************************/

int main(void)
{
	taliseDevice_t adrv9009_device[TALISE_DEVICE_ID_MAX] = { 0 };
	struct adi_hal adrv9009_hal[TALISE_DEVICE_ID_MAX] = { 0 };
	struct adrv9009_init_param adrv9009_init_par = { 0 };
	struct no_os_gpio_init_param gpio_init_plddrbypass;
	struct adrv9009_rf_phy *phy[TALISE_DEVICE_ID_MAX];
	taliseAgcCfg_t agcConfig_init_param = { 0 };
	struct no_os_gpio_desc *gpio_plddrbypass;
	struct hmc7044_dev * car_clkchip_device;
	struct hmc7044_dev* clkchip_device;
	unsigned int t;
	int status;

	// compute the lane rate from profile settings
	// lane_rate = input_rate * M * 20 / L
	// 		where L and M are explained in taliseJesd204bFramerConfig_t comments
	uint32_t rx_lane_rate_khz = talInit.rx.rxProfile.rxOutputRate_kHz *
				    talInit.jesd204Settings.framerA.M * (20 /
						    no_os_hweight8(talInit.jesd204Settings.framerA.serializerLanesEnabled));
	uint32_t rx_div40_rate_hz = rx_lane_rate_khz * (1000 / 40);
	uint32_t tx_lane_rate_khz = talInit.tx.txProfile.txInputRate_kHz *
				    talInit.jesd204Settings.deframerA.M * (20 /
						    no_os_hweight8(talInit.jesd204Settings.deframerA.deserializerLanesEnabled));
	uint32_t tx_div40_rate_hz = tx_lane_rate_khz * (1000 / 40);
	uint32_t rx_os_lane_rate_khz = talInit.obsRx.orxProfile.orxOutputRate_kHz *
				       talInit.jesd204Settings.framerB.M * (20 /
						       no_os_hweight8(talInit.jesd204Settings.framerB.serializerLanesEnabled));
	uint32_t rx_os_div40_rate_hz = rx_os_lane_rate_khz * (1000 / 40);

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *rx_dmac;

	struct axi_dmac_init rx_os_dmac_init = {
		"rx_os_dmac",
		RX_OS_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *rx_os_dmac;

	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *tx_dmac;

	struct xil_spi_init_param hal_spi_param = {
		.type = SPI_PS,
		.flags = SPI_CS_DECODE
	};
	struct xil_gpio_init_param hal_gpio_param = {
		.type = GPIO_PS,
		.device_id = GPIO_DEVICE_ID
	};

	pr_info("Hello (ZU11EG project - JESD FSM)\n");

	struct hmc7044_init_param hmc7044_param = {
		.spi_init = NULL,
		.export_no_os_clk = true,
		.clkin_freq = {30720000, 30720000, 0, 0},
		.vcxo_freq = 122880000,
		.pll2_freq = 2949120000,
		.pll1_loop_bw = 200,
		.sysref_timer_div = 3840,
		.in_buf_mode = {0x09, 0x09, 0x00, 0x00, 0x15},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x11},
		.gpo_ctrl = {0x1f, 0x2b, 0x00, 0x00},
		.num_channels = 10,
		.pll1_ref_prio_ctrl = 0xE5,
		.sync_pin_mode = 0x1,
		.high_performance_mode_clock_dist_en = true,
		.pulse_gen_mode = 0x0,
	};

	// 'DEV_REFCLK_A',          attr 'frequency', value '55000000' |  48
	// 'FPGA_SYSREF_TX_OBS_AB', attr 'frequency', value '2578125'  |  1024
	// 'CORE_CLK_RX_AB',        attr 'frequency', value '82500000' |  32
	// 'JESD_REFCLK_RX_AB',     attr 'frequency', value '82500000' |  32
	// 'FPGA_SYSREF_RX_AB',     attr 'frequency', value '2578125'  |  1024
	// 'JESD_REFCLK_TX_OBS_AB', attr 'frequency', value '165000000' |  16
	// 'DEV_SYSREF_B',          attr 'frequency', value '2578125'  |  1024
	// 'DEV_SYSREF_A',          attr 'frequency', value '2578125'  |  1024
	// 'CORE_CLK_TX_OBS_AB',    attr 'frequency', value '82500000' |  32
	// 'DEV_REFCLK_B',          attr 'frequency', value '55000000' |  48

	struct hmc7044_chan_spec chan_spec[10] = {
		/* DEV_REFCLK_A */
		{
			.disable = 0, .num = 0, .divider = 12, .driver_mode = 2,
			.coarse_delay = 15
		},
		/* DEV_SYSREF_A */
		{
			.disable = 0, .num = 1, .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.force_mute_enable = true,
			.driver_impedance = 1
		},
		/* DEV_REFCLK_B */
		{
			.disable = 0, .num = 2,  .divider = 12, .driver_mode = 2,
			.coarse_delay = 15
		},
		/* DEV_SYSREF_B */
		{
			.disable = 0, .num = 3,  .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.force_mute_enable = true,
			.driver_impedance = 1
		},
		/* JESD_REFCLK_TX_OBS_AB */
		{
			.disable = 0, .num = 4,  .divider = 12, .driver_mode = 1
		},
		/* JESD_REFCLK_RX_AB */
		{
			.disable = 0, .num = 5,  .divider = 12, .driver_mode = 1
		},
		/* CORE_CLK_TX_OBS_AB */
		{
			.disable = 0, .num = 6,
			.divider = hmc7044_param.pll2_freq / tx_div40_rate_hz,
			.driver_mode = 0,
			.driver_impedance = 1
		},
		/* CORE_CLK_RX_AB */
		{
			.disable = 0, .num = 7,
			.divider = hmc7044_param.pll2_freq / rx_div40_rate_hz,
			.driver_mode = 0,
			.driver_impedance = 1
		},
		/* FPGA_SYSREF_TX_OBS_AB */
		{
			.disable = 0, .num = 8, .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.force_mute_enable = true
		},
		/* FPGA_SYSREF_RX_AB */
		{
			.disable = 0, .num = 9, .divider = 3840, .driver_mode = 1,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.force_mute_enable = true
		}
	};
	hmc7044_param.channels = chan_spec;

	struct hmc7044_chan_spec car_chan_spec[2] = {
		/* REFCLK_OUT2 */
		{
			.disable = 0, .num = 2, .divider = 24, .driver_mode = 1,
		},
		/* SYNC_OUT1 */
		{
			.disable = 0, .num = 5, .divider = 3840, .driver_mode = 3,
			.start_up_mode_dynamic_enable = true,
			.high_performance_mode_dis = true,
			.driver_impedance = 3
		}
	};

	car_chan_spec[0].divider = 0x60;

	struct hmc7044_init_param hmc7044_car_param = {
		.spi_init = NULL,
		.clkin_freq = {122880000, 30720000, 0, 38400000},
		.vcxo_freq = 122880000,
		.pll2_freq = 2949120000,
		.pll1_loop_bw = 200,
		.sysref_timer_div = 3840,
		.in_buf_mode = {0x07, 0x07, 0x00, 0x11, 0x15},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x11},
		.gpo_ctrl = {0x1f, 0x2b, 0x00, 0x00},
		.num_channels = 2,
		.pll1_ref_prio_ctrl = 0xB1,
		.sync_pin_mode = 0x1,
		.pulse_gen_mode = 0x1,
		.channels = car_chan_spec,
		.pll1_ref_autorevert_en = true,
	};

	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
		.flags = SPI_CS_DECODE
	};

	// clock chip spi settings
	struct no_os_spi_init_param clkchip_spi_init_param = {
		.device_id = 0,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = CLK_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};

	struct no_os_spi_init_param car_clkchip_spi_init_param = {
		.device_id = 0,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = CAR_CLK_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};
	hmc7044_car_param.spi_init = &car_clkchip_spi_init_param;

	hmc7044_param.spi_init = &clkchip_spi_init_param;

	status = hmc7044_init(&car_clkchip_device, &hmc7044_car_param);
	if (status != 0) {
		printf("hmc7044_init() error: %d\n", status);
		goto error;
	}
	status = hmc7044_init(&clkchip_device, &hmc7044_param);
	if (status != 0) {
		printf("hmc7044_init() error: %d\n", status);
		goto error;
	}

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = TALISE_NUM_CHANNELS
	};

	struct axi_adc_init rx_os_adc_init = {
		.name = "rx_os_adc",
		.base = RX_OS_CORE_BASEADDR,
		.num_channels = TALISE_NUM_CHANNELS / 2
	};

	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.num_channels = TALISE_NUM_CHANNELS,
		.channels = NULL,
		.rate = 3
	};

	rx_lane_rate_khz = rx_div40_rate_hz / 1000 * 40;
	struct jesd204_rx_init rx_jesd_init = {
		.name = "rx_jesd",
		.base = RX_JESD_BASEADDR,
		.device_clk_khz = rx_div40_rate_hz / 1000,
		.lane_clk_khz = rx_lane_rate_khz
	};

	tx_lane_rate_khz = tx_div40_rate_hz / 1000 * 40;
	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.converters_per_device = talInit.jesd204Settings.deframerA.M,
		.bits_per_sample = talInit.jesd204Settings.deframerA.Np,
		.device_clk_khz = tx_div40_rate_hz / 1000,
		.lane_clk_khz = tx_lane_rate_khz
	};
	rx_os_lane_rate_khz = rx_os_div40_rate_hz / 1000 * 40;
	struct jesd204_rx_init rx_os_jesd_init = {
		.name = "rx_os_jesd",
		.base = RX_OS_JESD_BASEADDR,
		.device_clk_khz = rx_os_div40_rate_hz / 1000,
		.lane_clk_khz = rx_os_lane_rate_khz,
	};

	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_PROGDIV_CLK,
		.lpm_enable = 1,
		.lane_rate_khz = rx_lane_rate_khz,
		.ref_rate_khz = talInit.clocks.deviceClock_kHz,
		.export_no_os_clk = true
	};
	struct adxcvr *rx_adxcvr;

	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_PROGDIV_CLK,
		.lpm_enable = 0,
		.lane_rate_khz = tx_lane_rate_khz,
		.ref_rate_khz = talInit.clocks.deviceClock_kHz,
		.export_no_os_clk = true
	};
	struct adxcvr *tx_adxcvr;

	struct adxcvr_init rx_os_adxcvr_init = {
		.name = "rx_os_adxcvr",
		.base = RX_OS_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
		.out_clk_sel = ADXCVR_PROGDIV_CLK,
		.lpm_enable = 1,
		.lane_rate_khz = rx_os_lane_rate_khz,
		.ref_rate_khz = talInit.clocks.deviceClock_kHz,
		.export_no_os_clk = true
	};
	struct adxcvr *rx_os_adxcvr;

	status = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (status)
		goto error;

	status = adxcvr_init(&rx_adxcvr, &rx_adxcvr_init);
	if (status)
		goto error;

	status = adxcvr_init(&rx_os_adxcvr, &rx_os_adxcvr_init);
	if (status)
		goto error;

	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
	struct axi_jesd204_rx *rx_os_jesd;

	rx_jesd_init.lane_clk = rx_adxcvr->clk_out;
	tx_jesd_init.lane_clk = tx_adxcvr->clk_out;
	rx_os_jesd_init.lane_clk = rx_os_adxcvr->clk_out;

	status = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (status)
		goto error;

	status = axi_jesd204_rx_init(&rx_jesd, &rx_jesd_init);
	if (status)
		goto error;

	status = axi_jesd204_rx_init(&rx_os_jesd, &rx_os_jesd_init);
	if (status)
		goto error;

	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		adrv9009_hal[t].extra_gpio = &hal_gpio_param;
		adrv9009_hal[t].extra_spi = &hal_spi_param;
		adrv9009_device[t].devHalInfo = (void *)&adrv9009_hal[t];
	}

	adrv9009_hal[TALISE_A].gpio_adrv_resetb_num = TRX_A_RESETB_GPIO;
	adrv9009_hal[TALISE_A].spi_adrv_csn = ADRV_CS;
	adrv9009_hal[TALISE_B].gpio_adrv_resetb_num = TRX_B_RESETB_GPIO;
	adrv9009_hal[TALISE_B].spi_adrv_csn = ADRV_B_CS;

	adrv9009_init_par.adrv9009_device = &adrv9009_device[TALISE_A];
	adrv9009_init_par.dev_clk = clkchip_device->clk_desc[0];
	adrv9009_init_par.streamImageFile = STREAM;
	adrv9009_init_par.armImageFile = FIRMWARE;
	adrv9009_init_par.rxAgcConfig_init_param = &agcConfig_init_param;

	status = adrv9009_init(&phy[TALISE_A], &adrv9009_init_par);
	if (status) {
		pr_err("error TALISE_A: adrv9009_init() failed\n");
		goto error;
	}

	adrv9009_init_par.adrv9009_device = &adrv9009_device[TALISE_B];

	status = adrv9009_init(&phy[TALISE_B], &adrv9009_init_par);
	if (status) {
		pr_err("error TALISE_B: adrv9009_init() failed\n");
		goto error;
	}

	status = axi_adc_init(&phy[TALISE_A]->rx_adc, &rx_adc_init);
	if (status)
		goto error;

	status = axi_adc_init(&phy[TALISE_A]->rx_os_adc, &rx_os_adc_init);
	if (status)
		goto error;

	status = axi_dac_init(&phy[TALISE_A]->tx_dac, &tx_dac_init);
	if (status)
		goto error;

	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status) {
		pr_info("axi_dmac_init tx init error: %d\n", status);
		goto error;
	}
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status) {
		pr_info("axi_dmac_init rx init error: %d\n", status);
		goto error;
	}
	status = axi_dmac_init(&rx_os_dmac, &rx_os_dmac_init);
	if (status) {
		pr_info("axi_dmac_init rx os init error: %d\n", status);
		goto error;
	}

	phy[TALISE_B]->tx_dac = phy[TALISE_A]->tx_dac;
	phy[TALISE_B]->rx_adc = phy[TALISE_A]->rx_adc;
	phy[TALISE_B]->rx_os_adc = phy[TALISE_A]->rx_os_adc;

	struct jesd204_topology *topology;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = clkchip_device->jdev,
			.link_ids = {DEFRAMER_LINK_TX, FRAMER_LINK_RX, FRAMER_LINK_ORX},
			.links_number = 3,
			.is_sysref_provider = true,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = {FRAMER_LINK_RX},
			.links_number = 1,
		},
		{
			.jdev = rx_os_jesd->jdev,
			.link_ids = {FRAMER_LINK_ORX},
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = {DEFRAMER_LINK_TX},
			.links_number = 1,
		},
		{
			.jdev = phy[TALISE_A]->jdev,
			.link_ids = {DEFRAMER_LINK_TX, FRAMER_LINK_RX, FRAMER_LINK_ORX},
			.links_number = 3,
		},
		{
			.jdev = phy[TALISE_B]->jdev,
			.link_ids = {DEFRAMER_LINK_TX, FRAMER_LINK_RX, FRAMER_LINK_ORX},
			.links_number = 3,
			.is_top_device = true,
		},
	};

	jesd204_topology_init(&topology, devs,
			      sizeof(devs) / sizeof(*devs));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);

	axi_jesd204_rx_status_read(rx_jesd);
	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_os_jesd);

	status = TALISE_setRxGainControlMode(phy[TALISE_A]->talDevice, TAL_AGCSLOW);
	if (status) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_setRxGainControlMode() failed\n");
	}

	status = TALISE_setRxGainControlMode(phy[TALISE_B]->talDevice, TAL_AGCSLOW);
	if (status) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_setRxGainControlMode() failed\n");
	}


	gpio_init_plddrbypass.extra = &hal_gpio_param;
	gpio_init_plddrbypass.platform_ops = &xil_gpio_ops;
	gpio_init_plddrbypass.number = DAC_FIFO_BYPASS_GPIO;
	status = no_os_gpio_get(&gpio_plddrbypass, &gpio_init_plddrbypass);
	if (status) {
		printf("no_os_gpio_get() failed with status %d", status);
		goto error;
	}
	no_os_gpio_direction_output(gpio_plddrbypass, 0);

	Xil_DCacheFlush();

	extern const uint32_t sine_lut_iq[1024];
	axi_dac_set_datasel(phy[TALISE_A]->tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	axi_dac_load_custom_data(phy[TALISE_A]->tx_dac, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq), (uintptr_t)dac_buffer_dma);

	struct axi_dma_transfer transfer = {
		// Number of bytes to write/read
		.size = sizeof(sine_lut_iq),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = (uintptr_t)dac_buffer_dma,
		// Address of data destination
		.dest_addr = 0
	};
	/* Transfer the data. */
	axi_dmac_transfer_start(tx_dmac, &transfer);

	/* Flush cache data. */
	Xil_DCacheInvalidateRange((uintptr_t)dac_buffer_dma,
				  sizeof(sine_lut_iq) * phy[TALISE_A]->tx_dac->num_channels);

	no_os_mdelay(50);

	/* Transfer 16384 samples from ADC to MEM */
	struct axi_dma_transfer transfer_rx = {
		// Number of bytes to write/read
		.size = 16384 * TALISE_NUM_CHANNELS *
		NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)(DDR_MEM_BASEADDR + 0x800000)
	};

	/* Transfer 16384 samples from ADC to MEM */
	struct axi_dma_transfer transfer_os_rx = {
		// Number of bytes to write/read
		.size = 16384 * TALISE_NUM_CHANNELS *
		NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerB.Np, 8),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)(DDR_MEM_BASEADDR + 0xC00000)
	};

	no_os_mdelay(100);

	status = axi_dmac_transfer_start(rx_dmac, &transfer_rx);
	if (status)
		return status;
	status = axi_dmac_transfer_wait_completion(rx_dmac, 500);
	uint8_t num_chans = rx_adc_init.num_channels;
	if (status)
		return status;

	Xil_DCacheInvalidateRange(DDR_MEM_BASEADDR + 0x800000,
				  16384 * TALISE_NUM_CHANNELS *
				  NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));

	printf("Rx DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%u\n",
	       transfer_rx.dest_addr, transfer_rx.size / NO_OS_DIV_ROUND_UP(
		       talInit.jesd204Settings.framerA.Np, 8),
	       num_chans,
	       8 * NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));

	status = TALISE_setRxTxEnable(phy[TALISE_A]->talDevice, TAL_ORX1_EN,
				      TAL_TX1TX2);
	if (status)
		printf("ERROR\n");
	status = TALISE_setRxTxEnable(phy[TALISE_B]->talDevice, TAL_ORX1_EN,
				      TAL_TX1TX2);
	if (status)
		printf("ERROR\n");

	axi_dac_set_datasel(phy[TALISE_A]->tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
	axi_dac_data_setup(phy[TALISE_A]->tx_dac);

	no_os_mdelay(10);

	status = axi_dmac_transfer_start(rx_os_dmac, &transfer_os_rx);
	if (status)
		return status;
	status = axi_dmac_transfer_wait_completion(rx_os_dmac, 500);
	num_chans = rx_os_adc_init.num_channels;
	if (status)
		return status;

	Xil_DCacheInvalidateRange(DDR_MEM_BASEADDR + 0xC00000,
				  16384 * TALISE_NUM_CHANNELS *
				  NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerB.Np, 8));

	printf("ORx DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%u\n",
	       transfer_os_rx.dest_addr, transfer_os_rx.size / NO_OS_DIV_ROUND_UP(
		       talInit.jesd204Settings.framerB.Np, 8),
	       num_chans,
	       8 * NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerB.Np, 8));

error:
	pr_info("Bye\n");

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return 0;
}
