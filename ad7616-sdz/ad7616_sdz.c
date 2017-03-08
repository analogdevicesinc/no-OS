/***************************************************************************//**
* @file ad7616_sdz.c
* @brief Implementation of Main Function.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2016(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "spi_engine.h"
#include <xil_cache.h>
#include <xparameters.h>
#include "platform_drivers.h"
#include "ad7616_core.h"
#include "ad7616.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7616_CORE_BASEADDR		XPAR_AXI_AD7616_BASEADDR
#define AD7616_DMA_BASEADDR			XPAR_AXI_AD7616_DMA_BASEADDR
#define SPI_AD7616_CS				0
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET					32 + 54
#define GPIO_ADC_CRCEN				GPIO_OFFSET + 0
#define GPIO_ADC_CHSEL0				GPIO_OFFSET + 1
#define GPIO_ADC_CHSEL1				GPIO_OFFSET + 2
#define GPIO_ADC_CHSEL2				GPIO_OFFSET + 3
#define GPIO_ADC_BURST				GPIO_OFFSET + 4
#define GPIO_ADC_SEQEN				GPIO_OFFSET + 5
#define GPIO_ADC_OS0				GPIO_OFFSET + 6
#define GPIO_ADC_OS1				GPIO_OFFSET + 7
#define GPIO_ADC_OS2				GPIO_OFFSET + 8
#define GPIO_ADC_HW_RNGSEL0			GPIO_OFFSET + 9
#define GPIO_ADC_HW_RNGSEL1			GPIO_OFFSET + 10
#define GPIO_ADC_RESET_N			GPIO_OFFSET + 11
#define ADC_DDR_BASEADDR			XPAR_DDR_MEM_BASEADDR + 0x800000

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
ad7616_init_param default_init_param = {
		/* SPI */
		SPI_AD7616_CS,			// spi_chip_select
		SPI_MODE_2,				// spi_mode
		SPI_ENGINE,				// spi_type
		-1,						// spi_device_id
		/* GPIO */
		PS7_GPIO,				// gpio_type
		GPIO_DEVICE_ID,			// gpio_device_id
		-1,						// gpio_hw_rngsel0
		-1,						// gpio_hw_rngsel1
		GPIO_ADC_RESET_N,		// gpio_reset
		-1,						// gpio_os0
		-1,						// gpio_os1
		-1,						// gpio_os2
		/* Device Settings */
		AD7616_SW,				// mode
		{AD7616_10V, AD7616_10V, AD7616_10V, AD7616_10V,
		 AD7616_10V, AD7616_10V, AD7616_10V, AD7616_10V},	// va[8]
		{AD7616_10V, AD7616_10V, AD7616_10V, AD7616_10V,
		 AD7616_10V, AD7616_10V, AD7616_10V, AD7616_10V},	// vb[8]
		AD7616_OSR_0,			// osr
};

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	ad7616_dev	*dev;
	adc_core	core;
	uint8_t		reg_addr;
	uint16_t	reg_data;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	printf("AD7616 Reference Design.\n");

	core.adc_baseaddr = AD7616_CORE_BASEADDR;
	core.dmac_baseaddr = AD7616_DMA_BASEADDR;
	core.no_of_channels = 2;
	core.resolution = 16;
	ad7616_core_setup(core);

	ad7616_setup(&dev, &core, default_init_param);

	if (dev->interface == AD7616_PARALLEL)
		ad7616_capture_parallel(core, 16384, ADC_DDR_BASEADDR);
	else
		ad7616_capture_serial(core, 16384, ADC_DDR_BASEADDR);

	printf("Capture done. Please run the capture.bat script.\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
