/***************************************************************************//**
 *   @file   ad9739a_fmc_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "xil_cache.h"
#include "xparameters.h"
#include "spi.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "ad9739a.h"
#include "adf4350.h"
#include "parameters.h"
#include "spi_extra.h"
#include "error.h"

#ifdef IIO_SUPPORT
#include "app_iio.h"
#endif

#include "print_log.h"

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;

	/* Initialize SPI structures */

	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS,
	};

	struct spi_init_param adf4350_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 5000000u,
		.chip_select = 0,
		.mode = SPI_MODE_0,
		.extra = &xil_spi_param,
		.platform_ops = &xil_platform_ops,
	};

	struct spi_init_param ad9739_spi_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 20000000u,
		.chip_select = 1,
		.mode = SPI_MODE_0,
		.extra = &xil_spi_param,
		.platform_ops = &xil_platform_ops,
	};

	adf4350_init_param default_adf4350_init_param = {
		/* SPI */
		.spi_init = adf4350_spi_param,
		/* Device settings */
		.clkin = 25000000,
		.channel_spacing = 10000,
		.power_up_frequency = 2500000000ul,
		.reference_div_factor = 0,
		.reference_doubler_enable = 0,
		.reference_div2_enable = 0,

		/* r2_user_settings */
		.phase_detector_polarity_positive_enable = 1,
		.lock_detect_precision_6ns_enable = 0,
		.lock_detect_function_integer_n_enable = 0,
		.charge_pump_current = 2500,
		.muxout_select = 0,
		.low_spur_mode_enable = 0,

		/* r3_user_settings */
		.cycle_slip_reduction_enable = 0,
		.charge_cancellation_enable = 0,
		.anti_backlash_3ns_enable = 0,
		.band_select_clock_mode_high_enable = 0,
		.clk_divider_12bit = 0,
		.clk_divider_mode = 0,

		/* r4_user_settings */
		.aux_output_enable = 0,
		.aux_output_fundamental_enable = 1,
		.mute_till_lock_enable= 0,
		.output_power = 3,
		.aux_output_power = 0,
	};
	adf4350_dev *adf4350_device;

	struct ad9739a_init_param ad9739a_init_param = {
		.spi_init = ad9739_spi_param,
		.full_scale_current = 20.0,
		.common_mode_voltage_dacclk_n = 0xF,
		.common_mode_voltage_dacclk_p = 0xF
	};
	struct ad9739a_dev *ad9739a_device;

	/* AXI DAC Core */
	struct axi_dac_channel ad9739a_channels[1];

	struct axi_dac_init ad9739a_core_param = {
		.name = "ad9739a_dac",
		.base =	TX_CORE_BASEADDR,
		.num_channels = 1,
		.channels = &ad9739a_channels[0]
	};
	struct axi_dac	*ad9739a_core;

	status = adf4350_setup(&adf4350_device, default_adf4350_init_param);
	if (status != SUCCESS) {
		pr_info("adf4350_setup() failed!");
		return FAILURE;
	}

	status = axi_dac_init(&ad9739a_core, &ad9739a_core_param);
	if (status != SUCCESS) {
		pr_info("axi_dac_init() error: %s\n", ad9739a_core->name);
		return FAILURE;
	}

	ad9739a_setup(&ad9739a_device, ad9739a_init_param);
	if (status != SUCCESS) {
		pr_info("ad9739a_setup() error: %s\n", ad9739a_core->name);
		return FAILURE;
	}

#ifdef DAC_DMA_EXAMPLE
	extern const uint16_t sine_lut[128];
	struct axi_dmac_init ad9739a_dmac_init_param = {
		.name = "ad9739a_dmac",
		.base = TX_DMA_BASEADDR,
		.direction = DMA_MEM_TO_DEV,
		.flags = DMA_CYCLIC
	};
	struct axi_dmac *ad9739a_dmac_desc;

	ad9739a_channels[0].sel = AXI_DAC_DATA_SEL_DMA;
	axi_dac_data_setup(ad9739a_core);
	axi_dmac_init(&ad9739a_dmac_desc, &ad9739a_dmac_init_param);
	axi_dac_set_buff(ad9739a_core, DAC_DDR_BASEADDR, sine_lut,
			 ARRAY_SIZE(sine_lut));
	Xil_DCacheFlush();
	axi_dmac_transfer(ad9739a_dmac_desc, DAC_DDR_BASEADDR,
			  ARRAY_SIZE(sine_lut) * sizeof(uint16_t));
#else
	ad9739a_channels[0].dds_dual_tone = 0;
	ad9739a_channels[0].dds_frequency_0 = 33*1000*1000;
	ad9739a_channels[0].dds_phase_0 = 0;
	ad9739a_channels[0].dds_scale_0 = 250000;
	ad9739a_channels[0].sel = AXI_DAC_DATA_SEL_DDS;
	axi_dac_data_setup(ad9739a_core);
#endif

#ifdef IIO_SUPPORT
	pr_info("The board accepts libiio clients connections through the serial backend.\n");

	struct axi_dmac_init ad9739a_dmac_param = {
		.name = "ad9739a_dmac",
		.base = TX_DMA_BASEADDR,
		.direction = DMA_MEM_TO_DEV,
		.flags = DMA_CYCLIC
	};
	struct axi_dmac *ad9739a_dmac;

	axi_dmac_init(&ad9739a_dmac, &ad9739a_dmac_param);

	struct iio_axi_dac_init_param iio_axi_dac_init_par;
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = ad9739a_core,
		.tx_dmac = ad9739a_dmac,
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange
	};

	return iio_app_start(&iio_axi_dac_init_par);
#endif
	pr_info("Done.\n");

	return SUCCESS;
}
