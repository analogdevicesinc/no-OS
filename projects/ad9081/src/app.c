/***************************************************************************//**
 *   @file   app.c
 *   @brief  AD9081 application example.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <inttypes.h>
#include "error.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "spi.h"
#include "spi_extra.h"
#include "ad9081.h"
#include "app_clock.h"
#include "app_jesd.h"
#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "app_parameters.h"
#include "app_config.h"

#ifdef IIO_SUPPORT
#include "app_iio.h"
#endif

extern struct axi_jesd204_rx *rx_jesd;
extern struct axi_jesd204_tx *tx_jesd;

int main(void)
{
	struct clk app_clk[MULTIDEVICE_INSTANCE_COUNT];
	struct clk jesd_clk[2];
	struct xil_gpio_init_param  xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	struct gpio_init_param	gpio_phy_resetb = {
		.number = PHY_RESET,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
	};
	struct spi_init_param phy_spi_init_param = {
		.device_id = PHY_SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.mode = SPI_MODE_0,
		.chip_select = PHY_CS,
		.platform_ops = &xil_platform_ops,
		.extra = &xil_spi_param
	};
	struct link_init_param jesd_tx_link = {
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
		.tpl_phase_adjust = 12
	};
	struct link_init_param jesd_rx_link = {
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
		.multidevice_instance_count = 1,
#ifdef QUAD_MXFE
		.jesd_sync_pins_01_swap_enable = true,
#else
		.jesd_sync_pins_01_swap_enable = false,
#endif
		.lmfc_delay_dac_clk_cycles = 0,
		.nco_sync_ms_extra_lmfc_num = 0,
		/* TX */
		.dac_frequency_hz = AD9081_DAC_FREQUENCY,
		/* The 4 DAC Main Datapaths */
		.tx_main_interpolation = AD9081_TX_MAIN_INTERPOLATION,
		.tx_main_nco_frequency_shift_hz = AD9081_TX_MAIN_NCO_SHIFT,
		.tx_dac_channel_crossbar_select = AD9081_TX_DAC_CHAN_CROSSBAR,
		/* The 8 DAC Channelizers */
		.tx_channel_interpolation = AD9081_TX_CHAN_INTERPOLATION,
		.tx_channel_nco_frequency_shift_hz = AD9081_TX_CHAN_NCO_SHIFT,
		.tx_channel_gain = AD9081_TX_CHAN_GAIN,
		.jesd_tx_link = &jesd_tx_link,
		/* RX */
		.adc_frequency_hz = AD9081_ADC_FREQUENCY,
		.nyquist_zone = AD9081_ADC_NYQUIST_ZONE,
		/* The 4 ADC Main Datapaths */
		.rx_main_nco_frequency_shift_hz = AD9081_RX_MAIN_NCO_SHIFT,
		.rx_main_decimation = AD9081_RX_MAIN_DECIMATION,
		.rx_main_complex_to_real_enable = {0, 0, 0, 0},
		.rx_main_enable = AD9081_RX_MAIN_ENABLE,
		/* The 8 ADC Channelizers */
		.rx_channel_nco_frequency_shift_hz = AD9081_RX_CHAN_NCO_SHIFT,
		.rx_channel_decimation = AD9081_RX_CHAN_DECIMATION,
		.rx_channel_complex_to_real_enable = {0, 0, 0, 0, 0, 0, 0, 0},
		.rx_channel_enable = AD9081_RX_CHAN_ENABLE,
		.jesd_rx_link[0] = &jesd_rx_link,
		.jesd_rx_link[1] = NULL,
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
		DMA_DEV_TO_MEM,
		0
	};
	struct axi_dmac *rx_dmac;
	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		DMA_MEM_TO_DEV,
		DMA_CYCLIC
	};
	struct axi_dmac *tx_dmac;
	struct ad9081_phy* phy[MULTIDEVICE_INSTANCE_COUNT];
	int32_t status;
	int32_t i;

	printf("Hello\n");

#ifdef QUAD_MXFE
	struct xil_gpio_init_param  xil_gpio_param_2 = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_2_DEVICE_ID
	};
	struct gpio_init_param	ad9081_gpio0_mux_init = {
		.number = AD9081_GPIO_0_MUX,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param_2
	};
	gpio_desc *ad9081_gpio0_mux;

	status = gpio_get(&ad9081_gpio0_mux, &ad9081_gpio0_mux_init);
	if (status)
		return status;

	status = gpio_set_value(ad9081_gpio0_mux, 1);
	if (status)
		return status;
#endif

	status = app_clock_init(app_clk);
	if (status != SUCCESS)
		printf("app_clock_init() error: %" PRId32 "\n", status);

	status = app_jesd_init(jesd_clk,
			       500000, 250000, 250000, 10000000, 10000000);
	if (status != SUCCESS)
		printf("app_jesd_init() error: %" PRId32 "\n", status);

	rx_adc_init.num_channels = 0;
	tx_dac_init.num_channels = 0;

	for (i = 0; i < MULTIDEVICE_INSTANCE_COUNT; i++) {
		gpio_phy_resetb.number = PHY_RESET + i;
		phy_spi_init_param.chip_select = PHY_CS + i;
		phy_param.dev_clk = &app_clk[i];
		jesd_rx_link.device_id = i;

		status = ad9081_init(&phy[i], &phy_param);
		if (status != SUCCESS)
			printf("ad9081_init() error: %" PRId32 "\n", status);

		rx_adc_init.num_channels += phy[i]->jesd_rx_link[0].jesd_param.jesd_m +
					    phy[i]->jesd_rx_link[1].jesd_param.jesd_m;

		tx_dac_init.num_channels += phy[i]->jesd_tx_link.jesd_param.jesd_m *
					    (phy[i]->jesd_tx_link.jesd_param.jesd_duallink > 0 ? 2 : 1);
	}

	axi_jesd204_rx_watchdog(rx_jesd);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	axi_dac_init(&tx_dac, &tx_dac_init);
	axi_adc_init(&rx_adc, &rx_adc_init);

	axi_dmac_init(&tx_dmac, &tx_dmac_init);
	axi_dmac_init(&rx_dmac, &rx_dmac_init);

#ifdef IIO_SUPPORT
	printf("The board accepts libiio clients connections through the serial backend.\n");

	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
	};

	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
	};

	return iio_server_init(&iio_axi_adc_init_par, &iio_axi_dac_init_par);
#else
	printf("Bye\n");

	return SUCCESS;
#endif

}
