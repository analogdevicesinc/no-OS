/**
 * \file headless.c
 *
 * \brief Contains example code for user integration with their application
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 */

#include "talise.h"
#include "talise_jesd204.h"
#include "talise_arm.h"
#include "talise_radioctrl.h"
#include "talise_cals.h"

#include "talise_config.h"
#include "talise_error.h"
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
#include "ad9528.h"
#ifdef ALTERA_PLATFORM
#include "clk_altera_a10_fpll.h"
#include "altera_adxcvr.h"
#else
#include "xil_cache.h"
#include "clk_axi_clkgen.h"
#include "axi_adxcvr.h"
#endif
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_dac_core.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"

#include "talise_config_ad9528.h"
#include "talise_arm_binary.h"
#include "talise_stream_binary.h"
#include "app_config.h"
#include "app_sysref_req.h"

/**********************************************************/
/**********************************************************/
/********** Talise Data Structure Initializations ********/
/**********************************************************/
/**********************************************************/

int main(void)
{
	adiHalErr_t ad9528Error;
	struct gpio_desc *gpio_sysref_req;

	ad9528Device_t clockAD9528_ = {
		NULL,
		NULL,
		&clockSpiSettings,
		&clockPll1Settings,
		&clockPll2Settings,
		&clockOutputSettings,
		&clockSysrefSettings,
		NULL,
		NULL
	};
	ad9528Device_t *clockAD9528_device = &clockAD9528_;
#ifdef ALTERA_PLATFORM
	struct altera_spi_init_param ad9528_spi_param = {
		.type = NIOS_II_SPI,
		.device_id = 0,
		.base_address = SPI_BASEADDR
	};
	clockAD9528_.extra_spi = &ad9528_spi_param;
	struct altera_gpio_init_param ad9528_gpio_param = {
		.type = NIOS_II_GPIO,
		.device_id = 0,
		.base_address = GPIO_BASEADDR
	};
	clockAD9528_.extra_gpio = &ad9528_gpio_param;
	struct altera_a10_fpll_init rx_device_clk_pll_init = {
		"rx_device_clk_pll",
		RX_A10_FPLL_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct altera_a10_fpll_init tx_device_clk_pll_init = {
		"tx_device_clk_pll",
		TX_A10_FPLL_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct altera_a10_fpll_init rx_os_device_clk_pll_init = {
		"rx_os_device_clk_pll",
		RX_OS_A10_FPLL_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct altera_a10_fpll *rx_device_clk_pll;
	struct altera_a10_fpll *tx_device_clk_pll;
	struct altera_a10_fpll *rx_os_device_clk_pll;
#else
	struct xil_spi_init_param ad9528_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.device_id = 0
	};
	clockAD9528_.extra_spi = &ad9528_spi_param;
	struct xil_gpio_init_param ad9528_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID,
	};
	clockAD9528_.extra_gpio = &ad9528_gpio_param;
	struct axi_clkgen_init rx_clkgen_init = {
		"rx_clkgen",
		RX_CLKGEN_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct axi_clkgen_init tx_clkgen_init = {
		"tx_clkgen",
		TX_CLKGEN_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct axi_clkgen_init rx_os_clkgen_init = {
		"rx_os_clkgen",
		RX_OS_CLKGEN_BASEADDR,
		clockAD9528_device->outputSettings->outFrequency_Hz[1]
	};
	struct axi_clkgen *rx_clkgen;
	struct axi_clkgen *tx_clkgen;
	struct axi_clkgen *rx_os_clkgen;
#endif
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
	struct jesd204_rx_init rx_jesd_init = {
		"rx_jesd",
		RX_JESD_BASEADDR,
		4,
		32,
		1,
		rx_div40_rate_hz / 1000,
		rx_lane_rate_khz
	};
	struct jesd204_tx_init tx_jesd_init = {
		"tx_jesd",
		TX_JESD_BASEADDR,
		2,
		32,
		4,
		14,
		16,
		true,
		2,
		1,
		tx_div40_rate_hz / 1000,
		tx_lane_rate_khz
	};

	struct jesd204_rx_init rx_os_jesd_init = {
		"rx_os_jesd",
		RX_OS_JESD_BASEADDR,
		2,
		32,
		1,
		rx_os_div40_rate_hz / 1000,
		rx_os_lane_rate_khz,
	};
	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;
	struct axi_jesd204_rx *rx_os_jesd;
#ifdef ALTERA_PLATFORM
	struct adxcvr_init rx_adxcvr_init = {
		"rx_adxcvr",
		RX_XCVR_BASEADDR,
		{RX_ADXCFG_0_BASEADDR, RX_ADXCFG_1_BASEADDR, 0, 0},
		0,
		rx_lane_rate_khz,
		talInit.clocks.deviceClock_kHz,
	};
	struct adxcvr_init tx_adxcvr_init = {
		"tx_adxcvr",
		TX_XCVR_BASEADDR,
		{TX_ADXCFG_0_BASEADDR, TX_ADXCFG_1_BASEADDR, TX_ADXCFG_2_BASEADDR, TX_ADXCFG_3_BASEADDR},
		TX_PLL_BASEADDR,
		tx_lane_rate_khz,
		talInit.clocks.deviceClock_kHz,
	};
	struct adxcvr_init rx_os_adxcvr_init = {
		"rx_os_adxcvr",
		RX_OS_XCVR_BASEADDR,
		{RX_OS_ADXCFG_0_BASEADDR, RX_OS_ADXCFG_1_BASEADDR, 0, 0},
		0,
		rx_os_lane_rate_khz,
		talInit.clocks.deviceClock_kHz,
	};
#else
	struct adxcvr_init rx_adxcvr_init = {
		"rx_adxcvr",
		RX_XCVR_BASEADDR,
		0,
		3,
		1,
		1,
		rx_lane_rate_khz,
		talInit.clocks.deviceClock_kHz,
	};
	struct adxcvr_init tx_adxcvr_init = {
		"tx_adxcvr",
		TX_XCVR_BASEADDR,
		3,
		3,
		0,
		0,
		tx_lane_rate_khz,
		talInit.clocks.deviceClock_kHz,
	};
	struct adxcvr_init rx_os_adxcvr_init = {
		"rx_os_adxcvr",
		RX_OS_XCVR_BASEADDR,
		0,
		3,
		1,
		1,
		rx_os_lane_rate_khz,
		talInit.clocks.deviceClock_kHz,
	};
#endif
	struct adxcvr *rx_adxcvr;
	struct adxcvr *tx_adxcvr;
	struct adxcvr *rx_os_adxcvr;
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
	uint32_t talAction = TALACT_NO_ACTION;
	uint8_t errorFlag = 0;
	uint8_t mcsStatus = 0;
	uint8_t pllLockStatus = 0;
	uint16_t deframerStatus = 0;
	uint8_t framerStatus = 0;
	uint32_t count = sizeof(armBinary);
	taliseArmVersionInfo_t talArmVersionInfo;
	uint32_t initCalMask =  TAL_TX_BB_FILTER | TAL_ADC_TUNER | TAL_TIA_3DB_CORNER
				| TAL_DC_OFFSET | TAL_TX_ATTENUATION_DELAY | TAL_RX_GAIN_DELAY | TAL_FLASH_CAL |
				TAL_PATH_DELAY | TAL_TX_LO_LEAKAGE_INTERNAL | TAL_TX_QEC_INIT |
				TAL_LOOPBACK_RX_LO_DELAY | TAL_LOOPBACK_RX_RX_QEC_INIT |
				TAL_RX_LO_DELAY | TAL_RX_QEC_INIT;
	uint32_t trackingCalMask =  TAL_TRACK_RX1_QEC |
				    TAL_TRACK_RX2_QEC |
				    TAL_TRACK_TX1_QEC |
				    TAL_TRACK_TX2_QEC;

	uint32_t api_vers[4];
	uint8_t rev;
	int32_t status;

	printf("Hello\n");

	/**********************************************************/
	/**********************************************************/
	/************ Talise Initialization Sequence *************/
	/**********************************************************/
	/**********************************************************/

	/** < Insert User System Clock(s) Initialization Code Here >
	 * System Clock should provide a device clock and SYSREF signal
	 * to the Talise device.
	 **/

	if(SUCCESS != gpio_get(&gpio_sysref_req, ADRV_SYSREF_REQ)) {
		printf("error: gpio_get() failed for pin %d\n", ADRV_SYSREF_REQ);
		goto error_0;
	}

	/* Init the AD9528 data structure */
	ad9528Error = AD9528_initDeviceDataStruct(clockAD9528_device,
			clockAD9528_device->pll1Settings->vcxo_Frequency_Hz,
			clockAD9528_device->pll1Settings->refA_Frequency_Hz,
			clockAD9528_device->outputSettings->outFrequency_Hz[1]);
	if (ad9528Error != ADIHAL_OK) {
		printf("error: AD9528_initDeviceDataStruct() failed\n");
		goto error_0;
	}

	/* Perform a hard reset on the AD9528 DUT */
	ad9528Error = AD9528_resetDevice(clockAD9528_device);
	if (ad9528Error != ADIHAL_OK) {
		printf("error: AD9528_resetDevice() failed\n");
		goto error_1;
	}

	/* Initialize the AD9528 by writing all SPI registers */
	ad9528Error = AD9528_initialize(clockAD9528_device);
	if (ad9528Error == ADIHAL_ERR) {
		printf("error: AD9528_initialize() failed\n");
		goto error_1;
	}

	if (ad9528Error == ADIHAL_WARNING)
		printf("warning: AD9528_initialize() issues. "
		       "Possible cause: REF_CLK not connected\n");

#ifdef ALTERA_PLATFORM
	/* Initialize A10 FPLLs */
	status = altera_a10_fpll_init(&rx_device_clk_pll,
				      &rx_device_clk_pll_init);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       rx_os_device_clk_pll_init.name);
		goto error_1;
	}
	status = altera_a10_fpll_init(&tx_device_clk_pll,
				      &tx_device_clk_pll_init);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       rx_os_device_clk_pll_init.name);
		goto error_2;
	}
	status = altera_a10_fpll_init(&rx_os_device_clk_pll,
				      &rx_os_device_clk_pll_init);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       rx_os_device_clk_pll_init.name);
		goto error_3;
	}

	altera_a10_fpll_disable(rx_device_clk_pll);
	status = altera_a10_fpll_set_rate(rx_device_clk_pll,
					  rx_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       rx_device_clk_pll->name);
		goto error_4;
	}
	altera_a10_fpll_enable(rx_device_clk_pll);
	altera_a10_fpll_disable(tx_device_clk_pll);
	status = altera_a10_fpll_set_rate(tx_device_clk_pll,
					  tx_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       tx_device_clk_pll->name);
		goto error_4;
	}
	altera_a10_fpll_enable(tx_device_clk_pll);
	altera_a10_fpll_disable(rx_os_device_clk_pll);
	status = altera_a10_fpll_set_rate(rx_os_device_clk_pll,
					  rx_os_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       rx_os_device_clk_pll->name);
		goto error_4;
	}
	altera_a10_fpll_enable(rx_os_device_clk_pll);
#else
	/* Initialize CLKGEN */
	status = axi_clkgen_init(&rx_clkgen, &rx_clkgen_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_init() failed\n", rx_clkgen_init.name);
		goto error_1;
	}
	status = axi_clkgen_init(&tx_clkgen, &tx_clkgen_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_init() failed\n", tx_clkgen_init.name);
		goto error_2;
	}
	status = axi_clkgen_init(&rx_os_clkgen, &rx_os_clkgen_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_os_clkgen_init.name);
		goto error_3;
	}

	status = axi_clkgen_set_rate(rx_clkgen, rx_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_clkgen->name);
		goto error_4;
	}
	status = axi_clkgen_set_rate(tx_clkgen, tx_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", tx_clkgen->name);
		goto error_4;
	}
	status = axi_clkgen_set_rate(rx_os_clkgen, rx_os_div40_rate_hz);
	if (status != SUCCESS) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_os_clkgen->name);
		goto error_4;
	}
#endif

	/*** < Insert User BBIC JESD204B Initialization Code Here > ***/

	/* Initialize JESD */
	status = axi_jesd204_rx_init(&rx_jesd, &rx_jesd_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_rx_init() failed\n", rx_jesd_init.name);
		goto error_4;
	}
	status = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_tx_init() failed\n", tx_jesd_init.name);
		goto error_5;
	}
	status = axi_jesd204_rx_init(&rx_os_jesd, &rx_os_jesd_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_rx_init() failed\n", rx_os_jesd_init.name);
		goto error_6;
	}

	/* Initialize ADXCR */
	status = adxcvr_init(&rx_adxcvr, &rx_adxcvr_init);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_init() failed\n", rx_adxcvr_init.name);
		goto error_7;
	}
	status = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_init() failed\n", tx_adxcvr_init.name);
		goto error_8;
	}
	status = adxcvr_init(&rx_os_adxcvr, &rx_os_adxcvr_init);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_init() failed\n", rx_os_adxcvr_init.name);
		goto error_9;
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(rx_adxcvr);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_clk_enable() failed\n", rx_adxcvr->name);
		goto error_10;
	}
	status = adxcvr_clk_enable(tx_adxcvr);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_clk_enable() failed\n", tx_adxcvr->name);
		goto error_10;
	}
	status = adxcvr_clk_enable(rx_os_adxcvr);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_clk_enable() failed\n", rx_os_adxcvr->name);
		goto error_10;
	}
#endif
	/*******************************/
	/**** Talise Initialization ***/
	/*******************************/

	/*Open Talise Hw Device*/
	talAction = TALISE_openHw(&talDev);
	if(talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_openHw() failed\n");
		goto error_10;
	}

	/* Toggle RESETB pin on Talise device */
	talAction = TALISE_resetDevice(&talDev);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_resetDevice() failed\n");
		goto error_11;
	}

	mdelay(100);

	axi_jesd204_tx_lane_clk_enable(tx_jesd);

	/* TALISE_initialize() loads the Talise device data structure
	 * settings for the Rx/Tx/ORx profiles, FIR filters, digital
	 * filter enables, calibrates the CLKPLL, loads the user provided Rx
	 * gain tables, and configures the JESD204b serializers/framers/deserializers
	 * and deframers.
	 */
	talAction = TALISE_initialize(&talDev, &talInit);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_initialize() failed\n");
		goto error_11;
	}

	/*******************************/
	/***** CLKPLL Status Check *****/
	/*******************************/
	talAction = TALISE_getPllsLockStatus(&talDev, &pllLockStatus);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_getPllsLockStatus() failed\n");
		goto error_11;
	}

	/* Assert that Talise CLKPLL is locked */
	if ((pllLockStatus & 0x01) == 0) {
		/* <user code - CLKPLL not locked - ensure lock before proceeding */
		printf("error: CLKPLL not locked\n");
		goto error_11;
	}

	/*******************************************************/
	/**** Perform MultiChip Sync (MCS) on Talise Device ***/
	/*******************************************************/
	talAction = TALISE_enableMultichipSync(&talDev, 1, &mcsStatus);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_enableMultichipSync() failed\n");
		goto error_11;
	}

	/*< user code - Request minimum 3 SYSREF pulses from Clock Device - > */
	sysrefReq(gpio_sysref_req, SYSREF_PULSE);

	/*******************/
	/**** Verify MCS ***/
	/*******************/
	talAction = TALISE_enableMultichipSync(&talDev, 0, &mcsStatus);
	if ((mcsStatus & 0x0B) != 0x0B) {
		/*< user code - MCS failed - ensure MCS before proceeding*/
		printf("warning: TALISE_enableMultichipSync() failed\n");
	}

	/*******************************************************/
	/**** Prepare Talise Arm binary and Load Arm and	****/
	/**** Stream processor Binaryes 					****/
	/*******************************************************/
	if (pllLockStatus & 0x01) {
		talAction = TALISE_initArm(&talDev, &talInit);
		if (talAction != TALACT_NO_ACTION) {
			/*** < User: decide what to do based on Talise recovery action returned > ***/
			printf("error: TALISE_initArm() failed\n");
			goto error_11;
		}

		/*< user code- load Talise stream binary into streamBinary[4096] >*/
		/*< user code- load ARM binary byte array into armBinary[114688] >*/

		talAction = TALISE_loadStreamFromBinary(&talDev, &streamBinary[0]);
		if (talAction != TALACT_NO_ACTION) {
			/*** < User: decide what to do based on Talise recovery action returned > ***/
			printf("error: TALISE_loadStreamFromBinary() failed\n");
			goto error_11;
		}

		talAction = TALISE_loadArmFromBinary(&talDev, &armBinary[0], count);
		if (talAction != TALACT_NO_ACTION) {
			/*** < User: decide what to do based on Talise recovery action returned > ***/
			printf("error: TALISE_loadArmFromBinary() failed\n");
			goto error_11;
		}

		/* TALISE_verifyArmChecksum() will timeout after 200ms
		 * if ARM checksum is not computed
		 */
		talAction = TALISE_verifyArmChecksum(&talDev);
		if (talAction != TAL_ERR_OK) {
			/*< user code- ARM did not load properly - check armBinary & clock/profile settings >*/
			printf("error: TALISE_verifyArmChecksum() failed\n");
			goto error_11;
		}

	} else {
		/*< user code- check settings for proper CLKPLL lock  > ***/
		printf("error: CLKPLL not locked\n");
		goto error_11;
	}

	TALISE_getDeviceRev(&talDev, &rev);
	TALISE_getArmVersion_v2(&talDev, &talArmVersionInfo);
	TALISE_getApiVersion(&talDev,
			     &api_vers[0], &api_vers[1], &api_vers[2], &api_vers[3]);

	printf("talise: Device Revision %d, Firmware %u.%u.%u, API %u.%u.%u.%u\n",
	       rev, talArmVersionInfo.majorVer,
	       talArmVersionInfo.minorVer, talArmVersionInfo.rcVer,
	       api_vers[0], api_vers[1], api_vers[2], api_vers[3]);

	/*******************************/
	/**Set RF PLL LO Frequencies ***/
	/*******************************/
	talAction = TALISE_setRfPllFrequency(&talDev, TAL_RF_PLL, 2000000000);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_setRfPllFrequency() failed\n");
		goto error_11;
	}

	/*** < wait 200ms for PLLs to lock - user code here > ***/

	talAction = TALISE_getPllsLockStatus(&talDev, &pllLockStatus);
	if ((pllLockStatus & 0x07) != 0x07) {
		/*< user code - ensure lock of all PLLs before proceeding>*/
		printf("error: RFPLL not locked\n");
		goto error_11;
	}

	/****************************************************/
	/**** Run Talise ARM Initialization Calibrations ***/
	/****************************************************/
	talAction = TALISE_runInitCals(&talDev, initCalMask);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_runInitCals() failed\n");
		goto error_11;
	}

	talAction = TALISE_waitInitCals(&talDev, 20000, &errorFlag);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_waitInitCals() failed\n");
		goto error_11;
	}

	if (errorFlag) {
		/*< user code - Check error flag to determine ARM  error> */
		printf("error: Calibrations not completed\n");
		goto error_11;
	} else {
		/*< user code - Calibrations completed successfully > */
		printf("talise: Calibrations completed successfully\n");
	}

	/***************************************************/
	/**** Enable  Talise JESD204B Framer ***/
	/***************************************************/

	talAction = TALISE_enableFramerLink(&talDev, TAL_FRAMER_A, 0);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_enableFramerLink() failed\n");
		goto error_11;
	}

	talAction |= TALISE_enableFramerLink(&talDev, TAL_FRAMER_A, 1);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_enableFramerLink() failed\n");
		goto error_11;
	}

	/*************************************************/
	/**** Enable SYSREF to Talise JESD204B Framer ***/
	/*************************************************/
	/*** < User: Make sure SYSREF is stopped/disabled > ***/

	talAction = TALISE_enableSysrefToFramer(&talDev, TAL_FRAMER_A, 1);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_enableSysrefToFramer() failed\n");
		goto error_11;
	}

	/***************************************************/
	/**** Enable  Talise JESD204B Deframer ***/
	/***************************************************/

	talAction = TALISE_enableDeframerLink(&talDev, TAL_DEFRAMER_A, 0);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_enableDeframerLink() failed\n");
		goto error_11;
	}

	talAction |= TALISE_enableDeframerLink(&talDev, TAL_DEFRAMER_A, 1);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_enableDeframerLink() failed\n");
		goto error_11;
	}
	/***************************************************/
	/**** Enable SYSREF to Talise JESD204B Deframer ***/
	/***************************************************/
	talAction = TALISE_enableSysrefToDeframer(&talDev, TAL_DEFRAMER_A, 1);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_enableDeframerLink() failed\n");
		goto error_11;
	}

	/*** < User Sends SYSREF Here > ***/

	sysrefReq(gpio_sysref_req, SYSREF_CONT_ON);

	mdelay(100);

	axi_jesd204_rx_lane_clk_enable(rx_jesd);
	axi_jesd204_rx_lane_clk_enable(rx_os_jesd);

	mdelay(100);

	sysrefReq(gpio_sysref_req, SYSREF_CONT_OFF);

	mdelay(100);

	/*** < Insert User JESD204B Sync Verification Code Here > ***/

	/**************************************/
	/**** Check Talise Deframer Status ***/
	/**************************************/
	talAction = TALISE_readDeframerStatus(&talDev, TAL_DEFRAMER_A, &deframerStatus);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_readDeframerStatus() failed\n");
		goto error_11;
	}

	if ((deframerStatus & 0xF7) != 0x86)
		printf("warning: TAL_DEFRAMER_A status 0x%X\n", deframerStatus);

	/************************************/
	/**** Check Talise Framer Status ***/
	/************************************/
	talAction = TALISE_readFramerStatus(&talDev, TAL_FRAMER_A, &framerStatus);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_readFramerStatus() failed\n");
		goto error_11;
	}

	if ((framerStatus & 0x07) != 0x05) {
		printf("warning: TAL_FRAMER_A status 0x%X\n", framerStatus);
	}

	/*** < User: When links have been verified, proceed > ***/

	/***********************************************
	* Allow Rx1/2 QEC tracking and Tx1/2 QEC	   *
	* tracking to run when in the radioOn state	*
	* Tx calibrations will only run if radioOn and *
	* the obsRx path is set to OBS_INTERNAL_CALS   *
	* **********************************************/

	talAction = TALISE_enableTrackingCals(&talDev, trackingCalMask);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_enableTrackingCals() failed\n");
		goto error_11;
	}

	/* Function to turn radio on, Enables transmitters and receivers */
	/* that were setup during TALISE_initialize() */
	talAction = TALISE_radioOn(&talDev);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_radioOn() failed\n");
		goto error_11;
	}

	talAction = TALISE_setRxTxEnable(&talDev, TAL_RX1RX2_EN, TAL_TX1TX2);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
		printf("error: TALISE_setRxTxEnable() failed\n");
		goto error_10;
	}

	sysrefReq(gpio_sysref_req, SYSREF_CONT_ON);

	axi_jesd204_rx_watchdog(rx_jesd);
	axi_jesd204_rx_watchdog(rx_os_jesd);


	/* Print JESD status */
	axi_jesd204_rx_status_read(rx_jesd);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_os_jesd);

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

	/***********************************************
	* Shutdown Procedure *
	* **********************************************/
	/* Function to turn radio on, Disables transmitters and receivers */
	talAction = TALISE_radioOff(&talDev);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}
	/* Put Talise in safe state for power down */
	talAction = TALISE_shutdown(&talDev);
	if (talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}

	/*Close Talise Hw Device*/
	talAction = TALISE_closeHw(&talDev);
	if(talAction != TALACT_NO_ACTION) {
		/*** < User: decide what to do based on Talise recovery action returned > ***/
	}

	adxcvr_remove(rx_os_adxcvr);
	adxcvr_remove(tx_adxcvr);
	adxcvr_remove(rx_adxcvr);
	axi_jesd204_rx_remove(rx_os_jesd);
	axi_jesd204_tx_remove(tx_jesd);
	axi_jesd204_rx_remove(rx_jesd);
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(rx_device_clk_pll);
	altera_a10_fpll_remove(tx_device_clk_pll);
	altera_a10_fpll_remove(rx_os_device_clk_pll);
#else
	axi_clkgen_remove(rx_os_clkgen);
	axi_clkgen_remove(tx_clkgen);
	axi_clkgen_remove(rx_clkgen);
#endif
	AD9528_remove(clockAD9528_device);
	gpio_remove(gpio_sysref_req);


	printf("Bye\n");

	return SUCCESS;

error_11:
	TALISE_closeHw(&talDev);
error_10:
	adxcvr_remove(rx_os_adxcvr);
error_9:
	adxcvr_remove(tx_adxcvr);
error_8:
	adxcvr_remove(rx_adxcvr);
error_7:
	axi_jesd204_rx_remove(rx_os_jesd);
error_6:
	axi_jesd204_tx_remove(tx_jesd);
error_5:
	axi_jesd204_rx_remove(rx_jesd);
error_4:
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(rx_os_device_clk_pll);
#else
	axi_clkgen_remove(rx_os_clkgen);
#endif
error_3:
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(tx_device_clk_pll);
#else
	axi_clkgen_remove(tx_clkgen);
#endif
error_2:
#ifdef ALTERA_PLATFORM
	altera_a10_fpll_remove(rx_device_clk_pll);
#else
	axi_clkgen_remove(rx_clkgen);
#endif
error_1:
	AD9528_remove(clockAD9528_device);
	gpio_remove(gpio_sysref_req);
error_0:
	return FAILURE;
}
