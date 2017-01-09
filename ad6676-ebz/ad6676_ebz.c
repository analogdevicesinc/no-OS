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
#include <xparameters.h>
#include <xil_io.h>
#include "platform_drivers.h"
#include "ad6676.h"
#include "adc_core.h"
#include "adxcvr_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				54 + 32
#define ADC_DDR_BASEADDR		XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x800000
#else
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				32
#define ADC_DDR_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#endif
#define AD6676_CORE_BASEADDR	XPAR_AXI_AD6676_CORE_BASEADDR
#define AD6676_DMA_BASEADDR		XPAR_AXI_AD6676_DMA_BASEADDR
#define AD6676_JESD_BASEADDR	XPAR_AXI_AD6676_JESD_BASEADDR
#define AD6676_ADXCVR_BASEADDR  XPAR_AXI_AD6676_XCVR_BASEADDR
#define GPIO_ADC_OEN			GPIO_OFFSET + 9
#define GPIO_ADC_SELA			GPIO_OFFSET + 8
#define GPIO_ADC_SELB			GPIO_OFFSET + 7
#define GPIO_ADC_S0				GPIO_OFFSET + 6
#define GPIO_ADC_S1				GPIO_OFFSET + 5
#define GPIO_ADC_RESETB			GPIO_OFFSET + 4
#define GPIO_ADC_AGC1			GPIO_OFFSET + 3
#define GPIO_ADC_AGC2			GPIO_OFFSET + 2
#define GPIO_ADC_AGC3			GPIO_OFFSET + 1
#define GPIO_ADC_AGC4			GPIO_OFFSET + 0

#define GPIO_JESD204_SYSREF             GPIO_OFFSET + 16

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
ad6676_init_param default_init_param = {
	/* GPIO */
#ifdef _XPARAMETERS_PS_H_
	PS7_GPIO,		// gpio_type;
#else
	AXI_GPIO,		// gpio_type
#endif
	GPIO_DEVICE_ID,	// gpio_device_id
	GPIO_ADC_OEN,	// gpio_adc_oen
	GPIO_ADC_SELA,	// gpio_adc_sela
	GPIO_ADC_SELB,	// gpio_adc_selb
	GPIO_ADC_S0,	// gpio_adc_s0
	GPIO_ADC_S1,	// gpio_adc_s1
	GPIO_ADC_RESETB,	// gpio_adc_resetb
	GPIO_ADC_AGC1,	// gpio_adc_agc1
	GPIO_ADC_AGC2,	// gpio_adc_agc2
	GPIO_ADC_AGC3,	// gpio_adc_agc3
	GPIO_ADC_AGC4,	// gpio_adc_agc4
	/* SPI */
	0, 				// spi_chip_select
	SPI_MODE_0,		// spi_mode
#ifdef _XPARAMETERS_PS_H_
	PS7_SPI,		// spi_type
#else
	AXI_SPI,		// spi_type
#endif
	SPI_DEVICE_ID,	// spi_device_id
	/* Device Settings */
	200000000UL,	// reference_clk_rate
	0,				// spi_3_wire
	3200000000UL,	// adc_frequency_hz
	250000000UL,	// intermediate_frequency_hz
	MIN_FIF,		// intermediate_frequency_min_hz
	MAX_FIF,		// intermediate_frequency_max_hz
	75000000UL,		// bandwidth_hz
	5,				// bandwidth_margin_low_mhz
	5,				// bandwidth_margin_high_mhz
	0,				// bandwidth_margin_if_mhz
	16,				// decimation
	19,				// external_inductance_l_nh
	64,				// idac1_fullscale_adjust
	0,				// use_external_clk_enable
	1,				// adc_frequency_fixed_enable
	1,				// jesd_scrambling_enable
	1,				// jesd_use_lvds_syncb_enable
	0,				// jesd_powerdown_sysref_enable
	2,				// jesd_l_lanes
	16,				// jesd_f_frames_per_multiframe
	1,				// shuffler_control
	5,				// shuffler_thresh
};

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	adc_core		ad6676_core;
	ad6676_dev		*ad6676_device;
	jesd204_core	ad6676_jesd204;
	adxcvr_core		ad6676_xcvr;

	ad6676_jesd204.base_addr = AD6676_JESD_BASEADDR;
	ad6676_jesd204.rx_tx_n = 1;
	ad6676_jesd204.octets_per_frame = 1;
	ad6676_jesd204.frames_per_multiframe = 32;
	ad6676_jesd204.subclass_mode = 1;
	ad6676_jesd204.sysref_type = INTERN;
	ad6676_jesd204.gpio_device.device_id = default_init_param.gpio_device_id;
	ad6676_jesd204.gpio_device.type = default_init_param.gpio_type;
	ad6676_jesd204.gpio_sysref = GPIO_JESD204_SYSREF;

	ad6676_xcvr.base_addr = AD6676_ADXCVR_BASEADDR;
	ad6676_xcvr.tx_enable = 0;
	ad6676_xcvr.gth_enable = 0;
	ad6676_xcvr.lpm_enable = 0;
	ad6676_xcvr.out_clk_sel = 4;
	ad6676_xcvr.sys_clk_sel = 0;
	ad6676_xcvr.init_set_rate_enable = 0;
	ad6676_xcvr.lane_rate_khz = 4000000;
	ad6676_xcvr.ref_rate_khz = 200000;

	// set up the device
	ad6676_setup(&ad6676_device, default_init_param);

	ad6676_core.adc_baseaddr = AD6676_CORE_BASEADDR;
	ad6676_core.dmac_baseaddr = AD6676_DMA_BASEADDR;
	ad6676_core.no_of_channels = 2;
	ad6676_core.resolution = 16;

	// set up the JESD link and bring up the core
	jesd204_init(ad6676_jesd204);
	adxcvr_init(ad6676_xcvr);
	jesd204_gen_sysref(ad6676_jesd204);
	adc_setup(ad6676_core);

	// check JESD link status
	jesd204_read_status(ad6676_jesd204);

	/* Enable Ramp Test Mode */
	ad6676_spi_write(ad6676_device, AD6676_TEST_GEN, TESTGENMODE_RAMP);

	xil_printf("Start capturing data...\n\r");

	adc_capture(ad6676_core, 16384 * 2, ADC_DDR_BASEADDR);

	xil_printf("Done.\n\r");

	return 0;
}
