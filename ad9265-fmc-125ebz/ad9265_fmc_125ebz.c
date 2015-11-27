/***************************************************************************//**
 *   @file   ad9434_fmc_500ebz.c
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
#include <xil_cache.h>
#include <xil_printf.h>
#include "xparameters.h"
#include "adc_core.h"
#include "ad9265.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define AD9265_CORE_BASEADDR	XPAR_AXI_AD9265_BASEADDR
#define AD9265_DMA_BASEADDR		XPAR_AXI_AD9265_DMA_BASEADDR
#define ADC_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0x800000

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	adc_core ad9625_core;

	ad9625_core.adc_baseaddr = AD9265_CORE_BASEADDR;
	ad9625_core.dmac_baseaddr = AD9265_DMA_BASEADDR;
	ad9625_core.no_of_channels = 1;
	adc_setup(ad9625_core);

	ad9265_setup(SPI_DEVICE_ID, 0, ad9625_core);

	ad9265_testmode_set(0, TESTMODE_ONE_ZERO_TOGGLE);

	adc_capture(ad9625_core, 32768, ADC_DDR_BASEADDR);

	xil_printf("Done\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
