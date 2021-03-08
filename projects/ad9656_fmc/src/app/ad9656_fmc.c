/***************************************************************************//**
 *   @file   ad9656_fmc.c
 *   @brief  Implementation of Main Function.
 *   @author DHotolea (dan.hotoleanu@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <xparameters.h>
#include "xil_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include "ad9656.h"
#include "ad9508.h"
#include "ad9553.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "axi_adxcvr.h"
#include "axi_jesd204_rx.h"
#include "spi_extra.h"
#include "parameters.h"
#include "error.h"
#include "delay.h"

#ifdef IIO_SUPPORT
#include "app_iio.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define DMA_BUFFER		 0

/***************************************************************************//**
 * @brief main
 ******************************************************************************/
int main(void)
{
	int32_t status;

	/* Initialize SPI structures */
	struct spi_init_param ad9508_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 1,
		.mode = SPI_MODE_0
	};

	struct spi_init_param ad9553_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 2,
		.mode = SPI_MODE_0
	};
	struct spi_init_param ad9656_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.mode = SPI_MODE_0
	};

	struct xil_spi_init_param xil_spi_param = {
		.type = SPI_PS
	};

	/* this pattern is outputed by the ad9656 chip after the JESD204 test is finished */
	struct ad9656_user_input_test_pattern user_input_test_pattern = {
		.user_test_pattern1 = 0xA1B2,
		.user_test_pattern2 = 0xC3D4
	};

	ad9508_spi_param.device_id = SPI_DEVICE_ID;
	ad9508_spi_param.platform_ops = &xil_platform_ops;
	ad9508_spi_param.extra = &xil_spi_param;
	ad9553_spi_param.device_id = SPI_DEVICE_ID;
	ad9553_spi_param.platform_ops = &xil_platform_ops;
	ad9553_spi_param.extra = &xil_spi_param;
	ad9656_spi_param.device_id = SPI_DEVICE_ID;
	ad9656_spi_param.platform_ops = &xil_platform_ops;
	ad9656_spi_param.extra = &xil_spi_param;

	struct ad9508_init_param	ad9508_param;
	struct ad9553_init_param	ad9553_param;
	struct ad9656_init_param	ad9656_param;

	ad9508_param.spi_init = ad9508_spi_param;
	ad9553_param.spi_init = ad9553_spi_param;
	ad9656_param.spi_init = ad9656_spi_param;

	struct ad9508_dev *ad9508_device;
	struct ad9553_dev *ad9553_device;
	struct ad9656_dev *ad9656_device;

//******************************************************************************
// setup the base addresses
//******************************************************************************

	struct adxcvr_init ad9656_xcvr_param = {
		.name = "ad9656_xcvr",
		.base = XPAR_AXI_AD9656_RX_XCVR_BASEADDR,
		.sys_clk_sel = 0,
		.out_clk_sel = 4,
		.lpm_enable = 1,
		.cpll_enable = 1,
		.ref_rate_khz = 125000,
		.lane_rate_khz = 2500000
	};

//******************************************************************************
// ADC (AD9656) and the receive path ( AXI_ADXCVR,
//	JESD204, AXI_AD9656, RX DMAC) configuration
//******************************************************************************

	/* JESD initialization */
	struct jesd204_rx_init  ad9656_jesd_param = {
		.name = "ad9656_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = 2,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = 2500000/40,
		.lane_clk_khz = 2500000
	};

	struct axi_jesd204_rx *ad9656_jesd;

	/* ADC Core */
	struct axi_adc_init ad9656_core_param = {
		.name = "ad9656_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 4
	};
	struct axi_adc	*ad9656_core;
	struct adxcvr	*ad9656_xcvr;

//******************************************************************************
// configure the receiver DMA
//******************************************************************************

	struct axi_dmac_init ad9656_dmac_param = {
		.name = "ad9656_dmac",
		.base = RX_DMA_BASEADDR,
		.direction = DMA_DEV_TO_MEM,
		.flags = 0
	};
	struct axi_dmac *ad9656_dmac;

//******************************************************************************
// bring up the system
//******************************************************************************

	ad9656_param.lane_rate_kbps = 2500000;

	// setup clocks
	if (ad9508_setup(&ad9508_device, &ad9508_param) != SUCCESS)
		printf("The ad9508 chip could not be setup correctly!\n");
	else
		printf("The ad9508 chip successfully configured\n");

	if (ad9553_setup(&ad9553_device, &ad9553_param) != SUCCESS)
		printf("The ad9553 chip could not be setup!\n");
	else
		printf("The ad9553 chip successfully configured\n");

	// ADC
	if (ad9656_setup(&ad9656_device, &ad9656_param) != SUCCESS)
		printf("The ad9656 chip could not be setup correctly!\n");
	else
		printf("The ad9656 chip successfully configured\n");

	if (adxcvr_init(&ad9656_xcvr, &ad9656_xcvr_param) != SUCCESS) {
		printf("error: %s: adxcvr_init() failed\n", ad9656_xcvr->name);
	}

	if (adxcvr_clk_enable(ad9656_xcvr) != SUCCESS) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9656_xcvr->name);
	}

	if (axi_jesd204_rx_init(&ad9656_jesd, &ad9656_jesd_param) != SUCCESS) {
		printf("error: %s: axi_jesd204_rx_init() failed\n", ad9656_jesd->name);
	}

	if (axi_jesd204_rx_lane_clk_enable(ad9656_jesd) != SUCCESS) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9656_jesd->name);
	}

	status = axi_jesd204_rx_status_read(ad9656_jesd);
	if (status != SUCCESS) {
		printf("axi_jesd204_rx_status_read() error: %d\n", status);
	}

	if (axi_adc_init(&ad9656_core,  &ad9656_core_param) != SUCCESS) {
		printf("axi_adc_init() error: %s\n", ad9656_core->name);
	}

//******************************************************************************
// receive path testing
//******************************************************************************

	/* receive path testing */
	ad9656_JESD204_test(ad9656_device, AD9656_TEST_PN9);
	if(axi_adc_pn_mon(ad9656_core, AXI_ADC_PN9, 10) == -1) {
		printf("%s ad9656 - PN9 sequence mismatch!\n", __func__);
	} else {
		printf("%s ad9656 - PN9 sequence checked!\n", __func__);
	}
	ad9656_JESD204_test(ad9656_device, AD9656_TEST_PN23);
	if(axi_adc_pn_mon(ad9656_core, AXI_ADC_PN23A, 10) == -1) {
		printf("%s ad9656 - PN23 sequence mismatch!\n", __func__);
	} else {
		printf("%s ad9656 - PN23 sequence checked!\n", __func__);
	}

	ad9656_JESD204_test(ad9656_device, AD9656_TEST_OFF);

//******************************************************************************
// generate user input in place of real ADC data and capture data with DMA
//******************************************************************************

	/* start sending user input test pattern */
	if (ad9656_user_input_test(ad9656_device, AD9656_TEST_USER_INPUT,
				   user_input_test_pattern) == -1) {
		printf("Could not start the user input test sequence!\n");
	} else {
		printf("User input test sequence started!\n");
	}

	/* Initialize the DMAC and transfer 16384 samples from ADC to MEM */
	axi_dmac_init(&ad9656_dmac, &ad9656_dmac_param);
	axi_dmac_transfer(ad9656_dmac, ADC_DDR_BASEADDR, 16384 * 4);

	ad9656_user_input_test(ad9656_device, AD9656_TEST_OFF, user_input_test_pattern);

	printf("ad9656: setup, configuration and test program is done\n");

#ifdef IIO_SUPPORT
	printf("The board accepts libiio clients connections through the serial backend.\n");

	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad9656_core,
		.rx_dmac = ad9656_dmac,
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
	};

	return iio_app_start(&iio_axi_adc_init_par);
#endif

	/* Memory deallocation for devices and spi */
	ad9508_remove(ad9508_device);
	ad9553_remove(ad9553_device);
	ad9656_remove(ad9656_device);

	return(0);
}
