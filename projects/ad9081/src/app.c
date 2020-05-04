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

#ifdef IIO_SUPPORT
#include "app_iio.h"
#endif

extern struct axi_jesd204_rx *rx_jesd;
extern struct axi_jesd204_tx *tx_jesd;

int main(void)
{
	struct clk app_clk[NUM_APP_CLKS];
	struct clk jesd_clk[2];
	struct xil_gpio_init_param  xil_gpio_param = {
		.type = GPIO_PL,
		.device_id = GPIO_DEVICE_ID
	};
	struct gpio_init_param	gpio_phy_resetb = {
		.number = PHY_RESET,
		.extra = &xil_gpio_param
	};
	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PL,
		.device_id = SPI_DEVICE_ID,
	};
	struct spi_init_param phy_spi_init_param = {
		.max_speed_hz = 1000000,
		.mode = SPI_MODE_0,
		.chip_select = PHY_CS,
		.extra = &xil_spi_param
	};
	struct link_init_param jesd_tx_link = {
		.device_id = 0,
		.octets_per_frame = 4,
		.frames_per_multiframe = 32,
		.samples_per_converter_per_frame = 1,
		.high_density = 1,
		.converter_resolution = 16,
		.bits_per_sample = 16,
		.converters_per_device = 8,
		.control_bits_per_sample = 0,
		.lanes_per_device = 4,
		.subclass = 1,
		.link_mode = 9,
		.dual_link = 0,
		.version = 1,
		.logical_lane_mapping = {0, 2, 7, 7, 1, 7, 7, 3},
		.tpl_phase_adjust = 12
	};
	struct link_init_param jesd_rx_link = {
		.device_id = 0,
		.octets_per_frame = 4,
		.frames_per_multiframe = 32,
		.samples_per_converter_per_frame = 1,
		.high_density = 1,
		.converter_resolution = 16,
		.bits_per_sample = 16,
		.converters_per_device = 8,
		.control_bits_per_sample = 0,
		.lanes_per_device = 4,
		.subclass = 1,
		.link_mode = 10,
		.dual_link = 0,
		.version = 1,
		.logical_lane_mapping = {2, 0, 7, 7, 7, 7, 3, 1},
		.link_converter_select = {0, 1, 2, 3, 8, 9, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0},
	};
	struct ad9081_init_param phy_param = {
		.gpio_reset = &gpio_phy_resetb,
		.spi_init = &phy_spi_init_param,
		.dev_clk = &app_clk[DEV_REFCLK],
		.jesd_tx_clk = &jesd_clk[1],
		.jesd_rx_clk = &jesd_clk[0],
		.multidevice_instance_count = 1,
		.jesd_sync_pins_01_swap_enable = false,
		.lmfc_delay_dac_clk_cycles = 0,
		.nco_sync_ms_extra_lmfc_num = 0,
		/* TX */
		.dac_frequency_hz = 12000000000,
		/* The 4 DAC Main Datapaths */
		.tx_main_interpolation = 6,
		.tx_main_nco_frequency_shift_hz = {1000000000, 1100000000, 1200000000, 1300000000},
		.tx_dac_channel_crossbar_select = {0x1, 0x2, 0x4, 0x8},
		/* The 8 DAC Channelizers */
		.tx_channel_interpolation = 8,
		.tx_channel_nco_frequency_shift_hz = {0, 0, 0, 0, 0, 0, 0, 0},
		.tx_channel_gain = {2048, 2048, 2048, 2048, 0, 0, 0, 0},
		.jesd_tx_link = &jesd_tx_link,
		/* RX */
		.adc_frequency_hz = 4000000000,
		.nyquist_zone = 0,
		/* The 4 ADC Main Datapaths */
		.rx_main_nco_frequency_shift_hz = {400000000, -400000000, 100000000, 100000000},
		.rx_main_decimation = {4, 4, 4, 4},
		.rx_main_complex_to_real_enable = {0, 0, 0, 0},
		.rx_main_enable = {1, 1, 1, 1},
		/* The 8 ADC Channelizers */
		.rx_channel_nco_frequency_shift_hz = {0, 0, 0, 0, 0, 0, 0, 0},
		.rx_channel_decimation = {4, 4, 0, 0, 4, 4, 0, 0},
		.rx_channel_complex_to_real_enable = {0, 0, 0, 0, 0, 0, 0, 0},
		.rx_channel_enable = {1, 1, 0, 0, 1, 1, 0, 0},
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
	struct ad9081_phy* phy;
	int32_t status;

	printf("Hello\n");

	status = app_clock_init(app_clk);
	if (status != SUCCESS)
		printf("app_clock_init() error: %" PRId32 "\n", status);

	status = app_jesd_init(jesd_clk,
			       500000, 250000, 250000, 10000000, 10000000);
	if (status != SUCCESS)
		printf("app_jesd_init() error: %" PRId32 "\n", status);

	status = ad9081_init(&phy, &phy_param);
	if (status != SUCCESS)
		printf("ad9081_init() error: %" PRId32 "\n", status);

	axi_jesd204_rx_watchdog(rx_jesd);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

	rx_adc_init.num_channels = phy->jesd_rx_link[0].jesd_param.jesd_m +
				   phy->jesd_rx_link[1].jesd_param.jesd_m;;

	tx_dac_init.num_channels = phy->jesd_tx_link.jesd_param.jesd_m *
				   (phy->jesd_tx_link.jesd_param.jesd_duallink > 0 ? 2 : 1);;

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
		.adc_ddr_base = ADC_DDR_BASEADDR,
	};

	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
		.dac_ddr_base = DAC_DDR_BASEADDR
	};

	return iio_server_init(&iio_axi_adc_init_par, &iio_axi_dac_init_par);
#else
	printf("Bye\n");

	return SUCCESS;
#endif

}
