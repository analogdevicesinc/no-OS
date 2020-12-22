/***************************************************************************//**
 *   @file   ad6676_ebz.c
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
#include <inttypes.h>
#include "parameters.h"
#include <xparameters.h>
#include <xil_printf.h>
#include <xil_cache.h>
#include "spi.h"
#include "gpio.h"
#include "spi_extra.h"
#include "gpio_extra.h"
#include "delay.h"
#include "error.h"
#include "ad6676.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "axi_jesd204_rx.h"
#include "axi_adxcvr.h"

#define LOG_LEVEL 6
#include "print_log.h"

#ifdef IIO_SUPPORT
#include "app_iio.h"
#endif

/***************************************************************************//**
* @brief ad6676_gpio_config
*******************************************************************************/

int32_t ad6676_gpio_config(struct ad6676_init_param init_param)
{
	int32_t ret;

	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};

	struct gpio_init_param gpio_adc_oen_param = {
		.number = GPIO_ADC_OEN,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_desc *gpio_adc_oen;

	struct gpio_init_param gpio_adc_s0_param = {
		.number = GPIO_ADC_S0,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_desc *gpio_adc_s0;

	struct gpio_init_param gpio_adc_s1_param = {
		.number = GPIO_ADC_S1,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_desc *gpio_adc_s1;

	struct gpio_init_param gpio_adc_sela_param = {
		.number = GPIO_ADC_SELA,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_desc *gpio_adc_sela;

	struct gpio_init_param gpio_adc_selb_param = {
		.number = GPIO_ADC_SELB,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_desc *gpio_adc_selb;

	struct gpio_init_param gpio_adc_resetb_param = {
		.number = GPIO_ADC_RESETB,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_desc *gpio_adc_resetb;

	struct gpio_init_param gpio_adc_agc1_param = {
		.number = GPIO_ADC_AGC1,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_desc *gpio_adc_agc1;

	struct gpio_init_param gpio_adc_agc2_param = {
		.number = GPIO_ADC_AGC2,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_desc *gpio_adc_agc2;

	ret = gpio_get(&gpio_adc_oen, &gpio_adc_oen_param);
	if(ret != SUCCESS)
		return FAILURE;

	ret = gpio_get(&gpio_adc_s0, &gpio_adc_s0_param);
	if(ret != SUCCESS)
		return FAILURE;

	ret = gpio_get(&gpio_adc_s1, &gpio_adc_s1_param);
	if(ret != SUCCESS)
		return FAILURE;

	ret = gpio_get(&gpio_adc_sela, &gpio_adc_sela_param);
	if(ret != SUCCESS)
		return FAILURE;

	ret = gpio_get(&gpio_adc_selb, &gpio_adc_selb_param);
	if(ret != SUCCESS)
		return FAILURE;

	ret = gpio_get(&gpio_adc_resetb, &gpio_adc_resetb_param);
	if(ret != SUCCESS)
		return FAILURE;

	ret = gpio_get(&gpio_adc_agc1, &gpio_adc_agc1_param);
	if(ret != SUCCESS)
		return FAILURE;

	ret = gpio_get(&gpio_adc_agc2, &gpio_adc_agc2_param);
	if(ret != SUCCESS)
		return FAILURE;

	ret = gpio_direction_output(gpio_adc_oen, 0);
	if(ret != SUCCESS)
		return FAILURE;

	switch (init_param.decimation) {
	case 12:
		ret = gpio_direction_output(gpio_adc_s0, 1);
		if(ret != SUCCESS)
			return FAILURE;
		ret = gpio_direction_output(gpio_adc_s1, 1);
		if(ret != SUCCESS)
			return FAILURE;
		break;
	case 16:
		ret = gpio_direction_output(gpio_adc_s0, 0);
		if(ret != SUCCESS)
			return FAILURE;
		ret = gpio_direction_output(gpio_adc_s1, 1);
		if(ret != SUCCESS)
			return FAILURE;
		break;
	case 24:
		ret = gpio_direction_output(gpio_adc_s0, 0);
		if(ret != SUCCESS)
			return FAILURE;
		ret = gpio_direction_output(gpio_adc_s1, 1);
		if(ret != SUCCESS)
			return FAILURE;
		break;
	case 32:
		ret = gpio_direction_output(gpio_adc_s0, 0);
		if(ret != SUCCESS)
			return FAILURE;
		ret = gpio_direction_output(gpio_adc_s1, 0);
		if(ret != SUCCESS)
			return FAILURE;
		break;
	default:
		return FAILURE;
	}

	if (init_param.use_extclk) {
		ret = gpio_direction_output(gpio_adc_sela, 1);
		if(ret != SUCCESS)
			return FAILURE;
		ret = gpio_direction_output(gpio_adc_selb, 0);
		if(ret != SUCCESS)
			return FAILURE;
	} else {
		ret = gpio_direction_output(gpio_adc_sela, 0);
		if(ret != SUCCESS)
			return FAILURE;
		ret = gpio_direction_output(gpio_adc_selb, 1);
		if(ret != SUCCESS)
			return FAILURE;
	}

	ret = gpio_direction_output(gpio_adc_resetb, 1);
	if(ret != SUCCESS)
		return FAILURE;
	ret = gpio_direction_output(gpio_adc_agc1, 0);
	if(ret != SUCCESS)
		return FAILURE;
	ret = gpio_direction_output(gpio_adc_agc2, 0);
	if(ret != SUCCESS)
		return FAILURE;

	gpio_remove(gpio_adc_oen);
	gpio_remove(gpio_adc_s0);
	gpio_remove(gpio_adc_s1);
	gpio_remove(gpio_adc_sela);
	gpio_remove(gpio_adc_selb);
	gpio_remove(gpio_adc_resetb);
	gpio_remove(gpio_adc_agc1);
	gpio_remove(gpio_adc_agc2);

	return SUCCESS;
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	int32_t ret;

	struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};

	struct gpio_init_param gpio_sysref_param = {
		.number = GPIO_JESD204_SYSREF,
		.platform_ops = &xil_gpio_platform_ops,
		.extra = &xil_gpio_param
	};
	struct gpio_desc *gpio_sysref;

	struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.device_id = SPI_DEVICE_ID
	};

	struct spi_init_param ad6676_spi_param = {
		.max_speed_hz = 2000000u,
		.chip_select = 0,
		.mode = SPI_MODE_0,
		.platform_ops = &xil_platform_ops,
		.extra = &xil_spi_param
	};

	struct axi_adc_init ad6676_core_param = {
		.name = "ad6676_adc",
		.base = RX_CORE_BASEADDR,
		.num_channels = 2
	};
	struct axi_adc *ad6676_core;

	struct jesd204_rx_init  ad6676_jesd_param = {
		.name = "ad6676_jesd",
		.base = RX_JESD_BASEADDR,
		.octets_per_frame = 1,
		.frames_per_multiframe = 16,
		.subclass = 1,
		.device_clk_khz = 4000000 / 40,
		.lane_clk_khz = 4000000
	};
	struct axi_jesd204_rx *ad6676_jesd;

	struct adxcvr_init ad6676_xcvr_param = {
		.name = "ad6676_xcvr",
		.base = RX_XCVR_BASEADDR,
		.sys_clk_sel = 0,
		.out_clk_sel = 4,
		.lpm_enable = 0,
		.cpll_enable = 1,
		.ref_rate_khz = 200000,
		.lane_rate_khz = 4000000,
	};
	struct adxcvr *ad6676_xcvr;

	struct axi_dmac_init ad6676_dmac_param = {
		.name = "ad6676_dmac",
		.base = RX_DMA_BASEADDR,
		.direction = DMA_DEV_TO_MEM,
		.flags = 0
	};
	struct axi_dmac *ad6676_dmac;

	struct ad6676_dev		*ad6676_device;
	struct ad6676_init_param 	ad6676_param;

	ad6676_param.spi_init = ad6676_spi_param;
	ad6676_param.ref_clk = 200000000UL; // reference clk Hz
	ad6676_param.f_adc_hz = 3200000000UL; // adc frequency Hz
	ad6676_param.f_if_hz = 250000000UL; // intermediate frequency Hz
	ad6676_param.bw_hz = 75000000UL; // bandwidth Hz;
	ad6676_param.bw_margin_low_mhz = 5;
	ad6676_param.bw_margin_high_mhz = 5;
	ad6676_param.bw_margin_if_mhz = 0;
	ad6676_param.decimation = 16;
	ad6676_param.ext_l = 19; // external inductance l_nh
	ad6676_param.attenuation = 5;
	ad6676_param.scale = 64;
	ad6676_param.use_extclk = 0;
	ad6676_param.spi3wire = 0;
	// shuffle
	ad6676_param.shuffle_ctrl = 1;
	ad6676_param.shuffle_thresh = 5;
	// dev jesd
	ad6676_param.scrambling_en = 1; // jesd scrambling enable
	ad6676_param.lvds_syncb = 1; // jesd use lvds syncb enable
	ad6676_param.sysref_pd = 0; // jesd powerdown sysref enable
	ad6676_param.n_lanes = 2;
	ad6676_param.frames_per_multiframe = 16;

	// receiver DMA configuration
	ret = gpio_get(&gpio_sysref, &gpio_sysref_param);
	if(ret != SUCCESS)
		return FAILURE;

	ret = gpio_direction_output(gpio_sysref, 1);
	if(ret != SUCCESS)
		return FAILURE;

	// set up clock
	ret = ad6676_gpio_config(ad6676_param);
	if(ret != SUCCESS)
		return FAILURE;

	// set up the device
	ret = ad6676_setup(&ad6676_device, ad6676_param);
	if(ret != SUCCESS)
		return FAILURE;

	// set up the XCVR core
	ret = adxcvr_init(&ad6676_xcvr, &ad6676_xcvr_param);
	if (ret != SUCCESS) {
		pr_err("error: %s: adxcvr_init() failed\n", ad6676_xcvr->name);
	}

	ret = adxcvr_clk_enable(ad6676_xcvr);
	if (ret != SUCCESS) {
		pr_err("error: %s: adxcvr_clk_enable() failed\n", ad6676_xcvr->name);
	}

	// setup JESD core
	ret = axi_jesd204_rx_init(&ad6676_jesd, &ad6676_jesd_param);
	if (ret != SUCCESS) {
		pr_err("error: %s: axi_jesd204_rx_init() failed\n", ad6676_jesd->name);
	}

	ret = axi_jesd204_rx_lane_clk_enable(ad6676_jesd);
	if (ret != SUCCESS) {
		pr_err("error: %s: axi_jesd204_tx_lane_clk_enable() failed\n",
		       ad6676_jesd->name);
	}

	ret = axi_adc_init(&ad6676_core,  &ad6676_core_param);
	if (ret != SUCCESS) {
		pr_err("axi_adc_init() error: %s\n", ad6676_core->name);
	}

	// JESD core status
	ret = axi_jesd204_rx_status_read(ad6676_jesd);
	if (ret != SUCCESS) {
		pr_err("axi_jesd204_rx_status_read() error: %"PRIi32"\n", ret);
	}

	// PRBS test
	ad6676_test(ad6676_device, TESTGENMODE_PN9_SEQ);
	if(axi_adc_pn_mon(ad6676_core, AXI_ADC_PN9, 10) == -1) {
		pr_err("%s ad6676 - PN23 sequence mismatch!\n", __func__);
	};
	ad6676_test(ad6676_device, TESTGENMODE_PN23_SEQ);
	if(axi_adc_pn_mon(ad6676_core, AXI_ADC_PN23, 10) == -1) {
		pr_err("%s ad6676 - PN23 sequence mismatch!\n", __func__);
	};

	// set up ramp output
	ad6676_test(ad6676_device, TESTGENMODE_RAMP);

	// test the captured data
	axi_dmac_init(&ad6676_dmac, &ad6676_dmac_param);
	axi_dmac_transfer(ad6676_dmac, ADC_DDR_BASEADDR, 16384 * 2);

	// capture data with DMA
	ad6676_test(ad6676_device, TESTGENMODE_OFF);
	axi_dmac_transfer(ad6676_dmac, ADC_DDR_BASEADDR, 16384 * 2);

#ifdef IIO_SUPPORT
	printf("The board accepts libiio clients connections through the serial backend.\n");

	struct iio_axi_adc_init_param iio_axi_adc_init_par;
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = ad6676_core,
		.rx_dmac = ad6676_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};

	return iio_app_start(&iio_axi_adc_init_par);

#endif

	pr_info("Done.");

	return SUCCESS;
}
