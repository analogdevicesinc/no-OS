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

#define GPIO_JESD204_SYSREF		34

int main(void)
{
	spi_device		ad9625_spi_device;
	adc_core		ad9625_core;
	ad9625_init_param	ad9625_param;
	jesd_core		ad9625_jesd;
	xcvr_core		ad9625_xcvr;
	dmac_core               ad9625_dma;
	dmac_xfer               rx_xfer;

	// base addresses

	ad9625_xcvr.base_address = XPAR_AXI_AD9625_XCVR_BASEADDR;
	ad9625_jesd.base_address = XPAR_AXI_AD9625_JESD_BASEADDR;
	ad9625_core.base_address = XPAR_AXI_AD9625_CORE_BASEADDR;
	ad9625_dma.base_address = XPAR_AXI_AD9625_DMA_BASEADDR;

#ifdef ZYNQ
	rx_xfer.start_address = XPAR_DDR_MEM_BASEADDR + 0x800000;
#endif

#ifdef MICROBLAZE
	rx_xfer.start_address = XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000;
#endif

	// SPI configuration

	ad_spi_init(&ad9625_spi_device);
	ad9625_spi_device.chip_select = 0x6;

	// ADC and receive path configuration

	ad9625_param.lane_rate_kbps = 6250000;
	ad9625_param.test_samples[0] = 0x5A5;
	ad9625_param.test_samples[1] = 0x1E1;
	ad9625_param.test_samples[2] = 0x777;
	ad9625_param.test_samples[3] = 0x444;

	xcvr_getconfig(&ad9625_xcvr);
	ad9625_xcvr.mmcm_present = 0;
	ad9625_xcvr.reconfig_bypass = 1;
	ad9625_xcvr.lane_rate_kbps = ad9625_param.lane_rate_kbps;
	ad9625_xcvr.ref_clock_khz = 625000;

	ad9625_jesd.scramble_enable = 1;
	ad9625_jesd.octets_per_frame = 1;
	ad9625_jesd.frames_per_multiframe = 32;
	ad9625_jesd.subclass_mode = 1;
	ad9625_jesd.sysref_type = INTERN;
	ad9625_jesd.sysref_gpio_pin = GPIO_JESD204_SYSREF;

	ad9625_core.no_of_channels = 1;
	ad9625_core.resolution = 12;

	ad9625_dma.type = DMAC_RX;
	ad9625_dma.transfer = &rx_xfer;
	rx_xfer.id = 0;
	rx_xfer.size = 32768;

	ad_platform_init();

	// set up the device
	ad9625_setup(&ad9625_spi_device);
	// set up the JESD core
	jesd_setup(ad9625_jesd);
	// set up the XCVRs
	xcvr_setup(ad9625_xcvr);
	// generate SYSREF
	jesd_sysref_control(ad9625_jesd, 1);
	// JESD core status
	jesd_status(ad9625_jesd);

	// interface core setup
	adc_setup(ad9625_core);

	// PRBS test
	ad9625_test(&ad9625_spi_device, AD9625_TEST_PNLONG);
	adc_pn_mon(ad9625_core, ADC_PN23);

	// set up ramp output
	ad9625_test(&ad9625_spi_device, AD9625_TEST_RAMP);
	// test the captured data
	if(!dmac_start_transaction(ad9625_dma)) {
		adc_ramp_test(ad9625_core, 1, 1024, rx_xfer.start_address);
	};

	// capture data with DMA
	ad9625_test(&ad9625_spi_device, AD9625_TEST_OFF);
	if(!dmac_start_transaction(ad9625_dma)) {
		ad_printf("%s RX capture done!\n", __func__);
	};

	ad_platform_close();

	return(0);
}
