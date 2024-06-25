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
#include "talise.h"
#include "talise_config.h"
#include "app_config.h"
#include "app_clocking.h"
#include "app_jesd.h"
#include "app_transceiver.h"
#include "app_talise.h"
#include "ad9528.h"

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

	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};

#ifndef ADRV9008_1
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif
	};
#endif

	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		return status;

#ifndef ADRV9008_1
	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if(status < 0)
		return status;
#endif

	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
#ifndef ADRV9008_1
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);
#endif

	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
#ifndef ADRV9008_1
	struct iio_data_buffer write_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
#endif

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL, NULL),
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

	struct axi_adc_init rx_os_adc_init = {
		"rx_os_adc",
		RX_OS_CORE_BASEADDR,
		TALISE_NUM_CHANNELS / 2
	};
	struct axi_adc *rx_os_adc;

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

	hal.extra_gpio = &hal_gpio_param;
#endif
	int t;
	struct adi_hal hal[TALISE_DEVICE_ID_MAX];
	taliseDevice_t tal[TALISE_DEVICE_ID_MAX];
	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		hal[t].extra_gpio= &hal_gpio_param;
		hal[t].extra_spi = &hal_spi_param;
		tal[t].devHalInfo = (void *) &hal[t];
	}
	hal[TALISE_A].gpio_adrv_resetb_num = TRX_A_RESETB_GPIO;
	hal[TALISE_A].spi_adrv_csn = ADRV_CS;
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	hal[TALISE_B].gpio_adrv_resetb_num = TRX_B_RESETB_GPIO;
	hal[TALISE_B].spi_adrv_csn = ADRV_B_CS;
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

	err = jesd_init(rx_div40_rate_hz,
			tx_div40_rate_hz,
			rx_os_div40_rate_hz);
	if (err != ADIHAL_OK)
		goto error_1;

	err = fpga_xcvr_init(rx_lane_rate_khz,
			     tx_lane_rate_khz,
			     rx_os_lane_rate_khz,
			     talInit.clocks.deviceClock_kHz);
	if (err != ADIHAL_OK)
		goto error_2;

	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		err = talise_setup(&tal[t], &talInit);
		if (err != ADIHAL_OK)
			goto error_3;
	}
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	printf("Performing multi-chip synchronization...\n");
	for(int i=0; i < 12; i++) {
		for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
			err = talise_multi_chip_sync(&tal[t], i);
			if (err != ADIHAL_OK)
				goto error_3;
		}
	}
#endif
	ADIHAL_sysrefReq(tal[TALISE_A].devHalInfo, SYSREF_CONT_ON);

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
	status = axi_adc_init(&rx_os_adc, &rx_os_adc_init);
	if (status) {
		printf("OBS axi_adc_init() failed with status %d\n", status);
		goto error_3;
	}

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

	status = axi_dmac_init(&rx_os_dmac, &rx_os_dmac_init);
	if (status) {
		printf("OBS axi_dmac_init() rx init error: %d\n", status);
		goto error_3;
	}

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
	no_os_gpio_direction_output(gpio_plddrbypass, 1);

#ifndef ADRV9008_1
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
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
#ifndef ADRV9008_2
	status = axi_dmac_transfer_start(rx_dmac, &transfer_rx);
	if(status)
		return status;
	printf("Rx ");
	status = axi_dmac_transfer_wait_completion(rx_dmac, 500);
	uint8_t num_chans = rx_adc_init.num_channels;
#else
	status = axi_dmac_transfer_start(rx_os_dmac, &transfer_rx);
	if(status)
		return status;
	printf("Rx obs ");
	status = axi_dmac_transfer_wait_completion(rx_os_dmac, 500);
	uint8_t num_chans = rx_os_adc_init.num_channels;
#endif
	if(status)
		return status;
#ifndef ALTERA_PLATFORM
	Xil_DCacheInvalidateRange(DDR_MEM_BASEADDR + 0x800000,
				  16384 * TALISE_NUM_CHANNELS *
				  NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#endif
	printf("DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%u\n",
	       transfer_rx.dest_addr, transfer_rx.size / NO_OS_DIV_ROUND_UP(
		       talInit.jesd204Settings.framerA.Np, 8),
	       num_chans,
	       8 * NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#endif

#ifdef IIO_SUPPORT
	// Allow time to display messages correctly
	no_os_mdelay(100);
#ifdef ADRV9008_2
	status = start_iiod(rx_os_dmac, tx_dmac, rx_os_adc, tx_dac);
#else
	status = start_iiod(rx_dmac, tx_dmac, rx_adc, tx_dac);
#endif
	if (status)
		printf("iiod error: %d\n", status);
#endif // IIO_SUPPORT

	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		talise_shutdown(&tal[t]);
	}
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
