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
#include "ad9517.h"
#include "ad9250.h"
#include "jesd204b_gt.h"
#include "jesd204b_v51.h"
#include "adc_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0x800000
#else
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#endif
#define AD9250_GT_BASEADDR		XPAR_AXI_AD9250_GT_BASEADDR
#define AD9250_JESD_BASEADDR	XPAR_AXI_AD9250_JESD_BASEADDR

jesd204b_state jesd204b_st = {
	1,	// lanesync_enable
	1,	// scramble_enable
	0,	// sysref_always_enable
	32,	// frames_per_multiframe
	1,	// bytes_per_frame
	1,	// subclass
};

jesd204b_gt_link ad9680_gt_link = {
	AD9250_GT_BASEADDR,		// gt_core_addr
	JESD204B_GT_RX,			// tx_or_rx
	0,						// first_lane
	3,						// last_lane
	JESD204B_GT_CPLL,		// qpll_or_cpll
	JESD204B_GT_DFE,		// lpm_or_dfe
	500,					// ref_clk
	1000,					// lane_rate
	JESD204B_GT_SYSREF_INT,	// sysref_int_or_ext
	0,						// sys_clk_sel
	2,						// out_clk_sel
	0,						// gth_or_gtx
};

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	adc_core ad9250_0_core;
	adc_core ad9250_1_core;

	ad9517_setup(SPI_DEVICE_ID, 0);
	ad9250_setup(SPI_DEVICE_ID, 0, 0);
	ad9250_setup(SPI_DEVICE_ID, 0, 1);

	jesd204b_gt_setup(ad9680_gt_link);
	jesd204b_gt_en_sync_sysref(ad9680_gt_link);

	jesd204b_setup(AD9250_JESD_BASEADDR, jesd204b_st);

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

    ad9250_spi_write(0, 0x0d, 0x0f);
    ad9250_spi_write(0, 0xff, 0x01);

    ad9250_spi_write(1, 0x0d, 0x0f);
    ad9250_spi_write(1, 0xff, 0x01);

	adc_capture(ad9250_0_core, 32768, ADC_DDR_BASEADDR);
	adc_capture(ad9250_1_core, 32768, ADC_DDR_BASEADDR + 0x20000);

	xil_printf("Ramp capture done.\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	xil_printf("Done.\n");

	return 0;
}
