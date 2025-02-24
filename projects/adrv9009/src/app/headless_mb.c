/**
 * \file adrv9009/src/app/headless_mb.c
 *
 * \brief Contains example code for user integration with their application. This file contains code for MicroBlaze.
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 */

/****< Insert User Includes Here >***/

#include <stdio.h>
#include "no_os_print_log.h"
#include "clk_axi_clkgen.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_dac_core.h"
#include "axi_adc_core.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "xilinx_gpio.h"
#include "parameters.h"
#include "no_os_util.h"
#include "xilinx_spi.h"
#include "axi_adxcvr.h"
#include "no_os_spi.h"
#include "xil_cache.h"
#include "axi_dmac.h"
#include "adi_hal.h"

#include "talise.h"
#include "talise_config.h"
#include "app_config.h"
#include "adrv9009.h"
#include "ad9528.h"

uint32_t dac_buffer_dma[1024 * 16] __attribute__((aligned(1024)));

#ifdef IIO_SUPPORT

#include "iio.h"
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "xilinx_uart.h"

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
	if (status < 0)
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

	struct adrv9009_init_param adrv9009_init_par = { 0 };
	struct ad9528_platform_data ad9528_pdata = { 0 };
	struct ad9528_channel_spec ad9528_channels[14];
	taliseAgcCfg_t agcConfig_init_param = { 0 };
	taliseDevice_t adrv9009_device = { 0 };
	struct ad9528_init_param ad9528_param;
	struct adi_hal adrv9009_hal = { 0 };
	struct ad9528_dev* clkchip_device;
	struct adrv9009_rf_phy *phy;

	pr_info("Hello (MB project - JESD FSM)\n");

	// ad9528 defaults
	ad9528_param.pdata = &ad9528_pdata;
	ad9528_param.pdata->num_channels = 14;
	ad9528_param.pdata->channels = &ad9528_channels[0];

	status = ad9528_init(&ad9528_param);
	if (status) {
		pr_info("error: ad9528_init() failed with %d\n", status);
		goto error;
	}

	// ad9528 channel defaults
	for (unsigned int ch = 0; ch < ad9528_param.pdata->num_channels; ch++) {
		ad9528_channels[ch].channel_num = ch;
		ad9528_channels[ch].output_dis = 1;
	}

	// ad9528 channel specifics

	// adrv9009 device clock
	ad9528_channels[13].output_dis = 0;
	ad9528_channels[13].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[13].divider_phase = 0;
	ad9528_channels[13].channel_divider = 5;
	ad9528_channels[13].signal_source = SOURCE_VCO;

	// fpga device clock
	ad9528_channels[1].output_dis = 0;
	ad9528_channels[1].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[1].divider_phase = 0;
	ad9528_channels[1].channel_divider = 5;
	ad9528_channels[1].signal_source = SOURCE_VCO;

	// adrv9009 sysref
	ad9528_channels[12].output_dis = 0;
	ad9528_channels[12].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[12].divider_phase = 0;
	ad9528_channels[12].channel_divider = 5;
	ad9528_channels[12].signal_source = SOURCE_SYSREF_VCO;

	// fpga sysref
	ad9528_channels[3].output_dis = 0;
	ad9528_channels[3].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[3].divider_phase = 0;
	ad9528_channels[3].channel_divider = 5;
	ad9528_channels[3].signal_source = SOURCE_SYSREF_VCO;

	// ad9528 settings
	ad9528_param.pdata->spi3wire = 0;
	ad9528_param.pdata->vcxo_freq = 122880000;
	ad9528_param.pdata->refa_en = 1;
	ad9528_param.pdata->refa_diff_rcv_en = 1;
	ad9528_param.pdata->refa_r_div = 1;
	ad9528_param.pdata->osc_in_cmos_neg_inp_en = 1;
	ad9528_param.pdata->pll1_feedback_div = 4;
	ad9528_param.pdata->pll1_feedback_src_vcxo = 0; /* VCO */
	ad9528_param.pdata->pll1_charge_pump_current_nA = 5000;
	ad9528_param.pdata->pll1_bypass_en = 0;
	ad9528_param.pdata->pll2_vco_div_m1 = 3;
	ad9528_param.pdata->pll2_n2_div = 10;
	ad9528_param.pdata->pll2_r1_div = 1;
	ad9528_param.pdata->pll2_charge_pump_current_nA = 805000;
	ad9528_param.pdata->pll2_bypass_en = false;
	ad9528_param.pdata->sysref_src = SYSREF_SRC_INTERNAL;
	ad9528_param.pdata->sysref_pattern_mode = SYSREF_PATTERN_NSHOT;
	ad9528_param.pdata->sysref_k_div = 512;
	ad9528_param.pdata->sysref_req_en = false;
	ad9528_param.pdata->sysref_nshot_mode = SYSREF_NSHOT_4_PULSES;
	ad9528_param.pdata->sysref_req_trigger_mode = SYSREF_LEVEL_HIGH;
	ad9528_param.pdata->rpole2 = RPOLE2_900_OHM;
	ad9528_param.pdata->rzero = RZERO_1850_OHM;
	ad9528_param.pdata->cpole1 = CPOLE1_16_PF;
	ad9528_param.pdata->stat0_pin_func_sel = 0x2; /* PLL1 & PLL2 Locked */
	ad9528_param.pdata->stat1_pin_func_sel = 0x9; /* REFA Correct */

	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
		.flags = SPI_CS_DECODE
#endif
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

	ad9528_param.spi_init = clkchip_spi_init_param;

	// clock chip GPIO settings
	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID,
	};

	struct no_os_gpio_init_param clkchip_gpio_init_param = {
		.number = CLK_RESETB_GPIO,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param
	};
	ad9528_param.gpio_resetb = &clkchip_gpio_init_param;

	// Export no_os_clk_desc for each channel
	ad9528_param.export_no_os_clk = true;

	status = ad9528_setup(&clkchip_device, ad9528_param);
	if (status) {
		pr_info("error: ad9528_setup() failed with %d\n", status);
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
		.octets_per_frame = 4,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = rx_div40_rate_hz / 1000,
		.lane_clk_khz = rx_lane_rate_khz
	};
	tx_lane_rate_khz = tx_div40_rate_hz / 1000 * 40;
	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = 2,
		.frames_per_multiframe = talInit.jesd204Settings.deframerA.K,
		.converters_per_device = talInit.jesd204Settings.deframerA.M,
		.converter_resolution = 14,
		.bits_per_sample = talInit.jesd204Settings.deframerA.Np,
		.high_density = true,
		.control_bits_per_sample  = 2,
		.subclass = 1,
		.device_clk_khz = tx_div40_rate_hz / 1000,
		.lane_clk_khz = tx_lane_rate_khz
	};
	rx_os_lane_rate_khz = rx_os_div40_rate_hz / 1000 * 40;
	struct jesd204_rx_init rx_os_jesd_init = {
		.name = "rx_os_jesd",
		.base = RX_OS_JESD_BASEADDR,
		.octets_per_frame = 2,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = rx_os_div40_rate_hz / 1000,
		.lane_clk_khz = rx_os_lane_rate_khz,
	};

	struct adxcvr_init rx_adxcvr_init = {
		.name = "rx_adxcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_REFCLK,
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
		.out_clk_sel = ADXCVR_REFCLK,
		.lpm_enable = 0,
		.lane_rate_khz = tx_lane_rate_khz,
		.ref_rate_khz = talInit.clocks.deviceClock_kHz,
		.export_no_os_clk = true
	};
	struct adxcvr *tx_adxcvr;

	struct adxcvr_init rx_os_adxcvr_init = {
		.name = "rx_os_adxcvr",
		.base = RX_OS_XCVR_BASEADDR,
		.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
		.out_clk_sel = ADXCVR_REFCLK,
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

	struct axi_clkgen *rx_clkgen;
	struct axi_clkgen *tx_clkgen;
	struct axi_clkgen *rx_os_clkgen;

	struct axi_clkgen_init rx_clkgen_init = {
		"rx_clkgen",
		RX_CLKGEN_BASEADDR,
		talInit.clocks.deviceClock_kHz * 1000
	};
	struct axi_clkgen_init tx_clkgen_init = {
		"tx_clkgen",
		TX_CLKGEN_BASEADDR,
		talInit.clocks.deviceClock_kHz * 1000
	};
	struct axi_clkgen_init rx_os_clkgen_init = {
		"rx_os_clkgen",
		RX_OS_CLKGEN_BASEADDR,
		talInit.clocks.deviceClock_kHz * 1000
	};

#ifndef ADRV9008_2
	status = axi_clkgen_init(&rx_clkgen, &rx_clkgen_init);
	if (status) {
		printf("error: %s: axi_clkgen_init() failed\n", rx_clkgen_init.name);
		goto error;
	}
#endif
#ifndef ADRV9008_1
	status = axi_clkgen_init(&tx_clkgen, &tx_clkgen_init);
	if (status) {
		printf("error: %s: axi_clkgen_init() failed\n", tx_clkgen_init.name);
		goto error;
	}
	status = axi_clkgen_init(&rx_os_clkgen, &rx_os_clkgen_init);
	if (status) {
		printf("error: %s: axi_clkgen_init() failed\n", rx_os_clkgen_init.name);
		goto error;
	}
#endif

	// Required by CLKGEN, // fpga device clock
	status = ad9528_clk_set_rate(clkchip_device, 1,
				     talInit.clocks.deviceClock_kHz * 1000);
	if (status != 0) {
		printf("error: ad9528_clk_set_rate() failed\n");
		goto error;
	}

	// adrv9009 sysref
	status = ad9528_clk_set_rate(clkchip_device, 12, 60000);
	if (status != 0) {
		printf("error: ad9528_clk_set_rate() failed\n");
		goto error;
	}
	// fpga sysref
	status = ad9528_clk_set_rate(clkchip_device, 3, 60000);
	if (status != 0) {
		printf("error: ad9528_clk_set_rate() failed\n");
		goto error;
	}
	// adrv9009 device clock
	status = ad9528_clk_set_rate(clkchip_device, 13,
				     talInit.clocks.deviceClock_kHz * 1000);
	if (status != 0) {
		printf("error: ad9528_clk_set_rate() failed\n");
		goto error;
	}

#ifndef ADRV9008_2
	status = axi_clkgen_set_rate(rx_clkgen, rx_div40_rate_hz);
	if (status != 0) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_clkgen->name);
		goto error;
	}
#endif
#ifndef ADRV9008_1
	status = axi_clkgen_set_rate(tx_clkgen, tx_div40_rate_hz);
	if (status != 0) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", tx_clkgen->name);
		goto error;
	}
	status = axi_clkgen_set_rate(rx_os_clkgen, rx_os_div40_rate_hz);
	if (status != 0) {
		printf("error: %s: axi_clkgen_set_rate() failed\n", rx_os_clkgen->name);
		goto error;
	}
#endif

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

	adrv9009_hal.extra_gpio = &hal_gpio_param;
	adrv9009_hal.extra_spi = &hal_spi_param;
	adrv9009_hal.gpio_adrv_resetb_num = TRX_A_RESETB_GPIO;
	adrv9009_hal.spi_adrv_csn = ADRV_CS;

	adrv9009_device.devHalInfo = (void *)&adrv9009_hal;

	adrv9009_init_par.adrv9009_device = &adrv9009_device;
	adrv9009_init_par.dev_clk = clkchip_device->clk_desc[13];
	adrv9009_init_par.streamImageFile = STREAM;
	adrv9009_init_par.armImageFile = FIRMWARE;
	adrv9009_init_par.rxAgcConfig_init_param = &agcConfig_init_param;

	status = adrv9009_init(&phy, &adrv9009_init_par);
	if (status) {
		pr_err("error: adrv9009_init() failed\n");
		goto error;
	}

	status = axi_dac_init_begin(&phy->tx_dac, &tx_dac_init);
	if (status)
		goto error;
	status = axi_adc_init_begin(&phy->rx_adc, &rx_adc_init);
	if (status)
		goto error;
	status = axi_adc_init_begin(&phy->rx_os_adc, &rx_os_adc_init);
	if (status)
		goto error;

	// Reset Tx DAC
	axi_adc_write(phy->rx_adc, 0x4040, 0);
	axi_adc_write(phy->rx_adc, 0x4040,
		      NO_OS_BIT(1) | NO_OS_BIT(0));

	// Reset Rx ADC
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

	// Reset Rx OS ADC
	axi_adc_write(phy->rx_adc, 0x8040, 0);
	axi_adc_write(phy->rx_adc, 0x8040,
		      NO_OS_BIT(1) | NO_OS_BIT(0));

	status = adrv9009_post_setup(phy);
	if (status) {
		pr_err("error: adrv9009_post_setup() failed\n");
		goto error;
	}

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

#ifdef PLATFORM_MB
	/* Required for using SPI desc*/
	status = TALISE_openHw(phy->talDevice);
	if (status) {
		/*** < User: decides what to do based on Talise recovery action returned > ***/
		pr_info("error: TALISE_openHw() failed\n");
		goto error;
	}
#endif

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
			.jdev = phy->jdev,
			.link_ids = {DEFRAMER_LINK_TX, FRAMER_LINK_RX, FRAMER_LINK_ORX},
			.links_number = 3,
			.is_top_device = true,
		},
	};

	jesd204_topology_init(&topology, devs,
			      sizeof(devs) / sizeof(*devs));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);

	printf("\n");

	// Set DDS data
	axi_dac_data_setup(phy->tx_dac);

	axi_jesd204_rx_status_read(rx_jesd);
	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_os_jesd);

	// axi_dac_set_datasel(phy->tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
	extern const uint32_t sine_lut_iq[1024];
	axi_dac_set_datasel(phy->tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	axi_dac_load_custom_data(phy->tx_dac, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq), (uintptr_t)dac_buffer_dma);

	struct axi_dma_transfer transfer = {
		// Number of bytes to write/read
		.size = sizeof(sine_lut_iq) * phy->tx_dac->num_channels / 2,
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
				  sizeof(sine_lut_iq) * phy->tx_dac->num_channels / 2);

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

	no_os_mdelay(1000);

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

	printf("DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%u\n",
	       transfer_rx.dest_addr, transfer_rx.size / NO_OS_DIV_ROUND_UP(
		       talInit.jesd204Settings.framerA.Np, 8),
	       num_chans,
	       8 * NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));

#ifdef IIO_SUPPORT
	// Allow time to display messages correctly
	no_os_mdelay(100);
	status = start_iiod(rx_dmac, tx_dmac, phy->rx_adc, phy->tx_dac);
	if (status)
		printf("iiod error: %d\n", status);
#endif // IIO_SUPPORT

error:
	pr_info("Bye\n");

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return 0;
}
