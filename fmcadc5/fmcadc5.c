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

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define GPIO_JESD204_SYSREF		32
#define GPIO_RST_0			34
#define GPIO_PWDN_0			35
#define GPIO_RST_1			38
#define GPIO_PWDN_1			39
#define GPIO_IRQ_0			40
#define GPIO_FD_0			41
#define GPIO_IRQ_1			42
#define GPIO_FD_1			43
#define GPIO_PWR_GOOD			44
#define GPIO_VDITHER			45
#define GPIO_TRIG			46
#define GPIO_SYSREF_DELAY		56

#define SYSREF_DELAY_BITS		5

int main(void)
{
	uint8_t pwr_good = 0;
	uint32_t n = 0;
	uint32_t rdata = 0;
	uint32_t rstatus = 0;
	uint8_t rdata8_0 = 0;
	uint8_t rdata8_1 = 0;

	spi_device			ad9625_0_spi_device;
	spi_device			ad9625_1_spi_device;
	spi_device			dac_sync_0_spi_device;
	spi_device			dac_sync_1_spi_device;
	adc_core			ad9625_0_core;
	adc_core			ad9625_1_core;
	ad9625_init_param		ad9625_0_param;
	ad9625_init_param		ad9625_1_param;
	jesd_core			ad9625_0_jesd;
	jesd_core			ad9625_1_jesd;
	xcvr_core			ad9625_0_xcvr;
	xcvr_core			ad9625_1_xcvr;
	dmac_core			ad9625_dma;
	dmac_xfer			rx_xfer;

	// base addresses
	ad9625_0_xcvr.base_address = XPAR_AXI_AD9625_0_XCVR_BASEADDR;
	ad9625_0_jesd.base_address = XPAR_AXI_AD9625_0_JESD_BASEADDR;
	ad9625_0_core.base_address = XPAR_AXI_AD9625_0_CORE_BASEADDR;

	ad9625_1_xcvr.base_address = XPAR_AXI_AD9625_1_XCVR_BASEADDR;
	ad9625_1_jesd.base_address = XPAR_AXI_AD9625_1_JESD_BASEADDR;
	ad9625_1_core.base_address = XPAR_AXI_AD9625_1_CORE_BASEADDR;

	ad9625_dma.base_address = XPAR_AXI_AD9625_DMA_BASEADDR;

#ifdef ZYNQ
	rx_xfer.start_address = XPAR_DDR_MEM_BASEADDR + 0x800000;
#endif

#ifdef MICROBLAZE
	rx_xfer.start_address = XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000;
#endif

	ad_gpio_set(GPIO_RST_0, 0); // reset active low
	ad_gpio_set(GPIO_PWDN_0, 0); // power down active high
	ad_gpio_set(GPIO_RST_1, 0); // reset active low
	ad_gpio_set(GPIO_PWDN_1, 0); // power down active high

	ad_gpio_get(GPIO_PWR_GOOD, &pwr_good);
	if (!pwr_good) {
		xil_printf("Error: GPIO Power Good NOT set.\n\r");
		return -1;
        }
	ad_gpio_set(GPIO_RST_0, 1);
	ad_gpio_set(GPIO_RST_1, 1);
        	mdelay(100);

	// SPI configuration
	ad_spi_init(&ad9625_0_spi_device);
	ad9625_0_spi_device.chip_select = 0xe;
	ad_spi_init(&ad9625_1_spi_device);
	ad9625_1_spi_device.chip_select = 0xd;

	// ADC and receive path configuration
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
	ad9625_0_xcvr.mmcm_present = 0;
	ad9625_0_xcvr.reconfig_bypass = 1;
	ad9625_0_xcvr.lane_rate_kbps = ad9625_0_param.lane_rate_kbps;
	ad9625_0_xcvr.ref_clock_khz = 625000;

	xcvr_getconfig(&ad9625_1_xcvr);
	ad9625_1_xcvr.mmcm_present = 0;
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
	rx_xfer.size = 32768;

	ad_platform_init();

	// set up the device
	ad9625_setup(&ad9625_0_spi_device);
	ad9625_setup(&ad9625_1_spi_device);

	// set up the JESD core
	jesd_setup(ad9625_0_jesd);
	jesd_setup(ad9625_1_jesd);

	// set up the XCVRs
	xcvr_setup(ad9625_0_xcvr);
	xcvr_setup(ad9625_1_xcvr);

	// generate SYSREF
	jesd_sysref_control(ad9625_0_jesd, 1);
	jesd_sysref_control(ad9625_1_jesd, 1);

	// JESD core status
	jesd_status(ad9625_0_jesd);
	jesd_status(ad9625_1_jesd);

	// Synchronize the two AD9625s
	ad9625_spi_write(&ad9625_0_spi_device, AD9625_REG_JESD204B_CONFIGURATION, 0x8b); // CS - overrange + sysref time-stamp. (default is 0x0b)
	ad9625_spi_write(&ad9625_0_spi_device, AD9625_REG_SYSREF_CONTROL, 0x02); // Sysref enabled (default is 0x00)
	ad9625_spi_write(&ad9625_0_spi_device, AD9625_REG_TRANSFER, 0x01); // Register update
	ad9625_spi_write(&ad9625_1_spi_device, AD9625_REG_JESD204B_CONFIGURATION, 0x8b); // CS - overrange + sysref time-stamp. (default is 0x0b)
	ad9625_spi_write(&ad9625_1_spi_device, AD9625_REG_SYSREF_CONTROL, 0x02); // Sysref enabled (default is 0x00)
	ad9625_spi_write(&ad9625_1_spi_device, AD9625_REG_TRANSFER, 0x01); // Register update

	ad9625_spi_write(&ad9625_0_spi_device, AD9625_REG_SYSREF_SETUP_TIME_GUARDBAND, 0x60);
	ad9625_spi_write(&ad9625_0_spi_device, AD9625_REG_TRANSFER, 0x01);

	ad_gpio_set_range(GPIO_SYSREF_DELAY, SYSREF_DELAY_BITS, 0x1f);

	for (n = 0; n < 32; n++) {

		jesd_sysref_control(ad9625_0_jesd, 0);
		xcvr_reset(ad9625_0_xcvr);
		xcvr_reset(ad9625_1_xcvr);

		ad9625_spi_write(&ad9625_0_spi_device, AD9625_REG_SYSREF_CONTROL, 0x42);
		ad9625_spi_write(&ad9625_0_spi_device, AD9625_REG_TRANSFER, 0x01);
		ad9625_spi_write(&ad9625_0_spi_device, AD9625_REG_SYSREF_CONTROL, 0x02);
		ad9625_spi_write(&ad9625_0_spi_device, AD9625_REG_TRANSFER, 0x01);

		ad9625_spi_write(&ad9625_1_spi_device, AD9625_REG_SYSREF_CONTROL, 0x42);
		ad9625_spi_write(&ad9625_1_spi_device, AD9625_REG_TRANSFER, 0x01);
		ad9625_spi_write(&ad9625_1_spi_device, AD9625_REG_SYSREF_CONTROL, 0x02);
		ad9625_spi_write(&ad9625_1_spi_device, AD9625_REG_TRANSFER, 0x01);

		ad_gpio_set_range(GPIO_SYSREF_DELAY, SYSREF_DELAY_BITS, n);
		ad_gpio_get_range(GPIO_SYSREF_DELAY, SYSREF_DELAY_BITS, &rdata);
		if (rdata  != n ) {
				xil_printf("[%05d]: SYSREF Delay Control Failed(%d, %d), Exiting!!\n", __LINE__, n, rdata);
				return (-1) ;
		}

		ad9625_spi_read(&ad9625_0_spi_device, AD9625_REG_IRQ_STATUS, &rdata8_0);
		ad9625_spi_read(&ad9625_1_spi_device, AD9625_REG_IRQ_STATUS, &rdata8_1);

		if (((rdata8_0 & 0x04) != 0x00) || ((rdata8_1 & 0x04) != 0x00)) {
				xil_printf("[%05d]: SYSREF Clear Status Failed(%d, 0x%02x, 0x%02x), Exiting!!\n", __LINE__, n, rdata8_0, rdata8_1);
		}

		jesd_sysref_control(ad9625_0_jesd, 1);

		ad9625_spi_read(&ad9625_0_spi_device, AD9625_REG_IRQ_STATUS, &rdata8_0);
		ad9625_spi_read(&ad9625_1_spi_device, AD9625_REG_IRQ_STATUS, &rdata8_1);

		if ((rstatus == 1) && ((rdata8_0 & 0x04) == 0x04) && ((rdata8_1 & 0x04) == 0x00)) {
				rstatus = 2;
				xil_printf("SYSREF Calibration Successful[%d]\n", n);
				break;
		}

		rstatus = 0;
		if (((rdata8_0 & 0x04) == 0x00) && ((rdata8_1 & 0x04) == 0x00)) {
				rstatus = 1;
		}
	}

	if (rstatus != 2) {
		xil_printf("SYSREF Calibration Failed!!\n");
	}
	// interface core setup
	adc_setup(ad9625_0_core);
	adc_setup(ad9625_1_core);

	// PRBS test
	ad9625_test(&ad9625_0_spi_device, AD9625_TEST_PNLONG);
	adc_pn_mon(ad9625_0_core, ADC_PN23);
	ad9625_test(&ad9625_1_spi_device, AD9625_TEST_PNLONG);
	adc_pn_mon(ad9625_1_core, ADC_PN23);

	// set up ramp output
	ad9625_test(&ad9625_0_spi_device, AD9625_TEST_RAMP);
	ad9625_test(&ad9625_1_spi_device, AD9625_TEST_RAMP);

	// test the captured data
	if(!dmac_start_transaction(ad9625_dma)) {
		adc_ramp_test(ad9625_0_core, 2, 1024, rx_xfer.start_address);
	};
	if(!dmac_start_transaction(ad9625_dma)) {
		adc_ramp_test(ad9625_1_core, 2, 1024, rx_xfer.start_address);
	};

	// set up normal output
	ad9625_test(&ad9625_0_spi_device, AD9625_TEST_OFF);
	ad9625_test(&ad9625_1_spi_device, AD9625_TEST_OFF);

	// capture data with DMA
	if(!dmac_start_transaction(ad9625_dma)) {
		ad_printf("%s RX capture done!\n", __func__);
	};

	ad_platform_close();

	return(0);
}

