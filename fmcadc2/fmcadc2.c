/***************************************************************************//**
 *   @file   ad_fmcadc2_ebz.c
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

#define GPIO_XCVR_RESET	38
#define GPIO_JESD204_SYSREF 34

int main(void) {

	struct spi_device	ad9625_spi_device;
	struct xcvr_core_phy	ad9625_xcvr_phy;
	struct xcvr_core	ad9625_xcvr;
	struct jesd_core	ad9625_jesd;
	struct adc_core		ad9625_core;
	struct dmac_xfer	ad9625_dma_xfer;
	struct dmac_core	ad9625_dma;

	// base addresses

	ad9625_xcvr_phy.base_address = XPAR_AXI_AD9625_XCVR_BASEADDR;
	ad9625_core.base_address = XPAR_AXI_AD9625_CORE_BASEADDR;
	ad9625_jesd.base_address = XPAR_AXI_AD9625_JESD_RX_AXI_BASEADDR;
	ad9625_dma.base_address = XPAR_AXI_AD9625_DMA_BASEADDR;

	// spi configuration

	ad_spi_init(&ad9625_spi_device);
	ad9625_spi_device.chip_select = 0x6;

	// phy, jesd-ip, core & dma

	ad9625_xcvr.gpio_reset = GPIO_XCVR_RESET;
	ad9625_xcvr.link_pll_present = 0;
	ad9625_xcvr.tx_or_rx_n = 0;
	ad9625_xcvr.no_of_phys = 1;
	ad9625_xcvr.phys = &ad9625_xcvr_phy;

	ad9625_jesd.tx_or_rx_n = 0;
	ad9625_jesd.octets_per_frame = 1;
	ad9625_jesd.frames_per_multiframe = 32;
	ad9625_jesd.sysref_type = INTERN;
	ad9625_jesd.sysref_gpio_pin = GPIO_JESD204_SYSREF;

	ad9625_core.no_of_channels = 1;
	ad9625_core.resolution = 12;

	ad9625_dma.transfer = &ad9625_dma_xfer;
	dmac_init(&ad9625_dma, DMAC_RX);

	// main

	ad_platform_init();
	ad_gpio_set(GPIO_XCVR_RESET, 1);

	ad9625_setup(&ad9625_spi_device);
	jesd_setup(&ad9625_jesd);
	xcvr_setup(&ad9625_xcvr);
	jesd_sysref_control(&ad9625_jesd);
	xcvr_status(&ad9625_xcvr);
	jesd_status(&ad9625_jesd);
	adc_setup(&ad9625_core);

	// data-path

	ad9625_test(&ad9625_spi_device, AD9625_TEST_PNLONG);
	adc_pn_mon(&ad9625_core, ADC_PN23);

	// dma

	ad9625_test(&ad9625_spi_device, AD9625_TEST_OFF);
	dmac_start_transaction(&ad9625_dma);
	ad_printf("done.\n");
	ad_platform_close();
	return(0);
}
