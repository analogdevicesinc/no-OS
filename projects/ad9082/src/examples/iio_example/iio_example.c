/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  IIO example for the AD9082 (VPK180) project.
 *
 *           Brings the HMC7044 clocks, the AXI-JESD204 RX/TX cores and the
 *           AD9082 (ad9081 driver) up exactly like the basic example, then
 *           exposes the AXI ADC and DAC over the IIO framework
 *           (iio_axi_adc / iio_axi_dac) and runs iio_app_run() so an IIO host
 *           can capture ADC samples and inject DAC samples over the console
 *           UART.
 *
 *           NOTE: the PL DMACs master DDR over the Versal NoC (routed to the
 *           DDR controller by the ADI vpk180 HDL) and are not cache-coherent
 *           with the CPU; the IIO AXI ADC/DAC drivers keep the capture/playback
 *           buffers coherent through the dcache invalidate/flush callbacks
 *           passed below.  The buffers are 1 KB aligned and sized to whole cache
 *           lines so those cache ops never touch neighbouring data.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "iio_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"

#include <stdio.h>
#include <inttypes.h>

#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#include "xilinx_uart.h"
#include "xil_cache.h"

#include "ad9081.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"

#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "iio_app.h"

/* Capture / playback buffers for the IIO data transport.  Sized to
 * MAX_{ADC,DAC}_BUF_SAMPLES (parameters.h), as in the ad9081 project, so the
 * iiod can serve large host buffer requests; 1 KB aligned so the DMAC cache
 * ops the IIO drivers issue never spill into neighbouring data. */
uint32_t dac_buffer[MAX_DAC_BUF_SAMPLES] __attribute__((aligned(1024)));
uint16_t adc_buffer[MAX_ADC_BUF_SAMPLES] __attribute__((aligned(1024)));

/* The IIO drivers take 32-bit cache callbacks, while the Xilinx cache API
 * takes INTPTR (64-bit on the A72): wrap instead of casting the function
 * pointer.  Define AD9082_IIO_FULL_DCACHE_FLUSH to clean+invalidate the
 * whole dcache instead of a range (debug aid for coherency issues). */
static void iio_dcache_invalidate(uint32_t address, uint32_t bytes_count)
{
#ifdef AD9082_IIO_FULL_DCACHE_FLUSH
	Xil_DCacheFlush();
#else
	Xil_DCacheInvalidateRange((INTPTR)address, (INTPTR)bytes_count);
#endif
}

static void iio_dcache_flush(uint32_t address, uint32_t bytes_count)
{
#ifdef AD9082_IIO_FULL_DCACHE_FLUSH
	Xil_DCacheFlush();
#else
	Xil_DCacheFlushRange((INTPTR)address, (INTPTR)bytes_count);
#endif
}

/***************************************************************************//**
 * @brief IIO example main execution.
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
		/* Keep the DAC at baseband (no main-NCO upconversion), as in
		 * dma_example: the shared profile's 1 GHz shift aliases to broadband
		 * hash in this full-bandwidth RX profile, so an IIO capture of the
		 * loopback would be unreadable.  The IIO host can still retune the NCO
		 * at runtime.  Matches the Linux m1-l8 reference (shift = 0). */
		.tx_main_nco_frequency_shift_hz = {0, 0, 0, 0},
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

	/* IIO transport / app descriptors. */
	struct xil_uart_init_param platform_uart_init_par = {
		.type = UART_PS,
#ifdef SDT
		.base_addr = XPAR_XUARTPSV_0_BASEADDR,
#endif
		.irq_id = UART_IRQ_ID
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
	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	struct iio_axi_adc_desc *iio_axi_adc_desc;
	struct iio_axi_dac_desc *iio_axi_dac_desc;
	struct iio_device *adc_dev_desc, *dac_dev_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	printf("Hello, this is the iio example for ad9082\n");

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
	if (status != 0) {
		printf("app_clock_init() error: %" PRId32 "\n", status);
		goto error;
	}

	status = app_jesd_init(jesd_clk, ADXCVR_REF_CLK_KHZ,
			       ADXCVR_RX_DEV_CLK_KHZ, ADXCVR_TX_DEV_CLK_KHZ,
			       ADXCVR_RX_LANE_CLK_KHZ, ADXCVR_TX_LANE_CLK_KHZ);
	if (status != 0) {
		printf("app_jesd_init() error: %" PRId32 "\n", status);
		goto error_clk;
	}

	rx_adc_init.num_channels = 0;
	tx_dac_init.num_channels = 0;

	for (i = 0; i < MULTIDEVICE_INSTANCE_COUNT; i++) {
		gpio_phy_resetb.number = PHY_RESET + i;
		phy_spi_init_param.chip_select = PHY_CS + i;
		phy_param.dev_clk = &app_clk[i];
		jtx_link_rx.device_id = i;

		status = ad9081_init(&phy[i], &phy_param);
		if (status != 0) {
			printf("ad9081_init() error: %" PRId32 "\n", status);
			goto error_phy;
		}

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

	status = jesd204_topology_init(&topology, devs,
				       sizeof(devs) / sizeof(*devs));
	if (status) {
		printf("jesd204_topology_init() error: %" PRId32 "\n", status);
		goto error_phy;
	}

	status = jesd204_fsm_start(topology, JESD204_LINKS_ALL);
	if (status) {
		printf("jesd204_fsm_start() error: %" PRId32 "\n", status);
		goto error_phy;
	}

	axi_jesd204_rx_watchdog(rx_jesd);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	status = axi_dac_init(&tx_dac, &tx_dac_init);
	if (status != 0) {
		printf("axi_dac_init() error: %" PRId32 "\n", status);
		goto error_phy;
	}
	status = axi_adc_init(&rx_adc, &rx_adc_init);
	if (status != 0) {
		printf("axi_adc_init() error: %" PRId32 "\n", status);
		goto error_dac;
	}

	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status != 0) {
		printf("axi_dmac_init() tx error: %" PRId32 "\n", status);
		goto error_adc;
	}
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status != 0) {
		printf("axi_dmac_init() rx error: %" PRId32 "\n", status);
		goto error_txdmac;
	}

	/* Default the DACs to the internal DDS tone; the IIO host switches the
	 * data source to the DMA buffer when it pushes a TX stream. */
	status = axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
	if (status) {
		printf("axi_dac_set_datasel() error: %" PRId32 "\n", status);
		goto error_rxdmac;
	}

	/* ---- expose the AXI ADC over IIO ---- */
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
		.dcache_invalidate_range = iio_dcache_invalidate,
	};
	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		goto error_rxdmac;
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
	struct iio_data_buffer read_buff = {
		.buff = (void *)adc_buffer,
		.size = sizeof(adc_buffer),
	};

	/* ---- expose the AXI DAC over IIO ---- */
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
		.dcache_flush_range = iio_dcache_flush,
	};
	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if (status < 0)
		goto error_adc_iio;
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);
	struct iio_data_buffer write_buff = {
		.buff = (void *)dac_buffer,
		.size = sizeof(dac_buffer),
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi-ad9082-rx", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL, NULL),
		IIO_APP_DEVICE("axi-ad9082-tx", iio_axi_dac_desc, dac_dev_desc,
			       NULL, &write_buff, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	/* Let the bring-up log drain before the IIO transport takes the UART. */
	no_os_mdelay(100);

	status = iio_app_init(&app, app_init_param);
	if (status)
		goto error_dac_iio;

	return iio_app_run(app);

error_dac_iio:
	iio_axi_dac_remove(iio_axi_dac_desc);
error_adc_iio:
	iio_axi_adc_remove(iio_axi_adc_desc);
error_rxdmac:
	axi_dmac_remove(rx_dmac);
error_txdmac:
	axi_dmac_remove(tx_dmac);
error_adc:
	axi_adc_remove(rx_adc);
error_dac:
	axi_dac_remove(tx_dac);
error_phy:
	while (i--)
		ad9081_remove(phy[i]);
	axi_jesd204_tx_remove(tx_jesd);
	axi_jesd204_rx_remove(rx_jesd);
error_clk:
	hmc7044_remove(hmc7044_dev);
error:
	printf("Error %" PRId32 "\n", status);

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return status;
}
