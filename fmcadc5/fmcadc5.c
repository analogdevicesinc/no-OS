/***************************************************************************//**
 *   @file   ad_fmcadc5_ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
#include "platform_drivers.h"
#include "ad9625.h"
#include "adc_core.h"
#include "xcvr_core.h"
#include "jesd_core.h"
#include "dmac_core.h"
#include "i5g.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define GPIO_JESD204_SYSREF	32
#define GPIO_RST_0			34
#define GPIO_PWDN_0			35
#define GPIO_RST_1			38
#define GPIO_PWDN_1			39
#define GPIO_IRQ_0			40
#define GPIO_FD_0			41
#define GPIO_IRQ_1			42
#define GPIO_FD_1			43
#define GPIO_PWR_GOOD		44
#define GPIO_SYSREF_DELAY	56

#define SYSREF_DELAY_BITS	5

int main(void)
{
	uint8_t pwr_good = 0;
	uint32_t rstatus = 0;

	struct ad9625_dev		 *ad9625_0_device;
	struct ad9625_dev		 *ad9625_1_device;
	struct ad9625_init_param ad9625_0_param;
	struct ad9625_init_param ad9625_1_param;
	struct s_i5g			 *i5g_core;
	struct s_i5g_init		 i5g_init_param;
	adc_core				 ad9625_0_core;
	adc_core				 ad9625_1_core;
	jesd_core				 ad9625_0_jesd;
	jesd_core				 ad9625_1_jesd;
	xcvr_core				 ad9625_0_xcvr;
	xcvr_core				 ad9625_1_xcvr;
	dmac_core				 ad9625_dma;
	dmac_xfer				 rx_xfer;
	gpio_desc				 *gpio_rst_0;
	gpio_desc				 *gpio_pwdn_0;
	gpio_desc				 *gpio_rst_1;
	gpio_desc				 *gpio_pwdn_1;
	gpio_desc				 *gpio_pwr_good;

	/* Device initialization parameters */
	ad9625_0_param.spi_init.chip_select = 0xe;
	ad9625_0_param.spi_init.cpha = 0;
	ad9625_0_param.spi_init.cpol = 0;
	ad9625_0_param.spi_init.type = MICROBLAZE_SPI;
	ad9625_1_param.spi_init.chip_select = 0xd;
	ad9625_1_param.spi_init.cpha = 0;
	ad9625_1_param.spi_init.cpol = 0;
	ad9625_1_param.spi_init.type = MICROBLAZE_SPI;

	/* Base addresses */
	ad9625_0_xcvr.base_address = XPAR_AXI_AD9625_0_XCVR_BASEADDR;
	ad9625_0_jesd.base_address = XPAR_AXI_AD9625_0_JESD_RX_AXI_BASEADDR;
	ad9625_0_core.base_address = XPAR_AXI_AD9625_0_CORE_BASEADDR;

	ad9625_1_xcvr.base_address = XPAR_AXI_AD9625_1_XCVR_BASEADDR;
	ad9625_1_jesd.base_address = XPAR_AXI_AD9625_1_JESD_RX_AXI_BASEADDR;
	ad9625_1_core.base_address = XPAR_AXI_AD9625_1_CORE_BASEADDR;

	ad9625_dma.base_address = XPAR_AXI_AD9625_DMA_BASEADDR;

#ifdef ZYNQ
	rx_xfer.start_address = XPAR_DDR_MEM_BASEADDR + 0x800000;
#endif

#ifdef MICROBLAZE
	rx_xfer.start_address = XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000;
#endif

	gpio_get(&gpio_rst_0, GPIO_RST_0);
	gpio_get(&gpio_pwdn_0, GPIO_PWDN_0);
	gpio_get(&gpio_rst_1, GPIO_RST_1);
	gpio_get(&gpio_pwdn_1, GPIO_PWDN_1);
	gpio_get(&gpio_pwr_good, GPIO_PWR_GOOD);

	gpio_set_value(gpio_rst_0, 0); /* Reset active low */
	gpio_set_value(gpio_pwdn_0, 0); /* Power down active high */
	gpio_set_value(gpio_rst_1, 0); /* Reset active low */
	gpio_set_value(gpio_pwdn_1, 0); /* Power down active high */

	gpio_get_value(gpio_pwr_good, &pwr_good);
	if (!pwr_good) {
		xil_printf("Error: GPIO Power Good NOT set.\n\r");
		return -1;
    }
	gpio_set_value(gpio_rst_0, 1);
	gpio_set_value(gpio_rst_1, 1);
        	mdelay(100);

	/* ADC and receive path configuration */
	ad9625_0_param.lane_rate_kbps = 6250000;
	ad9625_0_param.test_samples[0] = 0x5A5;
	ad9625_0_param.test_samples[1] = 0x1E1;
	ad9625_0_param.test_samples[2] = 0x777;
	ad9625_0_param.test_samples[3] = 0x444;

	ad9625_1_param.lane_rate_kbps = 6250000;
	ad9625_1_param.test_samples[0] = 0x5A5;
	ad9625_1_param.test_samples[1] = 0x1E1;
	ad9625_1_param.test_samples[2] = 0x777;
	ad9625_1_param.test_samples[3] = 0x444;

	xcvr_getconfig(&ad9625_0_xcvr);
	ad9625_0_xcvr.reconfig_bypass = 1;
	ad9625_0_xcvr.lane_rate_kbps = ad9625_0_param.lane_rate_kbps;
	ad9625_0_xcvr.ref_clock_khz = 625000;

	xcvr_getconfig(&ad9625_1_xcvr);
	ad9625_1_xcvr.reconfig_bypass = 1;
	ad9625_1_xcvr.lane_rate_kbps = ad9625_1_param.lane_rate_kbps;
	ad9625_1_xcvr.ref_clock_khz = 625000;

	ad9625_0_jesd.scramble_enable = 1;
	ad9625_0_jesd.octets_per_frame = 1;
	ad9625_0_jesd.frames_per_multiframe = 32;
	ad9625_0_jesd.subclass_mode = 1;
	ad9625_0_jesd.sysref_type = INTERN;
	ad9625_0_jesd.sysref_gpio_pin = GPIO_JESD204_SYSREF;

	ad9625_1_jesd.scramble_enable = 1;
	ad9625_1_jesd.octets_per_frame = 1;
	ad9625_1_jesd.frames_per_multiframe = 32;
	ad9625_1_jesd.subclass_mode = 1;
	ad9625_1_jesd.sysref_type = INTERN;
	ad9625_1_jesd.sysref_gpio_pin = GPIO_JESD204_SYSREF;

	ad9625_0_core.no_of_channels = 1;
	ad9625_0_core.resolution = 12;

	ad9625_1_core.no_of_channels = 1;
	ad9625_1_core.resolution = 12;

	ad9625_dma.type = DMAC_RX;
	ad9625_dma.transfer = &rx_xfer;
	rx_xfer.id = 0;
	rx_xfer.no_of_samples = 32768;

	ad_platform_init();

	/* Set up the device */
	ad9625_setup(&ad9625_0_device, ad9625_0_param);
	ad9625_setup(&ad9625_1_device, ad9625_1_param);

	i5g_init_param.ad9625_0_device = ad9625_0_device;
	i5g_init_param.ad9625_1_device = ad9625_1_device;
	i5g_init_param.ad9625_cs_0 = 1;
	i5g_init_param.ad9625_cs_1 = 2;
	i5g_init_param.regs = XPAR_AXI_FMCADC5_SYNC_BASEADDR;

	/* Set up the JESD core */
	jesd_setup(&ad9625_0_jesd);
	jesd_setup(&ad9625_1_jesd);

	/* Set up the XCVRs */
	xcvr_setup(&ad9625_0_xcvr);
	xcvr_setup(&ad9625_1_xcvr);

	/* Interface core setup */
	adc_setup(ad9625_0_core);
	adc_setup(ad9625_1_core);

	/* JESD core status */
	axi_jesd204_rx_status_read(&ad9625_0_jesd);
	axi_jesd204_rx_status_read(&ad9625_1_jesd);

	i5g_setup(&i5g_core, i5g_init_param);

	/* JESD core status */
	axi_jesd204_rx_status_read(&ad9625_0_jesd);
	axi_jesd204_rx_status_read(&ad9625_1_jesd);

	/* PRBS test */
	ad9625_test(ad9625_0_device, AD9625_TEST_PNLONG);
	rstatus = adc_pn_mon(ad9625_0_core, ADC_PN23);
	printf("PNLONG 0 : %d\n", (int)rstatus);
	ad9625_test(ad9625_1_device, AD9625_TEST_PNLONG);
	rstatus = adc_pn_mon(ad9625_1_core, ADC_PN23);
	printf("PNLONG 1 : %d\n", (int)rstatus);

	/* Set up ramp output */
	ad9625_test(ad9625_0_device, AD9625_TEST_RAMP);
	ad9625_test(ad9625_1_device, AD9625_TEST_RAMP);

	/* Test the captured data */
	if(!dmac_start_transaction(ad9625_dma)) {
		adc_ramp_test(ad9625_0_core, 2, 1024, rx_xfer.start_address);
	};
	if(!dmac_start_transaction(ad9625_dma)) {
		adc_ramp_test(ad9625_1_core, 2, 1024, rx_xfer.start_address);
	};

	/* Set up normal output */
	ad9625_test(ad9625_0_device, AD9625_TEST_OFF);
	ad9625_test(ad9625_1_device, AD9625_TEST_OFF);

	/* Capture data with DMA */
	if(!dmac_start_transaction(ad9625_dma)) {
		ad_printf("%s RX capture done!\n", __func__);
	};

	ad9625_remove(ad9625_0_device);
	ad9625_remove(ad9625_1_device);

	gpio_remove(gpio_rst_0);
	gpio_remove(gpio_pwdn_0);
	gpio_remove(gpio_rst_1);
	gpio_remove(gpio_pwdn_1);
	gpio_remove(gpio_pwr_good);

	i5g_remove(i5g_core);

	ad_platform_close();

	printf("Done\n");

	return 0;
}
