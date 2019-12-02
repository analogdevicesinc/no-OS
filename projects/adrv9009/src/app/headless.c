/**
 * \file headless.c
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
#include "spi.h"
#include "spi_extra.h"
#include "gpio_extra.h"
#include "error.h"
#include "delay.h"
#include "parameters.h"
#include "util.h"
#include "axi_dac_core.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#ifndef ALTERA_PLATFORM
#include "xil_cache.h"
#endif
#include "talise.h"
#include "talise_config_ad9528.h"
#include "talise_config.h"
#include "app_config.h"
#include "app_clocking.h"
#include "app_jesd.h"
#include "app_transceiver.h"
#include "app_talise.h"

/**********************************************************/
/**********************************************************/
/********** Talise Data Structure Initializations ********/
/**********************************************************/
/**********************************************************/

int main(void)
{
	uint32_t rx_lane_rate_khz = talInit.rx.rxProfile.rxOutputRate_kHz *
				    talInit.jesd204Settings.framerA.M * (20 /
						    hweight8(talInit.jesd204Settings.framerA.serializerLanesEnabled));
	uint32_t rx_div40_rate_hz = rx_lane_rate_khz * (1000 / 40);
	uint32_t tx_lane_rate_khz = talInit.tx.txProfile.txInputRate_kHz *
				    talInit.jesd204Settings.deframerA.M * (20 /
						    hweight8(talInit.jesd204Settings.deframerA.deserializerLanesEnabled));
	uint32_t tx_div40_rate_hz = tx_lane_rate_khz * (1000 / 40);
	uint32_t rx_os_lane_rate_khz = talInit.obsRx.orxProfile.orxOutputRate_kHz *
				       talInit.jesd204Settings.framerB.M * (20 /
						       hweight8(talInit.jesd204Settings.framerB.serializerLanesEnabled));
	uint32_t rx_os_div40_rate_hz = rx_os_lane_rate_khz * (1000 / 40);

	struct axi_adc_init rx_adc_init = {
		"rx_adc",
		RX_CORE_BASEADDR,
		4
	};
	struct axi_adc *rx_adc;
	struct axi_dac_init tx_dac_init = {
		"tx_dac",
		TX_CORE_BASEADDR,
		4
	};
	struct axi_dac *tx_dac;
	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		DMA_DEV_TO_MEM,
		0
	};
	struct axi_dmac *rx_dmac;
#ifdef DAC_DMA_EXAMPLE
	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		DMA_MEM_TO_DEV,
		DMA_CYCLIC,
	};
	struct axi_dmac *tx_dmac;
	struct gpio_desc *gpio_plddrbypass;
	struct gpio_init_param gpio_init_plddrbypass;
	extern const uint32_t sine_lut_iq[1024];
#endif
	struct adi_hal hal;
#ifndef ALTERA_PLATFORM
	xil_spi_init_param hal_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.device_id = 0,
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
	hal.extra_gpio= &hal_gpio_param;
#else
	struct altera_spi_init_param hal_spi_param = {
		.type = NIOS_II_SPI,
		.device_id = 0,
		.base_address = SPI_BASEADDR
	};
	struct altera_gpio_init_param hal_gpio_param = {
		.type = NIOS_II_GPIO,
		.device_id = 0,
		.base_address = GPIO_BASEADDR
	};

	hal.extra_gpio = &hal_gpio_param;
#endif
	hal.extra_spi = &hal_spi_param;
	taliseDevice_t talDev = {
		.devHalInfo = &hal,
		.devStateInfo = {0}
	};

	printf("Hello\n");

	/**********************************************************/
	/**********************************************************/
	/************ Talise Initialization Sequence *************/
	/**********************************************************/
	/**********************************************************/

	clocking_init(rx_div40_rate_hz, tx_div40_rate_hz, rx_os_div40_rate_hz);

	/*** < Insert User BBIC JESD204B Initialization Code Here > ***/
	jesd_init(rx_div40_rate_hz,
		  tx_div40_rate_hz,
		  rx_os_div40_rate_hz);

	fpga_xcvr_init(rx_lane_rate_khz,
		       tx_lane_rate_khz,
		       rx_os_lane_rate_khz,
		       talInit.clocks.deviceClock_kHz);

	talise_setup(&talDev, &talInit);

	ADIHAL_sysrefReq(talDev.devHalInfo, SYSREF_CONT_ON);

	jesd_rx_watchdog();

	/* Print JESD status */
	jesd_status();

	/* Initialize the DAC DDS */
	axi_dac_init(&tx_dac, &tx_dac_init);

	/* Initialize the ADC core */
	axi_adc_init(&rx_adc, &rx_adc_init);

#ifdef DAC_DMA_EXAMPLE
	gpio_init_plddrbypass.extra = &ad9528_gpio_param;
	gpio_init_plddrbypass.number = DAC_FIFO_BYPASS;
	gpio_get(&gpio_plddrbypass, gpio_init_plddrbypass);
	gpio_direction_output(gpio_plddrbypass, 1);
	axi_dac_load_custom_data(tx_dac, sine_lut_iq,
				 ARRAY_SIZE(sine_lut_iq),
				 DDR_MEM_BASEADDR + 0xA000000);
#ifndef ALTERA_PLATFORM
	Xil_DCacheFlush();
#endif
	axi_dmac_init(&tx_dmac, &tx_dmac_init);
	axi_dmac_transfer(tx_dmac, DDR_MEM_BASEADDR + 0xA000000,
			  sizeof(sine_lut_iq) * 2);
#endif

	mdelay(1000);

	/* Initialize the DMAC and transfer 16384 samples from ADC to MEM */
	axi_dmac_init(&rx_dmac, &rx_dmac_init);
	axi_dmac_transfer(rx_dmac,
			  DDR_MEM_BASEADDR + 0x800000,
			  16384 * 8);
#ifndef ALTERA_PLATFORM
	Xil_DCacheInvalidateRange(XPAR_DDR_MEM_BASEADDR + 0x800000, 16384 * 8);
#endif

	talise_shutdown(&talDev);

	fpga_xcvr_deinit();
	jesd_deinit();
	clocking_deinit();
	printf("Bye\n");

	return SUCCESS;
}
