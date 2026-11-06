/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic bring-up example for the AD9082 (VPK180) project.
 *
 *           Initializes the HMC7044 clocks, the AXI-JESD204 RX/TX cores and the
 *           AD9082 (ad9081 driver), runs the JESD204 FSM, and prints the link
 *           status.  No data is moved -- see the (future) dma/iio examples.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "basic_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"

#include <stdio.h>
#include <inttypes.h>

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#include "xil_cache.h"

#include "ad9081.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return 0 on success, negative error code otherwise.
*******************************************************************************/
int example_main(void)
{
	struct no_os_clk app_clk[MULTIDEVICE_INSTANCE_COUNT];
	struct no_os_clk jesd_clk[2];

	/* MxFE reset / sync lines are on an AXI GPIO in the PL (XGpio). */
	struct xil_gpio_init_param xil_gpio_param = {
		.type = GPIO_PL,
#ifdef SDT
		.base_addr = XPAR_XGPIO_0_BASEADDR,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	struct no_os_gpio_init_param gpio_phy_resetb = {
		.number = PHY_RESET,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param
	};

	/* AD9082 (MxFE) on PS SPI0. */
	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
#ifdef SDT
		.base_addr = XPAR_XSPIPS_0_BASEADDR,
#endif
	};
	struct no_os_spi_init_param phy_spi_init_param = {
		.device_id = PHY_SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = PHY_CS,
		.platform_ops = &xil_spi_ops,
		.extra = &xil_spi_param
	};

	struct link_init_param jrx_link_tx = {
		.device_id = 0,
		.octets_per_frame = AD9081_TX_JESD_F,
		.frames_per_multiframe = AD9081_TX_JESD_K,
		.samples_per_converter_per_frame = AD9081_TX_JESD_S,
		.high_density = AD9081_TX_JESD_HD,
		.converter_resolution = AD9081_TX_JESD_N,
		.bits_per_sample = AD9081_TX_JESD_NP,
		.converters_per_device = AD9081_TX_JESD_M,
		.control_bits_per_sample = AD9081_TX_JESD_CS,
		.lanes_per_device = AD9081_TX_JESD_L,
		.subclass = AD9081_TX_JESD_SUBCLASS,
		.link_mode = AD9081_TX_JESD_MODE,
		.dual_link = 0,
		.version = AD9081_TX_JESD_VERSION,
		.logical_lane_mapping = AD9081_TX_LOGICAL_LANE_MAPPING,
		.tpl_phase_adjust = AD9081_JRX_TPL_PHASE_ADJUST
	};
	struct link_init_param jtx_link_rx = {
		.device_id = 0,
		.octets_per_frame = AD9081_RX_JESD_F,
		.frames_per_multiframe = AD9081_RX_JESD_K,
		.samples_per_converter_per_frame = AD9081_RX_JESD_S,
		.high_density = AD9081_RX_JESD_HD,
		.converter_resolution = AD9081_RX_JESD_N,
		.bits_per_sample = AD9081_RX_JESD_NP,
		.converters_per_device = AD9081_RX_JESD_M,
		.control_bits_per_sample = AD9081_RX_JESD_CS,
		.lanes_per_device = AD9081_RX_JESD_L,
		.subclass = AD9081_RX_JESD_SUBCLASS,
		.link_mode = AD9081_RX_JESD_MODE,
		.dual_link = 0,
		.version = AD9081_RX_JESD_VERSION,
		.logical_lane_mapping = AD9081_RX_LOGICAL_LANE_MAPPING,
		.link_converter_select = AD9081_RX_LINK_CONVERTER_SELECT,
	};
	struct ad9081_init_param phy_param = {
		.gpio_reset = &gpio_phy_resetb,
		.spi_init = &phy_spi_init_param,
		.dev_clk = &app_clk[0],
		.jesd_tx_clk = &jesd_clk[1],
		.jesd_rx_clk = &jesd_clk[0],
		.sysref_coupling_ac_en = 0,
		.multidevice_instance_count = 1,
		.jesd_sync_pins_01_swap_enable = false,
		.config_sync_0a_cmos_enable = false,
		.lmfc_delay_dac_clk_cycles = 0,
		.nco_sync_ms_extra_lmfc_num = 0,
		.nco_sync_direct_sysref_mode_enable = 0,
		.sysref_average_cnt_exp = 7,
		.continuous_sysref_mode_disable = 0,
		.tx_disable = false,
		.rx_disable = false,
		/* TX */
		.dac_frequency_hz = AD9081_DAC_FREQUENCY,
		.tx_main_interpolation = AD9081_TX_MAIN_INTERPOLATION,
		.tx_main_nco_frequency_shift_hz = AD9081_TX_MAIN_NCO_SHIFT,
		.tx_dac_channel_crossbar_select = AD9081_TX_DAC_CHAN_CROSSBAR,
		.tx_maindp_dac_1x_non1x_crossbar_select = AD9081_TX_DAC_1X_NON1X_CROSSBAR,
		.tx_full_scale_current_ua = AD9081_TX_FSC,
		.tx_channel_interpolation = AD9081_TX_CHAN_INTERPOLATION,
		.tx_channel_nco_frequency_shift_hz = AD9081_TX_CHAN_NCO_SHIFT,
		.tx_channel_gain = AD9081_TX_CHAN_GAIN,
		.jrx_link_tx[0] = &jrx_link_tx,
		.jrx_link_tx[1] = NULL,
		/* RX */
		.adc_frequency_hz = AD9081_ADC_FREQUENCY,
		.nyquist_zone = AD9081_ADC_NYQUIST_ZONE,
		.rx_main_nco_frequency_shift_hz = AD9081_RX_MAIN_NCO_SHIFT,
		.rx_main_decimation = AD9081_RX_MAIN_DECIMATION,
		.rx_main_complex_to_real_enable = {0, 0, 0, 0},
		.rx_main_digital_gain_6db_enable = {0, 0, 0, 0},
		.rx_main_enable = AD9081_RX_MAIN_ENABLE,
		.rx_channel_nco_frequency_shift_hz = AD9081_RX_CHAN_NCO_SHIFT,
		.rx_channel_decimation = AD9081_RX_CHAN_DECIMATION,
		.rx_channel_complex_to_real_enable = {0, 0, 0, 0, 0, 0, 0, 0},
		.rx_channel_nco_mixer_mode = {
			AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF,
			AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF,
			AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF, AD9081_ADC_NCO_VIF
		},
		.rx_channel_digital_gain_6db_enable = {0, 0, 0, 0, 0, 0, 0, 0},
		.rx_channel_enable = AD9081_RX_CHAN_ENABLE,
		.jtx_link_rx[0] = &jtx_link_rx,
		.jtx_link_rx[1] = NULL,
		.leader_follower_sync = false,
		.nco_test = false,
	};

	struct axi_adc_init rx_adc_init = {
		.name = "rx_adc",
		.base = RX_CORE_BASEADDR
	};
	struct axi_adc *rx_adc;
	struct axi_dac_init tx_dac_init = {
		.name = "tx_dac",
		.base = TX_CORE_BASEADDR,
		.channels = NULL
	};
	struct axi_dac *tx_dac;
	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *rx_dmac;
	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *tx_dmac;
	struct ad9081_phy *phy[MULTIDEVICE_INSTANCE_COUNT];
	int32_t status;
	int32_t i;

	printf("Hello, this is the basic example for ad9082\n");

	/* Self-report the core so the console log identifies APU vs RPU builds. */
#if defined(__aarch64__)
	printf("Running on Cortex-A72 (APU, aarch64)\n");
#elif defined(__arm__)
	printf("Running on Cortex-R5F (RPU, armv7-r)\n");
#endif

	/* Enable the instruction and data caches. */
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	status = app_clock_init(app_clk);
	if (status != 0)
		printf("app_clock_init() error: %" PRId32 "\n", status);

	status = app_jesd_init(jesd_clk, ADXCVR_REF_CLK_KHZ,
			       ADXCVR_RX_DEV_CLK_KHZ, ADXCVR_TX_DEV_CLK_KHZ,
			       ADXCVR_RX_LANE_CLK_KHZ, ADXCVR_TX_LANE_CLK_KHZ);
	if (status != 0)
		printf("app_jesd_init() error: %" PRId32 "\n", status);

	rx_adc_init.num_channels = 0;
	tx_dac_init.num_channels = 0;

	for (i = 0; i < MULTIDEVICE_INSTANCE_COUNT; i++) {
		gpio_phy_resetb.number = PHY_RESET + i;
		phy_spi_init_param.chip_select = PHY_CS + i;
		phy_param.dev_clk = &app_clk[i];
		jtx_link_rx.device_id = i;

		status = ad9081_init(&phy[i], &phy_param);
		if (status != 0)
			printf("ad9081_init() error: %" PRId32 "\n", status);

		rx_adc_init.num_channels += phy[i]->jtx_link_rx[0].jesd_param.jesd_m +
					    phy[i]->jtx_link_rx[1].jesd_param.jesd_m;

		tx_dac_init.num_channels += phy[i]->jrx_link_tx[0].jesd_param.jesd_m *
					    (phy[i]->jrx_link_tx[0].jesd_param.jesd_duallink > 0 ? 2 : 1);
	}

	struct jesd204_topology *topology;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = hmc7044_dev->jdev,
			.link_ids = {FRAMER_LINK0_RX, DEFRAMER_LINK0_TX},
			.links_number = 2,
			.is_sysref_provider = true,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = {FRAMER_LINK0_RX},
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = {DEFRAMER_LINK0_TX},
			.links_number = 1,
		},
		{
			.jdev = phy[0]->jdev,
			.link_ids = {FRAMER_LINK0_RX, DEFRAMER_LINK0_TX},
			.links_number = 2,
			.is_top_device = true,
		},
	};

	jesd204_topology_init(&topology, devs,
			      sizeof(devs) / sizeof(*devs));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);

	axi_jesd204_rx_watchdog(rx_jesd);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	axi_dac_init(&tx_dac, &tx_dac_init);
	axi_adc_init(&rx_adc, &rx_adc_init);

	axi_dmac_init(&tx_dmac, &tx_dmac_init);
	axi_dmac_init(&rx_dmac, &rx_dmac_init);

	printf("Bye\n");

	/* Disable the data and instruction caches. */
	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
