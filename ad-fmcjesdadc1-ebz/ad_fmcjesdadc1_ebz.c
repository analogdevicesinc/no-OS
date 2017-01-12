/***************************************************************************//**
 *   @file   ad_fmcjesdadc1_ebz.c
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
#include <xil_cache.h>
#include <xil_io.h>
#include "xparameters.h"
#include "platform_drivers.h"
#include "ad9517.h"
#include "ad9250.h"
#include "adxcvr_core.h"
#include "adc_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_TYPE				PS7_GPIO
#define GPIO_SYSREF				54 + 32
#define ADC_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0x800000
#else
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			0	// FIXME
#define GPIO_TYPE				AXI_GPIO
#define GPIO_SYSREF				0	// FIXME
#define ADC_DDR_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#endif
#define AD9250_ADXCVR_BASEADDR	XPAR_AXI_AD9250_XCVR_BASEADDR
#define AD9250_JESD_BASEADDR	XPAR_AXI_AD9250_JESD_BASEADDR

ad9517_init_param default_ad9517_init_param = {
	0,				// spi_chip_select
	SPI_MODE_0,		// spi_mode
#ifdef _XPARAMETERS_PS_H_
	PS7_SPI,		// spi_type
#else
	AXI_SPI,		// spi_type
#endif
	SPI_DEVICE_ID,	// spi_device_id
};

ad9250_init_param default_ad9250_init_param = {
	0,				// spi_chip_select
	SPI_MODE_0,		// spi_mode
#ifdef _XPARAMETERS_PS_H_
	PS7_SPI,		// spi_type
#else
	AXI_SPI,		// spi_type
#endif
	SPI_DEVICE_ID,	// spi_device_id
	0,				// id_no
};

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	ad9250_dev	*ad9250_0_device;
	ad9250_dev	*ad9250_1_device;
	ad9517_dev	*ad9517_device;
	adc_core	ad9250_0_core;
	adc_core	ad9250_1_core;
	jesd204_core	ad9250_jesd204;
	adxcvr_core	ad9250_xcvr;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ad9517_setup(&ad9517_device, default_ad9517_init_param);

	default_ad9250_init_param.id_no = 0;
	ad9250_setup(&ad9250_0_device, default_ad9250_init_param);

	default_ad9250_init_param.id_no = 1;
	ad9250_setup(&ad9250_1_device, default_ad9250_init_param);

	ad9250_jesd204.base_addr = AD9250_JESD_BASEADDR;
	ad9250_jesd204.rx_tx_n = 1;
	ad9250_jesd204.octets_per_frame = 1;
	ad9250_jesd204.frames_per_multiframe = 32;
	ad9250_jesd204.subclass_mode = 1;
	ad9250_jesd204.sysref_type = INTERN;
	ad9250_jesd204.gpio_device.device_id = GPIO_DEVICE_ID;
	ad9250_jesd204.gpio_device.type = PS7_GPIO;
	ad9250_jesd204.gpio_sysref = GPIO_SYSREF;

	ad9250_xcvr.base_addr = AD9250_ADXCVR_BASEADDR;
	ad9250_xcvr.tx_enable = 0;
	ad9250_xcvr.gth_enable = 0;
	ad9250_xcvr.lpm_enable = 0;
	ad9250_xcvr.out_clk_sel = 2;
	ad9250_xcvr.init_set_rate_enable = 0;

	jesd204_init(ad9250_jesd204);
	adxcvr_init(ad9250_xcvr);

	jesd204_gen_sysref(ad9250_jesd204);

	jesd204_read_status(ad9250_jesd204);

	ad9250_0_core.adc_baseaddr = XPAR_AXI_AD9250_0_CORE_BASEADDR;
	ad9250_0_core.dmac_baseaddr = XPAR_AXI_AD9250_0_DMA_BASEADDR;
	ad9250_0_core.no_of_channels = 2;
	ad9250_0_core.resolution = 14;

	ad9250_1_core.adc_baseaddr = XPAR_AXI_AD9250_1_CORE_BASEADDR;
	ad9250_1_core.dmac_baseaddr = XPAR_AXI_AD9250_1_DMA_BASEADDR;
	ad9250_1_core.no_of_channels = 2;
	ad9250_1_core.resolution = 14;

	adc_setup(ad9250_0_core);
	adc_setup(ad9250_1_core);

	ad9250_spi_write(ad9250_0_device, 0x0d, 0x0f);
	ad9250_spi_write(ad9250_0_device, 0xff, 0x01);

	ad9250_spi_write(ad9250_1_device, 0x0d, 0x0f);
	ad9250_spi_write(ad9250_1_device, 0xff, 0x01);

	adc_capture(ad9250_0_core, 32768, ADC_DDR_BASEADDR);
	adc_capture(ad9250_1_core, 32768, ADC_DDR_BASEADDR + 0x20000);

	xil_printf("Ramp capture done.\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	xil_printf("Done.\n");

	return 0;
}
