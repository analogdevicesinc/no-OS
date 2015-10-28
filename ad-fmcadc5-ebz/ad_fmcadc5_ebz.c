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

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD9625_CORE_0_BASEADDR		XPAR_AXI_AD9625_0_CORE_BASEADDR
#define AD9625_CORE_1_BASEADDR		XPAR_AXI_AD9625_1_CORE_BASEADDR
#define AD9625_DMA_BASEADDR			XPAR_AXI_AD9625_DMA_BASEADDR
#define AD9625_JESD_0_BASEADDR		XPAR_AXI_AD9625_0_JESD_BASEADDR
#define AD9625_JESD_1_BASEADDR		XPAR_AXI_AD9625_1_JESD_BASEADDR
#define FMCADC5_GT_0_BASEADDR		XPAR_AXI_FMCADC5_0_GT_BASEADDR
#define FMCADC5_GT_1_BASEADDR		XPAR_AXI_FMCADC5_1_GT_BASEADDR
#define GPIO_DEVICE_ID				XPAR_AXI_GPIO_DEVICE_ID
#define GPIO_OFFSET					32
#define GPIO_RST_0					GPIO_OFFSET + 2
#define GPIO_PWDN_0					GPIO_OFFSET + 3
#define GPIO_RST_1					GPIO_OFFSET + 6
#define GPIO_PWDN_1					GPIO_OFFSET + 7
#define GPIO_IRQ_0					GPIO_OFFSET + 8
#define GPIO_FD_0					GPIO_OFFSET + 9
#define GPIO_IRQ_1					GPIO_OFFSET + 10
#define GPIO_FD_1					GPIO_OFFSET + 11
#define GPIO_PWR_GOOD				GPIO_OFFSET + 12
#define ADC_DDR_BASEADDR			XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000

/***************************************************************************//**
* @brief adc5_gpio_ctl
*******************************************************************************/
int32_t adc5_gpio_ctl(uint32_t device_id)
{
	uint8_t pwr_good;

	gpio_init(device_id);

	gpio_direction(GPIO_RST_0, GPIO_OUTPUT);
	gpio_direction(GPIO_PWDN_0, GPIO_OUTPUT);
	gpio_direction(GPIO_RST_1, GPIO_OUTPUT);
	gpio_direction(GPIO_PWDN_1, GPIO_OUTPUT);
	gpio_direction(GPIO_IRQ_0, GPIO_INPUT);
	gpio_direction(GPIO_FD_0, GPIO_INPUT);
	gpio_direction(GPIO_IRQ_1, GPIO_INPUT);
	gpio_direction(GPIO_FD_1, GPIO_INPUT);
	gpio_direction(GPIO_PWR_GOOD, GPIO_INPUT);
	gpio_set_value(GPIO_RST_0, 0);
	gpio_set_value(GPIO_PWDN_0, 0);
	gpio_set_value(GPIO_RST_1, 0);
	gpio_set_value(GPIO_PWDN_1, 0);

	gpio_get_value(GPIO_PWR_GOOD, &pwr_good);
	if (!pwr_good) {
		xil_printf("Error: GPIO Power Good NOT set.\n\r");
		return -1;
	}
	mdelay(1);

	gpio_set_value(GPIO_RST_0, 1);
	gpio_set_value(GPIO_RST_1, 1);
	mdelay(100);

	return 0;
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	jesd204b_gt_link jesd204b_gt_link;
	jesd204b_state jesd204b_st;
	adc_core ad9625_0;
	adc_core ad9625_1;

	jesd204b_st.lanesync_enable = 1;
	jesd204b_st.scramble_enable = 1;
	jesd204b_st.sysref_always_enable = 0;
	jesd204b_st.frames_per_multiframe = 32;
	jesd204b_st.bytes_per_frame = 1;
	jesd204b_st.subclass = 1;
	jesd204b_gt_link.tx_or_rx = JESD204B_GT_RX;
	jesd204b_gt_link.first_lane = 0;
	jesd204b_gt_link.last_lane = 7;
	jesd204b_gt_link.qpll_or_cpll = JESD204B_GT_CPLL;
	jesd204b_gt_link.lpm_or_dfe = JESD204B_GT_DFE;
	jesd204b_gt_link.ref_clk = 625;
	jesd204b_gt_link.lane_rate = 6250;
	jesd204b_gt_link.sysref_int_or_ext = JESD204B_GT_SYSREF_INT;
	jesd204b_gt_link.sys_clk_sel = 0;
	jesd204b_gt_link.out_clk_sel = 2;
	jesd204b_gt_link.gth_or_gtx = 0;

	if (adc5_gpio_ctl(GPIO_DEVICE_ID))
		return -1;

	ad9625_setup(XPAR_SPI_0_DEVICE_ID, 0);
	ad9625_setup(XPAR_SPI_0_DEVICE_ID, 1);

	jesd204b_gt_initialize(XPAR_AXI_FMCADC5_0_GT_BASEADDR, 8);
	jesd204b_setup(XPAR_AXI_AD9625_0_JESD_BASEADDR, jesd204b_st);
	jesd204b_gt_setup(jesd204b_gt_link);

	jesd204b_gt_initialize(XPAR_AXI_FMCADC5_1_GT_BASEADDR, 8);
	jesd204b_setup(XPAR_AXI_AD9625_1_JESD_BASEADDR, jesd204b_st);
	jesd204b_gt_setup(jesd204b_gt_link);

	ad9625_0.adc_baseaddr = AD9625_CORE_0_BASEADDR;
	ad9625_0.dmac_baseaddr = AD9625_DMA_BASEADDR;
	adc_setup(ad9625_0, 2);

	ad9625_1.adc_baseaddr = AD9625_CORE_1_BASEADDR;
	ad9625_1.dmac_baseaddr = 0;
	adc_setup(ad9625_1, 2);

	ad9625_spi_write(0, AD9625_REG_TEST_CNTRL, 0x0F);
	ad9625_spi_write(0, AD9625_REG_OUTPUT_MODE, 0x00);
	ad9625_spi_write(0, AD9625_REG_TRANSFER, 0x01);

	ad9625_spi_write(1, AD9625_REG_TEST_CNTRL, 0x0F);
	ad9625_spi_write(1, AD9625_REG_OUTPUT_MODE, 0x00);
	ad9625_spi_write(1, AD9625_REG_TRANSFER, 0x01);

	xil_printf("Initialization done.\n\r");

	adc_capture(ad9625_0, 32768, ADC_DDR_BASEADDR);

	xil_printf("Capture done.\n\r");

	return 0;
}
