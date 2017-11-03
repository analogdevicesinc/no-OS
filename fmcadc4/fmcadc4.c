/***************************************************************************//**
 *   @file   ad_fmcadc4_ebz.c
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
#include "ad9528.h"
#include "ad9680.h"
#include "adc_core.h"
#include "adc_buffer.h"
#include "xcvr_core.h"
#include "jesd_core.h"
#include "dmac_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define GPIO_AD9528_RSTN		32
#define GPIO_AD9528_STATUS		33
#define GPIO_XCVR_RESET			38

/***************************************************************************//**
* @brief main
*******************************************************************************/

int main(void) {

	struct spi_device		ad9528_spi_device;
	struct spi_device		ad9680_0_spi_device;
	struct spi_device		ad9680_1_spi_device;
	struct ad9528_channel_spec	ad9528_channels[6];
	struct ad9528_platform_data	ad9528_param;
	struct ad9680_init_param	ad9680_0_param;
	struct ad9680_init_param	ad9680_1_param;
	struct xcvr_core_phy		fmcadc4_xcvr_phy;
	struct xcvr_core		fmcadc4_xcvr;
	struct jesd_core		fmcadc4_jesd;
	struct adc_core			ad9680_0_core;
	struct adc_core			ad9680_1_core;
	struct dmac_xfer		ad9680_dma_xfer;
	struct dmac_core		ad9680_dma;

	// base addresses

	fmcadc4_xcvr_phy.base_address = XPAR_AXI_FMCADC4_XCVR_BASEADDR;
	fmcadc4_jesd.base_address = XPAR_AXI_FMCADC4_JESD_RX_AXI_BASEADDR;
	ad9680_0_core.base_address = XPAR_AXI_AD9680_CORE_0_BASEADDR;
	ad9680_1_core.base_address = XPAR_AXI_AD9680_CORE_1_BASEADDR;
	ad9680_dma.base_address = XPAR_AXI_AD9680_DMA_BASEADDR;

	// spi settings

	ad_spi_init(&ad9528_spi_device);
	ad_spi_init(&ad9680_0_spi_device);
	ad_spi_init(&ad9680_1_spi_device);

	ad9528_spi_device.chip_select = 0x0;
	ad9680_0_spi_device.chip_select = 0x1;
	ad9680_1_spi_device.chip_select = 0x2;

	// ad9528 defaults

	ad9528_param.num_channels = 6;
	ad9528_param.channels = &ad9528_channels[0];
	ad9528_init(&ad9528_param);

	// fpga clock (500M) & sysref (31.25M)

	ad9528_channels[0].channel_num = 1;
	ad9528_channels[0].channel_divider = 2;
	ad9528_channels[1].channel_num = 13;
	ad9528_channels[1].channel_divider = 32;

	// adc1 clock (1000M) & sysref (31.25M)

	ad9528_channels[2].channel_num = 8;
	ad9528_channels[2].channel_divider = 1;
	ad9528_channels[3].channel_num = 9;
	ad9528_channels[3].channel_divider = 32;

	// adc2 clock (1000M) & sysref (31.25M)

	ad9528_channels[4].channel_num = 3;
	ad9528_channels[4].channel_divider = 1;
	ad9528_channels[5].channel_num = 2;
	ad9528_channels[5].channel_divider = 32;

	// pllx settings

	ad9528_param.spi3wire = 1;
	ad9528_param.vcxo_freq = 80000000;
	ad9528_param.osc_in_diff_en = 0;
	ad9528_param.osc_in_cmos_neg_inp_en = 1;
	ad9528_param.pll2_charge_pump_current_nA = 806000;
	ad9528_param.pll2_vco_diff_m1 = 4;
	ad9528_param.pll2_ndiv_a_cnt = 0;
	ad9528_param.pll2_ndiv_b_cnt = 25;
	ad9528_param.pll2_n2_div = 25;
	ad9528_param.pll2_r1_div = 2;
	ad9528_param.rpole2 = RPOLE2_900_OHM;
	ad9528_param.rzero= RZERO_3250_OHM;
	ad9528_param.cpole1 = CPOLE1_16_PF;

	// fs=1.24G (requires -3 device)

#ifdef ADC_FS_1240MSPS /* make DEFINE=ADC_FS_1240MSPS */
	ad9528_param.pll2_vco_diff_m1 = 3;
	ad9528_param.pll2_ndiv_a_cnt = 1;
	ad9528_param.pll2_ndiv_b_cnt = 23;
	ad9528_param.pll2_n2_div = 31;
#endif

	// device and peripherals settings

	ad9680_0_param.lane_rate_kbps = 10000000;
	ad9680_1_param.lane_rate_kbps = 10000000;

	fmcadc4_xcvr.gpio_reset = GPIO_XCVR_RESET;
	fmcadc4_xcvr.link_pll_present = 0;
	fmcadc4_xcvr.tx_or_rx_n = 0;
	fmcadc4_xcvr.no_of_phys = 1;
	fmcadc4_xcvr.phys = &fmcadc4_xcvr_phy;

	fmcadc4_jesd.tx_or_rx_n = 0;
	fmcadc4_jesd.octets_per_frame = 1;
	fmcadc4_jesd.frames_per_multiframe = 32;

	ad9680_0_core.no_of_channels = 2;
	ad9680_0_core.resolution = 14;
	ad9680_1_core.no_of_channels = 2;
	ad9680_1_core.resolution = 14;

	ad9680_dma.transfer = &ad9680_dma_xfer;
	dmac_init(&ad9680_dma, DMAC_RX);

	// main

	ad_platform_init();
	ad_gpio_set(GPIO_AD9528_STATUS, 0x0);
	ad_gpio_set(GPIO_AD9528_RSTN, 0x0);
	ad_gpio_set(GPIO_XCVR_RESET, 1);
	ad_gpio_set(GPIO_AD9528_RSTN, 0x1);
	mdelay(1);

	ad9528_setup(&ad9528_spi_device, &ad9528_param);
	ad9680_setup(&ad9680_0_spi_device, &ad9680_0_param);
	ad9680_setup(&ad9680_1_spi_device, &ad9680_1_param);

	jesd_setup(&fmcadc4_jesd);
	xcvr_setup(&fmcadc4_xcvr);
	mdelay(10);

	xcvr_status(&fmcadc4_xcvr);
	jesd_status(&fmcadc4_jesd);
	adc_setup(&ad9680_0_core);
	adc_setup(&ad9680_1_core);

	ad9680_test(&ad9680_0_spi_device, AD9680_TEST_PN9);
	ad9680_test(&ad9680_1_spi_device, AD9680_TEST_PN23);
	adc_pn_mon(&ad9680_0_core, ADC_PN9);
	adc_pn_mon(&ad9680_1_core, ADC_PN23A);

	ad9680_test(&ad9680_0_spi_device, AD9680_TEST_PN23);
	ad9680_test(&ad9680_1_spi_device, AD9680_TEST_PN9);
	adc_pn_mon(&ad9680_0_core, ADC_PN23A);
	adc_pn_mon(&ad9680_1_core, ADC_PN9);

	// dma

	ad9680_test(&ad9680_0_spi_device, AD9680_TEST_RAMP);
	ad9680_test(&ad9680_1_spi_device, AD9680_TEST_RAMP);
	dmac_start_transaction(&ad9680_dma);
	adc_ramp_test(&ad9680_dma_xfer, 4, 14);

	ad9680_test(&ad9680_0_spi_device, AD9680_TEST_OFF);
	ad9680_test(&ad9680_1_spi_device, AD9680_TEST_OFF);
	dmac_start_transaction(&ad9680_dma);

	ad_printf("done.\n");
	ad_platform_close();
	return(0);
}
