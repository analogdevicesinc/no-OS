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
#include <xparameters.h>
#include <xil_cache.h>
#include <xil_io.h>
#include "platform_drivers.h"
#include "ad9625.h"
#include "adc_core.h"
#include "adxcvr_core.h"

#ifdef _XPARAMETERS_PS_H_
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_TYPE			PS7_GPIO
#define GPIO_JESD204_SYSREF		54 + 34
#define ADC_DDR_BASEADDR		XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x800000
#else
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#endif
#define AD9625_CORE_BASEADDR		XPAR_AXI_AD9625_CORE_BASEADDR
#define AD9625_DMA_BASEADDR		XPAR_AXI_AD9625_DMA_BASEADDR
#define AD9625_JESD_BASEADDR		XPAR_AXI_AD9625_JESD_BASEADDR
#define AD9625_ADXCVR_BASEADDR		XPAR_AXI_AD9625_XCVR_BASEADDR

ad9625_init_param default_ad9625_init_param = {
	0,				// spi_chip_select
	SPI_MODE_3,		// spi_mode
#ifdef _XPARAMETERS_PS_H_
	PS7_SPI,		// spi_type
#else
	AXI_SPI,		// spi_type
#endif
	SPI_DEVICE_ID,	// spi_device_id;
};

int main(void)
{
	adc_core	ad9625_core;
	ad9625_dev	*ad9625_device;
	jesd204_core	ad9625_jesd204;
	adxcvr_core	ad9625_xcvr;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ad9625_setup(&ad9625_device, default_ad9625_init_param);

	ad9625_jesd204.base_addr = AD9625_JESD_BASEADDR;
	ad9625_jesd204.rx_tx_n = 1;
	ad9625_jesd204.octets_per_frame = 1;
	ad9625_jesd204.frames_per_multiframe = 32;
	ad9625_jesd204.subclass_mode = 1;
	ad9625_jesd204.sys_ref = INTERN;
	ad9625_jesd204.gpio_device.device_id = GPIO_DEVICE_ID;
	ad9625_jesd204.gpio_device.type = GPIO_TYPE;
	ad9625_jesd204.gpio_sysref = GPIO_JESD204_SYSREF;

	ad9625_xcvr.base_addr = AD9625_ADXCVR_BASEADDR;
	ad9625_xcvr.tx_enable = 0;
	ad9625_xcvr.gth_enable = 0;
	ad9625_xcvr.lpm_enable = 1;
	ad9625_xcvr.out_clk_sel = 2;
	ad9625_xcvr.sys_clk_sel = 0;

	ad9625_xcvr.lane_rate_khz = 6250000;
	ad9625_xcvr.ref_rate_khz = 625000;

	ad9625_spi_write(ad9625_device, AD9625_REG_TEST_CNTRL, 0x0F);
	ad9625_spi_write(ad9625_device, AD9625_REG_OUTPUT_MODE, 0x01);
	ad9625_spi_write(ad9625_device, AD9625_REG_TRANSFER, 0x01);

	ad9625_core.adc_baseaddr = AD9625_CORE_BASEADDR;
	ad9625_core.dmac_baseaddr = AD9625_DMA_BASEADDR;
	ad9625_core.no_of_channels = 1;
	ad9625_core.resolution = 12;

	jesd204_init(ad9625_jesd204);
	adxcvr_init(ad9625_xcvr);
	jesd204_gen_sysref(ad9625_jesd204);
	adc_setup(ad9625_core);

	// check JESD link status
	jesd204_read_status(ad9625_jesd204);

	xil_printf("Start capturing data...\n\r");

	adc_capture(ad9625_core, 16384, ADC_DDR_BASEADDR);

	xil_printf("Done.\n\r");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
