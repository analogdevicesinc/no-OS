/**
 * \file adrv9009/src/app/headless.c
 *
 * \brief Contains example code for user integration with their application
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 */

/****< Insert User Includes Here >***/

#include <stdio.h>
#include "adi_hal.h"
#include "no_os_spi.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "parameters.h"
#include "no_os_util.h"
#include "axi_dac_core.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#ifndef ALTERA_PLATFORM
#include "xil_cache.h"
#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#else
#include "altera_spi.h"
#include "altera_gpio.h"
#endif
#include "adrv9009.h"
#include "talise.h"
#include "talise_config.h"
#include "talise_arm_binary.h"
#include "talise_stream_binary.h"
#include "app_config.h"
#include "app_clocking.h"
#include "app_jesd.h"
#include "app_transceiver.h"
#include "ad9528.h"
#include "hmc7044.h"
#include "jesd204.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"

enum taliseDeviceId {
	TALISE_A = 0u,
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	TALISE_B,
#endif
	TALISE_DEVICE_ID_MAX
};

#define TALISE_NUM_SUBCHANNELS		2 /* I - in-phase and Q - quadrature channels */
#define TALISE_NUM_CHAIN_CHANNELS	2 /* channels per RX/TX chain */
#define TALISE_NUM_CHANNELS		(TALISE_DEVICE_ID_MAX * TALISE_NUM_CHAIN_CHANNELS * TALISE_NUM_SUBCHANNELS)

#ifdef IIO_SUPPORT

#include "iio.h"
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#ifndef ALTERA_PLATFORM
#include "xilinx_uart.h"
#else
#include "altera_uart.h"
#endif

int32_t start_iiod(struct axi_dmac *rx_dmac, struct axi_dmac *tx_dmac,
		   struct axi_adc *rx_adc, struct axi_dac *tx_dac)
{
	struct iio_axi_adc_init_param	iio_axi_adc_init_par;
	struct iio_axi_dac_init_param	iio_axi_dac_init_par;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_axi_adc_desc		*iio_axi_adc_desc;
	struct iio_axi_dac_desc		*iio_axi_dac_desc;
	struct iio_device		*adc_dev_desc;
	struct iio_device		*dac_dev_desc;
	struct iio_app_desc *app;
	int32_t				status;
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

#ifndef ADRV9008_2
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};
#endif
#ifndef ADRV9008_1
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif
	};
#endif

#ifndef ADRV9008_2
	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		return status;
#endif

#ifndef ADRV9008_1
	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if(status < 0)
		return status;
#endif

#ifndef ADRV9008_2
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
#endif
#ifndef ADRV9008_1
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);
#endif

#ifndef ADRV9008_2
	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
#endif
#ifndef ADRV9008_1
	struct iio_data_buffer write_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
#endif

	struct iio_app_device devices[] = {
#ifndef ADRV9008_2
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL, NULL),
#endif
#ifndef ADRV9008_1
		IIO_APP_DEVICE("axi_dac", iio_axi_dac_desc, dac_dev_desc,
			       NULL, &write_buff, NULL)
#endif
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
}

#endif // IIO_SUPPORT

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
extern struct hmc7044_dev* clkchip_device;
extern struct hmc7044_dev * car_clkchip_device;
#else
extern struct ad9528_dev* clkchip_device;
#endif

/**********************************************************/
/**********************************************************/
/********** Talise Data Structure Initializations ********/
/**********************************************************/
/**********************************************************/

int main(void)
{
	adiHalErr_t err;
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

	// compute the local multiframe clock
	// serializer:   lmfc_rate = (lane_rate * 100) / (K * F)
	// deserializer: lmfc_rate = (lane_rate * 100) / (K * 2 * M / L)
	// 		where K, F, L, M are explained in taliseJesd204bFramerConfig_t comments
	uint32_t rx_lmfc_rate = (rx_lane_rate_khz * 100) /
				(talInit.jesd204Settings.framerA.K * talInit.jesd204Settings.framerA.F);
	uint32_t tx_lmfc_rate = (tx_lane_rate_khz * 100) /
				(talInit.jesd204Settings.deframerA.K * 2 * talInit.jesd204Settings.deframerA.M /
				 no_os_hweight8(talInit.jesd204Settings.deframerA.deserializerLanesEnabled));
	uint32_t rx_os_lmfc_rate = (rx_os_lane_rate_khz * 100) /
				   (talInit.jesd204Settings.framerB.K * talInit.jesd204Settings.framerB.F);

	uint32_t lmfc_rate = no_os_min(rx_lmfc_rate, rx_os_lmfc_rate);
	lmfc_rate = no_os_min(tx_lmfc_rate, lmfc_rate);

	struct axi_adc_init rx_adc_init = {
		"rx_adc",
		RX_CORE_BASEADDR,
		TALISE_NUM_CHANNELS
	};
	struct axi_adc *rx_adc;

	struct axi_dac_init tx_dac_init = {
		"tx_dac",
		TX_CORE_BASEADDR,
		TALISE_NUM_CHANNELS,
		NULL,
		3
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

#ifdef DMA_EXAMPLE
	struct no_os_gpio_desc *gpio_plddrbypass;
	struct no_os_gpio_init_param gpio_init_plddrbypass;
	extern const uint32_t sine_lut_iq[1024];
#endif
#ifndef ALTERA_PLATFORM
	struct xil_spi_init_param hal_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.flags = SPI_CS_DECODE
	};
	struct xil_gpio_init_param hal_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
#else
	struct altera_spi_init_param hal_spi_param = {
		.type = NIOS_II_SPI,
		.base_address = SPI_BASEADDR
	};
	struct altera_gpio_init_param hal_gpio_param = {
		.type = NIOS_II_GPIO,
		.device_id = 0,
		.base_address = GPIO_BASEADDR
	};
#endif

#ifndef ALTERA_PLATFORM
	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();
#endif

	printf("Hello\n");

	/**********************************************************/
	/**********************************************************/
	/************ Talise Initialization Sequence *************/
	/**********************************************************/
	/**********************************************************/

	err = clocking_init(rx_div40_rate_hz,
			    tx_div40_rate_hz,
			    rx_os_div40_rate_hz,
			    talInit.clocks.deviceClock_kHz,
			    lmfc_rate);
	if (err != ADIHAL_OK)
		goto error_0;

	err = fpga_xcvr_init(rx_lane_rate_khz,
			     tx_lane_rate_khz,
			     rx_os_lane_rate_khz,
			     talInit.clocks.deviceClock_kHz);
	if (err != ADIHAL_OK)
		goto error_2;

	err = jesd_init(rx_div40_rate_hz,
			tx_div40_rate_hz,
			rx_os_div40_rate_hz);
	if (err != ADIHAL_OK)
		goto error_1;

	struct no_os_spi_init_param phy_spi_init_param = {
		.device_id = 0,
		.max_speed_hz = 25000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = ADRV_CS,
#ifndef ALTERA_PLATFORM
		.platform_ops = &xil_spi_ops,
#else
		.platform_ops = &altera_spi_ops,
#endif
		.extra = &hal_spi_param
	};

	struct no_os_gpio_init_param gpio_adrv_resetb = {
		.number = TRX_A_RESETB_GPIO,
#ifndef ALTERA_PLATFORM
		.platform_ops = &xil_gpio_ops,
#else
		.platform_ops = &altera_gpio_ops,
#endif
		.extra = &hal_gpio_param
	};

	struct no_os_gpio_init_param gpio_adrv_sysref_req = {
		.number = SYSREF_REQ_GPIO,
#ifndef ALTERA_PLATFORM
		.platform_ops = &xil_gpio_ops,
#else
		.platform_ops = &altera_gpio_ops,
#endif
		.extra = &hal_gpio_param
	};

	struct adrv9009_init_param phy_param = {
		.spi_device_id = ID_ADRV9009,
		.spi_init = &phy_spi_init_param,
		.gpio_adrv_resetb_init = &gpio_adrv_resetb,
		.gpio_adrv_sysref_req_init = &gpio_adrv_sysref_req,
		.stream = &streamBinary[0],
		.fw = &armBinary[0],
		.fw_size = sizeof(armBinary),
		.talInit = &talInit,

		.rxAgcCtrl = &rxAgcCtrl,

		.tx1_atten_ctrl_pin_step_size = 0,
		.tx1_atten_ctrl_pin_tx_atten_inc_pin = 4,
		.tx1_atten_ctrl_pin_tx_atten_dec_pin = 5,
		.tx1_atten_ctrl_pin_enable = 0,

		.tx2_atten_ctrl_pin_step_size = 0,
		.tx2_atten_ctrl_pin_tx_atten_inc_pin = 6,
		.tx2_atten_ctrl_pin_tx_atten_dec_pin = 7,
		.tx2_atten_ctrl_pin_enable = 0,

		.tx_pa_protection_avg_duration = 3,
		.tx_pa_protection_tx_atten_step = 2,
		.tx_pa_protection_tx1_power_threshold = 4096,
		.tx_pa_protection_tx2_power_threshold = 4096,
		.tx_pa_protection_peak_count = 4,
		.tx_pa_protection_tx1_peak_threshold = 128,
		.tx_pa_protection_tx2_peak_threshold = 128,

		.aux_dac_enables = 0,
		.aux_dac_vref0 = 3,
		.aux_dac_resolution0 = 0,
		.aux_dac_values0 = 0,
		.aux_dac_vref1 = 3,
		.aux_dac_resolution1 = 0,
		.aux_dac_values1 = 0,
		.aux_dac_vref2 = 3,
		.aux_dac_resolution2 = 0,
		.aux_dac_values2 = 0,
		.aux_dac_vref3 = 3,
		.aux_dac_resolution3 = 0,
		.aux_dac_values3 = 0,
		.aux_dac_vref4 = 3,
		.aux_dac_resolution4 = 0,
		.aux_dac_values4 = 0,
		.aux_dac_vref5 = 3,
		.aux_dac_resolution5 = 0,
		.aux_dac_values5 = 0,
		.aux_dac_vref6 = 3,
		.aux_dac_resolution6 = 0,
		.aux_dac_values6 = 0,
		.aux_dac_vref7 = 3,
		.aux_dac_resolution7 = 0,
		.aux_dac_values7 = 0,
		.aux_dac_vref8 = 3,
		.aux_dac_resolution8 = 0,
		.aux_dac_values8 = 0,
		.aux_dac_vref9 = 3,
		.aux_dac_resolution9 = 0,
		.aux_dac_values9 = 0,
		.aux_dac_values10 = 0,
		.aux_dac_values11 = 0,

		.gpio3v3_source_control = 0,
		.gpio3v3_output_level_mask = 0,
		.gpio3v3_output_enable_mask = 0,

		.dev_clk = clkchip_device->clk_desc[0],

		.trx_pll_lo_frequency_hz = 2000000000,
	};
	struct adrv9009_rf_phy* phy[TALISE_DEVICE_ID_MAX];

	phy_spi_init_param.chip_select = ADRV_CS;
	gpio_adrv_resetb.number = TRX_A_RESETB_GPIO;
	status = adrv9009_init(&phy[TALISE_A], &phy_param);
	if (status != 0)
		printf("adrv9009_init() error: %d\n", status);

#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	phy_spi_init_param.chip_select = ADRV_B_CS;
	gpio_adrv_resetb.number = TRX_B_RESETB_GPIO;
	phy_param.gpio_adrv_sysref_req_init = NULL;
	status = adrv9009_init(&phy[TALISE_B], &phy_param);
	if (status != 0)
		printf("adrv9009_init() error: %d\n", status);
#endif

	struct jesd204_topology *topology;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = clkchip_device->jdev,
			.link_ids = {DEFRAMER_LINK_TX,FRAMER_LINK_RX, FRAMER_LINK_ORX},
			.links_number = 3,
			.is_sysref_provider = true,
		},
#ifdef ZU11EG
		{
			.jdev = car_clkchip_device->jdev,
			.link_ids = {DEFRAMER_LINK_TX,FRAMER_LINK_RX, FRAMER_LINK_ORX},
			.links_number = 3,
		},
#endif
		{
			.jdev = rx_jesd->jdev,
			.link_ids = {FRAMER_LINK_RX},
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = {DEFRAMER_LINK_TX},
			.links_number = 1,
		},
		{
			.jdev = rx_os_jesd->jdev,
			.link_ids = {FRAMER_LINK_ORX},
			.links_number = 1,
		},
#ifdef ZU11EG
		{
			.jdev = phy[TALISE_B]->jdev,
			.link_ids = {DEFRAMER_LINK_TX,FRAMER_LINK_RX, FRAMER_LINK_ORX},
			.links_number = 3,
		},
#endif
		{
			.jdev = phy[TALISE_A]->jdev,
			.link_ids = {DEFRAMER_LINK_TX,FRAMER_LINK_RX, FRAMER_LINK_ORX},
			.links_number = 3,
			.is_top_device = true,
		},
	};

	jesd204_topology_init(&topology, devs,
				  sizeof(devs)/sizeof(*devs));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);

	jesd_rx_watchdog();

	/* Print JESD status */
	jesd_status();

	/* Initialize the DAC core */
#ifndef ADRV9008_1
	status = axi_dac_init(&tx_dac, &tx_dac_init);
	if (status) {
		printf("axi_dac_init() failed with status %d\n", status);
		goto error_3;
	}
#endif

	/* Initialize the ADC core */
#ifndef ADRV9008_2
	status = axi_adc_init(&rx_adc, &rx_adc_init);
	if (status) {
		printf("axi_adc_init() failed with status %d\n", status);
		goto error_3;
	}

#endif
#ifndef ADRV9008_1
	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status) {
		printf("axi_dmac_init() tx init error: %d\n", status);
		goto error_3;
	}

#endif
#ifndef ADRV9008_2
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status) {
		printf("axi_dmac_init() rx init error: %d\n", status);
		goto error_3;
	}
#endif

#ifdef DMA_EXAMPLE
	gpio_init_plddrbypass.extra = &hal_gpio_param;
#ifndef ALTERA_PLATFORM
	gpio_init_plddrbypass.platform_ops = &xil_gpio_ops;
#else
	gpio_init_plddrbypass.platform_ops = &altera_gpio_ops;
#endif
	gpio_init_plddrbypass.number = DAC_FIFO_BYPASS_GPIO;
	status = no_os_gpio_get(&gpio_plddrbypass, &gpio_init_plddrbypass);
	if (status) {
		printf("no_os_gpio_get() failed with status %d", status);
		goto error_3;
	}
	no_os_gpio_direction_output(gpio_plddrbypass, 0);

#ifndef ADRV9008_1
	axi_dac_load_custom_data(tx_dac, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq),
				 DAC_DDR_BASEADDR);
#ifndef ALTERA_PLATFORM
	Xil_DCacheFlush();
#endif

	struct axi_dma_transfer transfer_tx = {
		// Number of bytes to write/read
		.size = sizeof(sine_lut_iq),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
#ifdef IIO_SUPPORT
		.cyclic = CYCLIC,
#else
		.cyclic = NO,
#endif
		// Address of data source
		.src_addr = (uintptr_t)DAC_DDR_BASEADDR,
		// Address of data destination
		.dest_addr = 0
	};
	axi_dmac_transfer_start(tx_dmac, &transfer_tx);
	Xil_DCacheInvalidateRange((uintptr_t)DAC_DDR_BASEADDR, sizeof(sine_lut_iq));

	no_os_mdelay(1000);
#endif

	/* Transfer 16384 samples from ADC to MEM */
#ifndef ADRV9008_2
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
	axi_dmac_transfer_start(rx_dmac, &transfer_rx);
	status = axi_dmac_transfer_wait_completion(rx_dmac, 500);
	if(status)
		return status;
#ifndef ALTERA_PLATFORM
	Xil_DCacheInvalidateRange(DDR_MEM_BASEADDR + 0x800000,
				  16384 * TALISE_NUM_CHANNELS *
				  NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#endif
#endif
#endif

#ifdef IIO_SUPPORT
	status = start_iiod(rx_dmac, tx_dmac, rx_adc, tx_dac);
	if (status)
		printf("Tinyiiod error: %d\n", status);
#endif // IIO_SUPPORT

	jesd204_fsm_stop(topology, JESD204_LINKS_ALL);

error_3:
	fpga_xcvr_deinit();
error_2:
	jesd_deinit();
error_1:
	clocking_deinit();
error_0:
	printf("Bye\n");

#ifndef ALTERA_PLATFORM
	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();
#endif

	return 0;
}
