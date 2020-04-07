/***************************************************************************//**
 *   @file   fmcjesdadc1.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Antoniu Miclaus(antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
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
#include "ad9517.h"
#include "ad9250.h"
#include "parameters.h"
#include <xparameters.h>
#include <xil_printf.h>
#include <xil_cache.h>
#include "axi_adxcvr.h"
#include "spi.h"
#include "gpio.h"
#include "spi_extra.h"
#include "gpio_extra.h"
#include "delay.h"
#include "error.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "axi_jesd204_rx.h"

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{

	int32_t status;

	// SPI configuration
	struct spi_init_param ad9250_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.mode = SPI_MODE_0
	};

	struct spi_init_param ad9517_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.mode = SPI_MODE_0
	};

	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.device_id = SPI_DEVICE_ID
	};
	ad9250_spi_param.extra = &xil_spi_param;
	ad9517_spi_param.extra = &xil_spi_param;

	struct gpio_init_param gpio_sysref_param = {
		.number = GPIO_JESD204_SYSREF
	};

	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
	gpio_sysref_param.extra = &xil_gpio_param;

	gpio_desc *gpio_sysref;

	struct adxcvr_init ad9250_xcvr_param = {
		.name = "ad9250_xcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = 0,
		.out_clk_sel = 2,
		.lpm_enable = 0,
		.cpll_enable = 1,
		.ref_rate_khz = 245760,
		.lane_rate_khz = 4915200,
	};
	struct adxcvr *ad9250_xcvr;

	struct jesd204_rx_init  ad9250_jesd_param = {
		.name = "ad9250_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = 2,
		.frames_per_multiframe = 32,
		.subclass = 1,
		.device_clk_khz = 4915200 / 40,
		.lane_clk_khz = 4915200
	};
	struct axi_jesd204_rx *ad9250_jesd;

	struct axi_adc_init ad9250_0_core_param = {
		.name = "ad9250_0_adc",
		.base = RX_0_CORE_BASEADDR,
		.num_channels = 2
	};
	struct axi_adc	*ad9250_0_core;

	struct axi_adc_init ad9250_1_core_param = {
		.name = "ad9250_1_adc",
		.base = RX_1_CORE_BASEADDR,
		.num_channels = 2
	};
	struct axi_adc	*ad9250_1_core;

	struct axi_dmac_init ad9250_0_dmac_param = {
		.name = "ad9250_0_dmac",
		.base = RX_DMA_0_BASEADDR,
		.direction = DMA_DEV_TO_MEM,
		.flags = 0
	};
	struct axi_dmac *ad9250_0_dmac;

	struct axi_dmac_init ad9250_1_dmac_param = {
		.name = "ad9250_1_dmac",
		.base = RX_DMA_1_BASEADDR,
		.direction = DMA_DEV_TO_MEM,
		.flags = 0
	};
	struct axi_dmac *ad9250_1_dmac;

	struct ad9250_dev		*ad9250_0_device;
	struct ad9250_dev		*ad9250_1_device;
	struct ad9517_dev		*ad9517_device;
	struct ad9250_init_param	ad9250_0_param;
	struct ad9250_init_param	ad9250_1_param;
	struct ad9517_init_param	ad9517_param;

	// SPI configuration
	ad9517_param.spi_init = ad9517_spi_param;
	ad9250_0_param.spi_init = ad9250_spi_param;
	ad9250_1_param.spi_init = ad9250_spi_param;

	ad9250_0_param.id_no = 0x0;
	ad9250_1_param.id_no = 0x1;

	// ADC and receive path configuration
	ad9250_0_param.lane_rate_kbps = 4915200;
	ad9250_1_param.lane_rate_kbps = 4915200;

	// setup GPIOs
	gpio_get(&gpio_sysref,  &gpio_sysref_param);
	gpio_direction_output(gpio_sysref,  1);
	mdelay(10);

	// set up clock generator
	status = ad9517_setup(&ad9517_device, ad9517_param);
	if(status < 0)
		printf("Error ad9517_setup()\n");

	// set up the devices
	status= ad9250_setup(&ad9250_0_device, ad9250_0_param);
	if(status< 0)
		printf("Error ad9250_0_setup()\n");

	status= ad9250_setup(&ad9250_1_device, ad9250_1_param);
	if(status< 0)
		printf("Error ad9250_0_setup()\n");

	// set up the XCVR core
	status = adxcvr_init(&ad9250_xcvr, &ad9250_xcvr_param);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_init() failed\n", ad9250_xcvr->name);
	}
	status = adxcvr_clk_enable(ad9250_xcvr);
	if (status != SUCCESS) {
		printf("error: %s: adxcvr_clk_enable() failed\n", ad9250_xcvr->name);
	}

	// setup JESD core
	status = axi_jesd204_rx_init(&ad9250_jesd, &ad9250_jesd_param);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_rx_init() failed\n", ad9250_jesd->name);
	}
	status = axi_jesd204_rx_lane_clk_enable(ad9250_jesd);
	if (status != SUCCESS) {
		printf("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad9250_jesd->name);
	}

	// interface core setup
	status = axi_adc_init(&ad9250_0_core,  &ad9250_0_core_param);
	if (status != SUCCESS) {
		printf("axi_adc_init() error: %s\n", ad9250_0_core->name);
	}

	status = axi_adc_init(&ad9250_1_core,  &ad9250_1_core_param);
	if (status != SUCCESS) {
		printf("axi_adc_init() error: %s\n", ad9250_1_core->name);
	}

	// JESD core status
	status = axi_jesd204_rx_status_read(ad9250_jesd);
	if (status != SUCCESS) {
		printf("axi_jesd204_rx_status_read() error: %"PRIi32"\n", status);
	}

	// PRBS test
	ad9250_test(ad9250_0_device, AD9250_TEST_PNLONG);
	if(axi_adc_pn_mon(ad9250_0_core, AXI_ADC_PN23, 10) == -1) {
		printf("%s ad9250_0 - PN23 sequence mismatch!\n", __func__);
	};
	ad9250_test(ad9250_1_device, AD9250_TEST_PNLONG);
	if(axi_adc_pn_mon(ad9250_1_core, AXI_ADC_PN23, 10) == -1) {
		printf("%s ad9250_1 - PN23 sequence mismatch!\n", __func__);
	};

	// set up ramp output
	ad9250_test(ad9250_0_device, AD9250_TEST_RAMP);
	ad9250_test(ad9250_1_device, AD9250_TEST_RAMP);

	// test the captured data
	axi_dmac_init(&ad9250_0_dmac, &ad9250_0_dmac_param);
	axi_dmac_transfer(ad9250_0_dmac, ADC_0_DDR_BASEADDR, 16384 * 2);

	axi_dmac_init(&ad9250_1_dmac, &ad9250_1_dmac_param);
	axi_dmac_transfer(ad9250_1_dmac, ADC_1_DDR_BASEADDR, 16384 * 2);

	// set up normal output
	ad9250_test(ad9250_0_device, AD9250_TEST_OFF);
	ad9250_test(ad9250_1_device, AD9250_TEST_OFF);

	// capture data with DMA
	axi_dmac_transfer(ad9250_0_dmac, ADC_0_DDR_BASEADDR, 16384 * 2);
	axi_dmac_transfer(ad9250_1_dmac, ADC_1_DDR_BASEADDR, 16384 * 2);

	printf("adc1: setup and configuration is done\n");

	ad9517_remove(ad9517_device);
	ad9250_remove(ad9250_0_device);
	ad9250_remove(ad9250_1_device);
	gpio_remove(gpio_sysref);

	return SUCCESS;
}
