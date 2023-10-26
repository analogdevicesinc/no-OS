/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Xilinx platform of ADF4382 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <inttypes.h>
#include "parameters.h"
#include "common_data.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "ad9152.h"
#include "ad9528.h"
#include "ad9680.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_jesd204_tx.h"
#include "axi_jesd204_rx.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_ad9680.h"
#include "iio_ad9152.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "xilinx_uart.h"
#endif

/* DAQ3 reconfiguration */
void fmcdaq3_reconfig(struct ad9152_init_param *ad9152_param,
		      struct adxcvr_init *ad9152_xcvr_param,
		      struct ad9680_init_param *ad9680_param,
		      struct adxcvr_init *ad9680_xcvr_param,
		      struct ad9528_platform_data *ad9528_param)
{

	uint8_t mode = 0;

	printf ("Available sampling rates:\n");
	printf ("\t1 - ADC 1233 MSPS; DAC 1233 MSPS\n");
	printf ("\t2 - ADC 616.5 MSPS; DAC 616.5 MSPS\n");

	mode = getc(stdin);

	switch (mode) {
	case '2':
		printf ("2 - ADC 616.5 MSPS; DAC 616.5 MSPS\n");
		ad9680_param->lane_rate_kbps = 6165000;
		ad9152_param->lane_rate_kbps = 6165000;
		ad9152_xcvr_param->lane_rate_khz = 6165000;
#ifndef ALTERA_PLATFORM
		ad9152_xcvr_param->ref_rate_khz = 308250;
#else
		ad9152_xcvr_param->parent_rate_khz = 308250;
#endif
		ad9680_xcvr_param->lane_rate_khz = 6165000;
#ifndef ALTERA_PLATFORM
		ad9680_xcvr_param->ref_rate_khz = 308250;
#else
		ad9680_xcvr_param->parent_rate_khz = 308250;
#endif
		(&ad9528_param->channels[0])->channel_divider = 2;
		(&ad9528_param->channels[2])->channel_divider = 4;
		(&ad9528_param->channels[4])->channel_divider = 2;
		(&ad9528_param->channels[6])->channel_divider = 4;
		break;
	default:
		printf ("1 - ADC 1233 MSPS; DAC 1233 MSPS\n");
		break;
	}
}

/***************************************************************************//**
 * @brief main
 ******************************************************************************/
int main(void)
{

	int32_t status;

	struct no_os_gpio_desc *dac_txen;
	struct no_os_gpio_desc *adc_pd;

	struct ad9528_dev* ad9528_device;
	struct ad9152_dev* ad9152_device;
	struct ad9680_dev* ad9680_device;

	struct adxcvr	*ad9152_xcvr;
	struct adxcvr	*ad9680_xcvr;

	struct axi_jesd204_tx *ad9152_jesd;
	struct axi_jesd204_rx *ad9680_jesd;

	struct axi_adc	*ad9680_core;
	struct axi_dac	*ad9152_core;

	struct axi_dmac *ad9680_dmac;

	uint32_t stpl_samples[2][4] = {
		{
			(ad9152_channels[0].pat_data >> 0)  & 0xffff,
			(ad9152_channels[0].pat_data >> 16) & 0xffff,
			(ad9152_channels[0].pat_data >> 0)  & 0xffff,
			(ad9152_channels[0].pat_data >> 16) & 0xffff,
		},
		{
			(ad9152_channels[1].pat_data >> 0)  & 0xffff,
			(ad9152_channels[1].pat_data >> 16) & 0xffff,
			(ad9152_channels[1].pat_data >> 0)  & 0xffff,
			(ad9152_channels[1].pat_data >> 16) & 0xffff,
		}
	};
	ad9680_param.spi_init = ad9680_spi_param;
	ad9528_param.spi_init = ad9528_spi_param;
	ad9152_param.spi_init = ad9152_spi_param;
	ad9152_param.stpl_samples = stpl_samples;

#ifndef ALTERA_PLATFORM
	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();
#endif

	/* set GPIOs */
	no_os_gpio_get(&dac_txen,  &dac_txen_param);
	no_os_gpio_get(&adc_pd,    &adc_pd_param);

	no_os_gpio_direction_output(dac_txen,  1);
	no_os_gpio_direction_output(adc_pd,    0);

	fmcdaq3_reconfig(&ad9152_param,
			 &ad9152_xcvr_param,
			 &ad9680_param,
			 &ad9680_xcvr_param,
			 ad9528_param.pdata);

	/* Reconfigure the default JESD configurations */
	ad9680_jesd_param.lane_clk_khz = ad9680_xcvr_param.lane_rate_khz;
	ad9680_jesd_param.device_clk_khz = ad9680_xcvr_param.lane_rate_khz / 40;
	ad9152_jesd_param.lane_clk_khz = ad9152_xcvr_param.lane_rate_khz;
	ad9152_jesd_param.device_clk_khz = ad9152_xcvr_param.lane_rate_khz / 40;

	status = ad9528_setup(&ad9528_device, ad9528_param);
	if (status != 0) {
		printf("error: ad9523_setup() failed\n");
	}
	// Recommended DAC JESD204 link startup sequence
	//   1. FPGA JESD204 Link Layer
	//   2. FPGA JESD204 PHY Layer
	//   3. DAC

#ifdef ALTERA_PLATFORM
	/* Initialize A10 FPLLs */
	status = altera_a10_fpll_init(&ad9680_device_clk_pll,
				      &ad9680_device_clk_pll_param);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       ad9680_device_clk_pll_param.name);
	}
	status = altera_a10_fpll_init(&ad9152_device_clk_pll,
				      &ad9152_device_clk_pll_param);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_init() failed\n",
		       ad9152_device_clk_pll_param.name);
	}

	altera_a10_fpll_disable(ad9680_device_clk_pll);
	status = altera_a10_fpll_set_rate(ad9680_device_clk_pll,
					  ad9680_jesd_param.device_clk_khz * 1000);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       ad9680_device_clk_pll->name);
	}
	altera_a10_fpll_enable(ad9680_device_clk_pll);
	altera_a10_fpll_disable(ad9152_device_clk_pll);
	status = altera_a10_fpll_set_rate(ad9152_device_clk_pll,
					  ad9152_jesd_param.device_clk_khz * 1000);
	if (status != 0) {
		printf("error: %s: altera_a10_fpll_set_rate() failed\n",
		       ad9152_device_clk_pll->name);
	}
	altera_a10_fpll_enable(ad9152_device_clk_pll);
#endif
	status = ad9680_setup(&ad9680_device, &ad9680_param);
	if (status != 0) {
		printf("error: ad9680_setup() failed\n");
	}
	status = axi_jesd204_tx_init_legacy(&ad9152_jesd, &ad9152_jesd_param);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_init_legacy() failed\n", ad9152_jesd->name);
	}
	status = axi_jesd204_tx_lane_clk_enable(ad9152_jesd);
	if (status != 0) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9152_jesd->name);
	}
	status = adxcvr_init(&ad9152_xcvr, &ad9152_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", ad9152_xcvr->name);
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(ad9152_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9152_xcvr->name);
	}
#endif
	status = adxcvr_init(&ad9680_xcvr, &ad9680_xcvr_param);
	if (status != 0) {
		printf("error: %s: adxcvr_init() failed\n", ad9680_xcvr->name);
	}
#ifndef ALTERA_PLATFORM
	status = adxcvr_clk_enable(ad9680_xcvr);
	if (status != 0) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9680_xcvr->name);
	}
#endif
	status = axi_jesd204_rx_init_legacy(&ad9680_jesd, &ad9680_jesd_param);
	if (status != 0) {
		printf("error: %s: axi_jesd204_rx_init_legacy() failed\n", ad9680_jesd->name);
	}
	status = axi_jesd204_rx_lane_clk_enable(ad9680_jesd);
	if (status != 0) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9680_jesd->name);
	}
	status = ad9152_setup(&ad9152_device, ad9152_param);
	if (status != 0) {
		printf("error: ad9152_setup() failed\n");
	}
	status = axi_adc_init(&ad9680_core,  &ad9680_core_param);
	if (status != 0) {
		printf("axi_adc_init() error: %s\n", ad9680_core->name);
	}
	status = axi_dac_init(&ad9152_core, &ad9152_core_param);
	if (status != 0) {
		printf("axi_dac_init() error: %s\n", ad9152_core->name);
	}

	status = axi_jesd204_rx_status_read(ad9680_jesd);
	if (status != 0) {
		printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n", status);
	}

	status = axi_jesd204_tx_status_read(ad9152_jesd);
	if (status != 0) {
		printf("axi_jesd204_tx_status_read() error: %"PRIi32"\n", status);
	}

	ad9680_test(ad9680_device, AD9680_TEST_PN9);
	if(axi_adc_pn_mon(ad9680_core, AXI_ADC_PN9, 10) == -1) {
		printf("%s ad9680 - PN9 sequence mismatch!\n", __func__);
	};
	ad9680_test(ad9680_device, AD9680_TEST_PN23);
	if(axi_adc_pn_mon(ad9680_core, AXI_ADC_PN23A, 10) == -1) {
		printf("%s ad9680 - PN23 sequence mismatch!\n", __func__);
	};

	ad9152_status(ad9152_device);

	// ad9152-x1 do not support data path prbs (use short-tpl)

	ad9152_core_param.channels[0].sel = AXI_DAC_DATA_SEL_SED;
	ad9152_core_param.channels[1].sel = AXI_DAC_DATA_SEL_SED;
	axi_dac_data_setup(ad9152_core);
	ad9152_short_pattern_test(ad9152_device, ad9152_param);

	// ad9152-xN (n > 1) supports data path prbs

	ad9152_core_param.channels[0].sel = AXI_DAC_DATA_SEL_PN23;
	ad9152_core_param.channels[1].sel = AXI_DAC_DATA_SEL_PN23;
	axi_dac_data_setup(ad9152_core);
	ad9152_param.prbs_type = AD9152_TEST_PN7;
	ad9152_datapath_prbs_test(ad9152_device, ad9152_param);

	ad9152_core_param.channels[0].sel = AXI_DAC_DATA_SEL_PN31;
	ad9152_core_param.channels[1].sel = AXI_DAC_DATA_SEL_PN31;
	axi_dac_data_setup(ad9152_core);
	ad9152_param.prbs_type = AD9152_TEST_PN15;
	ad9152_datapath_prbs_test(ad9152_device, ad9152_param);

	// default data

	ad9152_core_param.channels[0].sel = AXI_DAC_DATA_SEL_DDS;
	ad9152_core_param.channels[1].sel = AXI_DAC_DATA_SEL_DDS;
	axi_dac_data_setup(ad9152_core);
	ad9680_test(ad9680_device, AD9680_TEST_OFF);

	// capture data with DMA
	/* Initialize the DMAC and transfer 16384 samples from ADC to MEM */
	axi_dmac_init(&ad9680_dmac, &ad9680_dmac_param);

	struct axi_dma_transfer transfer_rx = {
		// Number of bytes to write/read
		.size = 16384 * 2,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
	axi_dmac_transfer_start(ad9680_dmac, &transfer_rx);
	status = axi_dmac_transfer_wait_completion(ad9680_dmac, 500);
	if(status)
		return status;
#ifdef XILINX_PLATFORM
	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR,
				  16384 * 2);
#endif

#ifdef IIO_SUPPORT
	
	status = iio_example_main();
	if(status)
		return status;

#endif

	printf("daq3: setup and configuration is done\n");

	ad9528_remove(ad9528_device);
	ad9152_remove(ad9152_device);
	ad9680_remove(ad9680_device);
	no_os_gpio_remove(dac_txen);
	no_os_gpio_remove(adc_pd);

#ifndef ALTERA_PLATFORM
	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();
#endif

	return(0);
}
