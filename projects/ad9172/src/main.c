/***************************************************************************//**
 *   @file   ad9172/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Cristian Pop (cristianpop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#include "hmc7044.h"
#include "ad9172.h"
#include "axi_adxcvr.h"
#include "axi_jesd204_tx.h"
#include "axi_dac_core.h"
#include "parameters.h"
#include "inttypes.h"
#include "error.h"
#include <xparameters.h>
#include <xil_cache.h>
#include "app_config.h"
#include "spi.h"
#include "spi_extra.h"
#include "gpio.h"
#include "gpio_extra.h"

#ifdef DAC_DMA_EXAMPLE
#include "axi_dmac.h"
#endif /* DAC_DMA_EXAMPLE */

#ifdef IIO_SUPPORT
#include "app_iio.h"
#endif

int main(void)
{
	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
		.flags = 0
	};

	struct spi_init_param hmc7044_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 10000000,
		.mode = SPI_MODE_0,
		.chip_select = SPI_HMC7044_CS,
		.platform_ops = &xil_platform_ops,
		.extra = &xil_spi_param
	};

	struct hmc7044_chan_spec chan_spec[4] = {
		{.disable = 0, .num = 2, .divider = 8, .driver_mode = 1},		/* DAC_CLK */
		{.disable = 0, .num = 3, .divider = 512, .driver_mode = 1},		/* DAC_SYSREF */
		{.disable = 0, .num = 12, .divider = 8, .driver_mode = 2},		/* FPGA_CLK */
		{.disable = 0, .num = 13, .divider = 512, .driver_mode = 2},	/* FPGA_SYSREF */
	};

	struct hmc7044_init_param hmc7044_param = {
		.spi_init = &hmc7044_spi_param,
		.clkin_freq = {122880000, 0, 0, 0},
		.vcxo_freq = 122880000,
		.pll2_freq = 2949120000,
		.pll1_loop_bw = 200,
		.sysref_timer_div = 1024,
		.pulse_gen_mode = 0,
		.in_buf_mode = {0x15, 0, 0, 0, 0x15},
		.gpi_ctrl = {0x00, 0x00, 0x00, 0x00},
		.gpo_ctrl = {0x1f, 0x2b, 0x00, 0x00},
		.num_channels = 4,
		.channels = chan_spec,
	};

	struct jesd204_tx_init tx_jesd_init = {
		.name = "tx_jesd",
		.base = TX_JESD_BASEADDR,
		.octets_per_frame = 2,
		.frames_per_multiframe = 32,
		.converters_per_device = 4,
		.converter_resolution = 16,
		.bits_per_sample = 16,
		.high_density = false,
		.control_bits_per_sample = 0,
		.subclass = 1,
		.device_clk_khz = 184320,	/* (lane_clk_khz / 40) */
		.lane_clk_khz = 7372800,	/* LaneRate = ( M/L)*NP*(10/8)*DataRate */
	};

	struct adxcvr_init tx_adxcvr_init = {
		.name = "tx_adxcvr",
		.base = TX_XCVR_BASEADDR,
		.sys_clk_sel = 3,
		.out_clk_sel = 4,
		.cpll_enable = 0,
		.lpm_enable = 1,
		.lane_rate_khz = 7372800,	/* LaneRate = ( M/L)*NP*(10/8)*DataRate */
		.ref_rate_khz = 368640,		/* FPGA_CLK, output 12 of HMC 7044 */
	};

	struct spi_init_param ad9172_spi_param = {
		.max_speed_hz = 1000000,
		.mode = SPI_MODE_0,
		.chip_select = SPI_AD9172_CS,
		.platform_ops = &xil_platform_ops,
		.extra = &xil_spi_param
	};

	struct xil_gpio_init_param xilinx_gpio_init_param = {
		.type = GPIO_PS,
		.device_id = GPIO_DEVICE_ID
	};
	struct ad9172_init_param ad9172_param = {
		.spi_init = &ad9172_spi_param,	/* spi_init_param */
		.gpio_reset = {
			.number = 54 + 0,
			.platform_ops = &xil_gpio_platform_ops,
			.extra = &xilinx_gpio_init_param
		},
		.gpio_txen0 = {
			.number = 54 + 22,
			.platform_ops = &xil_gpio_platform_ops,
			.extra = &xilinx_gpio_init_param
		},
		.gpio_txen1 = {
			.number = 54 + 23,
			.platform_ops = &xil_gpio_platform_ops,
			.extra = &xilinx_gpio_init_param
		},
		.dac_rate_khz = 11796480,		/* or sample rate */
		.dac_clkin_Hz = 368640000,		/* DAC_CLK, output 2 of HMC 7044 */
		.jesd_link_mode = 4,
		.jesd_subclass = 1,
		.dac_interpolation = 8,
		.channel_interpolation = 4,
		.clock_output_config = 4,
		.syncoutb_type = SIGNAL_LVDS,
		.sysref_coupling = COUPLING_AC,
	};

	struct axi_dac_init tx_dac_init = {
		"tx_dac",
		TX_CORE_BASEADDR,
		4,
		NULL
	};

#ifdef DAC_DMA_EXAMPLE
	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		DMA_MEM_TO_DEV,
		DMA_LAST,
	};
	struct axi_dmac *tx_dmac;
#endif /* DAC_DMA_EXAMPLE */

	struct hmc7044_dev *hmc7044_device;
	struct ad9172_dev *ad9172_device;
	struct axi_jesd204_tx *tx_jesd;
	struct adxcvr *tx_adxcvr;
	struct axi_dac *tx_dac;
	int32_t status;

	status = hmc7044_init(&hmc7044_device, &hmc7044_param);
	if (status != SUCCESS) {
		printf("hmc7044_init() error: %"PRIi32"\n", status);
		goto error_1;
	}

	status = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_tx_init() failed\n", tx_jesd_init.name);
		goto error_2;
	}

	status = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_init() failed\n", tx_adxcvr_init.name);
		goto error_3;
	}

	status = adxcvr_clk_enable(tx_adxcvr);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_clk_enable() failed\n", tx_adxcvr->name);
		goto error_3;
	}

	status = axi_jesd204_tx_lane_clk_enable(tx_jesd);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n", tx_jesd->name);
		goto error_3;
	}

	status = ad9172_init(&ad9172_device, &ad9172_param);
	if (status != SUCCESS) {
		printf("ad9172_init() error: %"PRIi32"\n", status);
		goto error_4;
	}

	status = axi_jesd204_tx_status_read(tx_jesd);
	if (status != SUCCESS) {
		printf("axi_jesd204_tx_status_read() error: %"PRIi32"\n", status);
		goto error_4;
	}

	status = axi_dac_init(&tx_dac, &tx_dac_init);
	if (status != SUCCESS) {
		printf("axi_dac_init() error: %"PRIi32"\n", status);
		goto error_5;
	}

#ifdef DAC_DMA_EXAMPLE
	extern const uint32_t sine_lut_iq[1024];
	axi_dac_load_custom_data(tx_dac, sine_lut_iq,
				 ARRAY_SIZE(sine_lut_iq),
				 DDR_MEM_BASEADDR);
#ifndef ALTERA_PLATFORM
	Xil_DCacheFlush();
#endif
	axi_dmac_init(&tx_dmac, &tx_dmac_init);
	axi_dmac_transfer(tx_dmac, DDR_MEM_BASEADDR,
			  sizeof(sine_lut_iq) * (tx_dac->num_channels / 2));
#else /* DAC_DMA_EXAMPLE */
	printf("Set dds frequency at 40MHz\n");

	axi_dac_dds_set_frequency(tx_dac, 0, 40000000);	/* TX1_I_F1 */
	axi_dac_dds_set_frequency(tx_dac, 1, 40000000);	/* TX1_I_F2 */
	axi_dac_dds_set_frequency(tx_dac, 2, 40000000);	/* TX1_Q_F1 */
	axi_dac_dds_set_frequency(tx_dac, 3, 40000000);	/* TX1_Q_F2 */

	axi_dac_dds_set_frequency(tx_dac, 4, 40000000); /* TX2_I_F1 */
	axi_dac_dds_set_frequency(tx_dac, 5, 40000000); /* TX2_I_F2 */
	axi_dac_dds_set_frequency(tx_dac, 6, 40000000); /* TX2_Q_F1 */
	axi_dac_dds_set_frequency(tx_dac, 7, 40000000); /* TX2_Q_F2 */


	axi_dac_dds_set_scale(tx_dac, 0, 250000);
	axi_dac_dds_set_scale(tx_dac, 1, 250000);
	axi_dac_dds_set_scale(tx_dac, 2, 250000);
	axi_dac_dds_set_scale(tx_dac, 3, 250000);

	axi_dac_dds_set_scale(tx_dac, 4, 250000);
	axi_dac_dds_set_scale(tx_dac, 5, 250000);
	axi_dac_dds_set_scale(tx_dac, 6, 250000);
	axi_dac_dds_set_scale(tx_dac, 7, 250000);


	axi_dac_dds_set_phase(tx_dac, 0, 90000);
	axi_dac_dds_set_phase(tx_dac, 1, 90000);
	axi_dac_dds_set_phase(tx_dac, 2, 0);
	axi_dac_dds_set_phase(tx_dac, 3, 0);

	axi_dac_dds_set_phase(tx_dac, 4, 90000);
	axi_dac_dds_set_phase(tx_dac, 5, 90000);
	axi_dac_dds_set_phase(tx_dac, 6, 0);
	axi_dac_dds_set_phase(tx_dac, 7, 0);

	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
#endif /* DAC_DMA_EXAMPLE */

#ifdef IIO_SUPPORT
	printf("The board accepts libiio clients connections through the serial backend.\n");

	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		DMA_MEM_TO_DEV,
		DMA_LAST,
	};
	struct axi_dmac *tx_dmac;

	axi_dmac_init(&tx_dmac, &tx_dmac_init);

	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange
	};

	return iio_server_init(&iio_axi_dac_init_par);
#endif

	printf("Bye\n");

error_5:
	axi_dac_remove(tx_dac);
error_4:
	ad9172_remove(ad9172_device);
error_3:
	adxcvr_remove(tx_adxcvr);
error_2:
	axi_jesd204_tx_remove(tx_jesd);
error_1:
	hmc7044_remove(hmc7044_device);

	return 0;
}
