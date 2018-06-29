/***************************************************************************//**
 *   @file   ad6676_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
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
#include "platform_drivers.h"
#include "ad6676.h"
#include "adc_core.h"
#include "xcvr_core.h"
#include "jesd_core.h"
#include "dmac_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define GPIO_ADC_OEN			41
#define GPIO_ADC_SELA			40
#define GPIO_ADC_SELB			39
#define GPIO_ADC_S0			38
#define GPIO_ADC_S1			37
#define GPIO_ADC_RESETB			36
#define GPIO_ADC_AGC1			35
#define GPIO_ADC_AGC2			34
#define GPIO_ADC_AGC3			33
#define GPIO_ADC_AGC4			32

#define GPIO_JESD204_SYSREF		48

/***************************************************************************//**
* @brief ad6676_gpio_config
*******************************************************************************/

int32_t ad6676_gpio_config(struct ad6676_init_param init_param)
{
	gpio_desc *gpio_adc_oen;
	gpio_desc *gpio_adc_s0;
	gpio_desc *gpio_adc_s1;
	gpio_desc *gpio_adc_sela;
	gpio_desc *gpio_adc_selb;
	gpio_desc *gpio_adc_resetb;
	gpio_desc *gpio_adc_agc1;
	gpio_desc *gpio_adc_agc2;

	gpio_get(&gpio_adc_oen, GPIO_ADC_OEN);
	gpio_get(&gpio_adc_s0, GPIO_ADC_S0);
	gpio_get(&gpio_adc_s1, GPIO_ADC_S1);
	gpio_get(&gpio_adc_sela, GPIO_ADC_SELA);
	gpio_get(&gpio_adc_selb, GPIO_ADC_SELB);
	gpio_get(&gpio_adc_resetb, GPIO_ADC_RESETB);
	gpio_get(&gpio_adc_agc1, GPIO_ADC_AGC1);
	gpio_get(&gpio_adc_agc2, GPIO_ADC_AGC2);

	gpio_set_value(gpio_adc_oen, 0);

	switch (init_param.decimation) {
	case 12:
		gpio_set_value(gpio_adc_s0, 1);
		gpio_set_value(gpio_adc_s1, 1);
		break;
	case 16:
		gpio_set_value(gpio_adc_s0, 0);
		gpio_set_value(gpio_adc_s1, 1);
		break;
	case 24:
		gpio_set_value(gpio_adc_s0, 0);
		gpio_set_value(gpio_adc_s1, 1);
		break;
	case 32:
		gpio_set_value(gpio_adc_s0, 0);
		gpio_set_value(gpio_adc_s1, 0);
		break;
	default:
		return -1;
	}

	if (init_param.use_extclk) {
		gpio_set_value(gpio_adc_sela, 1);
		gpio_set_value(gpio_adc_selb, 0);
	} else {
		gpio_set_value(gpio_adc_sela, 0);
		gpio_set_value(gpio_adc_selb, 1);
	}

	gpio_set_value(gpio_adc_resetb, 1);
	gpio_set_value(gpio_adc_agc1, 0);
	gpio_set_value(gpio_adc_agc2, 0);

	gpio_remove(gpio_adc_oen);
	gpio_remove(gpio_adc_s0);
	gpio_remove(gpio_adc_s1);
	gpio_remove(gpio_adc_sela);
	gpio_remove(gpio_adc_selb);
	gpio_remove(gpio_adc_resetb);
	gpio_remove(gpio_adc_agc1);
	gpio_remove(gpio_adc_agc2);

	return 0;
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	adc_core		ad6676_core;
	struct ad6676_init_param 	ad6676_param;
	jesd_core		ad6676_jesd;
	xcvr_core		ad6676_xcvr;
	struct ad6676_dev		*ad6676_device;
	dmac_core		ad6676_dma;
	dmac_xfer		rx_xfer;

	uint32_t		store_samples;

	// base addresses

	ad6676_param.spi_init.chip_select = 0x0;
	ad6676_param.spi_init.cpha = 0;
	ad6676_param.spi_init.cpol = 0;
	ad6676_param.spi_init.type = ZYNQ_PS7_SPI;

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

	// jesd_core settings

	ad6676_jesd.rx_tx_n = 1;
	ad6676_jesd.scramble_enable = 1;
	ad6676_jesd.octets_per_frame = 1;
	ad6676_jesd.frames_per_multiframe = 16;
	ad6676_jesd.subclass_mode = 1;
	ad6676_jesd.sysref_type = INTERN;
	ad6676_jesd.sysref_gpio_pin = GPIO_JESD204_SYSREF;

	// xcvr settings

	ad6676_xcvr.dev.lpm_enable = 0;
	ad6676_xcvr.dev.out_clk_sel = 4;
	ad6676_xcvr.dev.sys_clk_sel = 0;
	ad6676_xcvr.reconfig_bypass = 0;
	ad6676_xcvr.lane_rate_kbps = 4000000;
	ad6676_xcvr.ref_clock_khz = 200000;

	// adc settings

	ad6676_core.no_of_channels = 2;
	ad6676_core.resolution = 16;

	// receiver DMA configuration

#ifdef ZYNQ
	rx_xfer.start_address = XPAR_DDR_MEM_BASEADDR + 0x800000;
#endif
#ifdef MICROBLAZE
	rx_xfer.start_address = XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000;
#endif
	ad6676_dma.type = DMAC_RX;
	ad6676_dma.transfer = &rx_xfer;
	rx_xfer.id = 0;
	rx_xfer.no_of_samples = 32768;
#ifdef XILINX
	ad6676_xcvr.base_address = XPAR_AXI_AD6676_XCVR_BASEADDR;
	ad6676_jesd.base_address = XPAR_AXI_AD6676_JESD_RX_AXI_BASEADDR;
	ad6676_dma.base_address = XPAR_AXI_AD6676_DMA_BASEADDR;
	ad6676_core.base_address = XPAR_AXI_AD6676_CORE_BASEADDR;
#endif
	xcvr_getconfig(&ad6676_xcvr);

	// set up clock
	ad6676_gpio_config(ad6676_param);

	// set up the device
	ad6676_setup(&ad6676_device, ad6676_param);

	// set up the JESD core
	jesd_setup(&ad6676_jesd);

	// set up the XCVRs
	xcvr_setup(&ad6676_xcvr);

	// generate SYSREF
	jesd_sysref_control(&ad6676_jesd, 1);

	// JESD core status
	axi_jesd204_rx_status_read(&ad6676_jesd);

	// interface core setup
	adc_setup(ad6676_core);

	// PRBS test
	ad6676_test(ad6676_device, TESTGENMODE_PN9_SEQ);
	adc_pn_mon(ad6676_core, ADC_PN9);
	ad6676_test(ad6676_device, TESTGENMODE_PN23_SEQ);
	adc_pn_mon(ad6676_core, ADC_PN23A);

	// set up ramp output
	ad6676_test(ad6676_device, TESTGENMODE_RAMP);
	// test the captured data
	if(!dmac_start_transaction(ad6676_dma)) {
		store_samples = rx_xfer.no_of_samples/ad6676_core.no_of_channels;
		adc_ramp_test(ad6676_core, 1, store_samples, rx_xfer.start_address);
	};

	// capture data with DMA
	ad6676_test(ad6676_device, TESTGENMODE_OFF);
	if(!dmac_start_transaction(ad6676_dma)) {
		ad_printf("RX capture done!\n");
	};

	return 0;
}
