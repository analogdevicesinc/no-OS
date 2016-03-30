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
#include <xil_io.h>
#include "platform_drivers.h"
#include "ad9625.h"
#include "adc_core.h"
#include "jesd204b_gt.h"
#include "jesd204b_v51.h"

#ifdef _XPARAMETERS_PS_H_
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR		XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x800000
#else
#define SPI_DEVICE_ID			XPAR_SPI_0_DEVICE_ID
#define ADC_DDR_BASEADDR		XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000
#endif
#define AD9625_CORE_BASEADDR	XPAR_AXI_AD9625_CORE_BASEADDR
#define AD9625_DMA_BASEADDR		XPAR_AXI_AD9625_DMA_BASEADDR
#define AD9625_JESD_BASEADDR	XPAR_AXI_AD9625_JESD_BASEADDR
#define AD9625_GT_BASEADDR		XPAR_AXI_AD9625_GT_BASEADDR

jesd204b_state jesd204b_st = {
	1,	// lanesync_enable
	1,	// scramble_enable
	0,	// sysref_always_enable
	32,	// frames_per_multiframe
	1,	// bytes_per_frame
	1,	// subclass
};

jesd204b_gt_link gt_link = {
	AD9625_GT_BASEADDR,		// gt_core_addr
	JESD204B_GT_RX,			// tx_or_rx
	0,						// first_lane
	7,						// last_lane
	JESD204B_GT_CPLL,		// qpll_or_cpll
	JESD204B_GT_DFE,		// lpm_or_dfe
	500,					// ref_clk
	1000,					// lane_rate
	JESD204B_GT_SYSREF_INT,	// sysref_int_or_ext
	0,						// sys_clk_sel
	2,						// out_clk_sel
	0,						// gth_or_gtx
};

int main(void)
{
	adc_core ad9625_core;

	ad9625_setup(SPI_DEVICE_ID, 0);

	jesd204b_setup(AD9625_JESD_BASEADDR, jesd204b_st);

	jesd204b_gt_setup(gt_link);
	jesd204b_gt_en_sync_sysref(gt_link);

	ad9625_spi_write(0, AD9625_REG_TEST_CNTRL, 0x0F);
	ad9625_spi_write(0, AD9625_REG_OUTPUT_MODE, 0x01);
	ad9625_spi_write(0, AD9625_REG_TRANSFER, 0x01);

	ad9625_core.adc_baseaddr = AD9625_CORE_BASEADDR;
	ad9625_core.dmac_baseaddr = AD9625_DMA_BASEADDR;
	ad9625_core.no_of_channels = 1;
	ad9625_core.resolution = 12;

	adc_setup(ad9625_core);

	xil_printf("Start capturing data...\n\r");

	adc_capture(ad9625_core, 16384, ADC_DDR_BASEADDR);

	xil_printf("Done.\n\r");

	return 0;
}
