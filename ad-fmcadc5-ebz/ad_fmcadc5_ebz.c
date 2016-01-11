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
#define SPI_DEVICE_ID				XPAR_SPI_0_DEVICE_ID
#define SPI_AD9625_0_SS				0
#define SPI_AD9625_1_SS				1
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

jesd204b_state jesd204b_st = {
	1,	// lanesync_enable
	1,	// scramble_enable
	0,	// sysref_always_enable
	32,	// frames_per_multiframe
	1,	// bytes_per_frame
	1,	// subclass
};

jesd204b_gt_link gt_0_link = {
	FMCADC5_GT_0_BASEADDR,	// gt_core_addr
	JESD204B_GT_RX,			// tx_or_rx
	0,						// first_lane
	7,						// last_lane
	JESD204B_GT_QPLL,		// qpll_or_cpll
	JESD204B_GT_DFE,		// lpm_or_dfe
	625,					// ref_clk
	6250,					// lane_rate
	JESD204B_GT_SYSREF_INT,	// sysref_int_or_ext
	0,						// sys_clk_sel
	2,						// out_clk_sel
	0,						// gth_or_gtx
};

jesd204b_gt_link gt_1_link = {
	FMCADC5_GT_1_BASEADDR,	// gt_core_addr
	JESD204B_GT_RX,			// tx_or_rx
	0,						// first_lane
	7,						// last_lane
	JESD204B_GT_QPLL,		// qpll_or_cpll
	JESD204B_GT_DFE,		// lpm_or_dfe
	625,					// ref_clk
	6250,					// lane_rate
	JESD204B_GT_SYSREF_INT,	// sysref_int_or_ext
	0,						// sys_clk_sel
	2,						// out_clk_sel
	0,						// gth_or_gtx
};

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
	mdelay(10);

	gpio_get_value(GPIO_PWR_GOOD, &pwr_good);
	if (!pwr_good) {
		xil_printf("Error: GPIO Power Good NOT set.\n\r");
		return -1;
	}

	gpio_set_value(GPIO_RST_0, 1);
	gpio_set_value(GPIO_RST_1, 1);
	mdelay(100);

	return 0;
}

/***************************************************************************//**
* @brief adc5_gtlink_control
*******************************************************************************/
int32_t adc5_gtlink_control(uint32_t data)
{
	int32_t n;

	for (n = 7; n >= 0; n--) {
		jesd204b_gt_txrx_write(gt_0_link, JESD204B_GT_REG_RSTN(n), data);
		jesd204b_gt_txrx_write(gt_0_link, JESD204B_GT_REG_SYNC_CTL(n), data);
		jesd204b_gt_txrx_write(gt_1_link, JESD204B_GT_REG_RSTN(n), data);
		jesd204b_gt_txrx_write(gt_1_link, JESD204B_GT_REG_SYNC_CTL(n), data);
	}
	return(0);
}

/***************************************************************************//**
* @brief adc5_gtlink_sysref
*******************************************************************************/
int32_t adc5_gtlink_sysref(uint32_t data,
						   uint32_t status)
{
	uint32_t n;
	uint32_t rdata;
	int32_t rstatus = 0;

	jesd204b_gt_txrx_write(gt_0_link, JESD204B_GT_REG_SYSREF_CTL(0), 0x0);
	jesd204b_gt_txrx_write(gt_0_link, JESD204B_GT_REG_SYSREF_CTL(0), data);
	jesd204b_gt_txrx_write(gt_0_link, JESD204B_GT_REG_SYSREF_CTL(0), 0x0);

	// check the status again-

	for (n = 0; n < 8; n++) {
		jesd204b_gt_read(gt_0_link, JESD204B_GT_REG_STATUS(n), &rdata);
		if (rdata != status) {
			rstatus = -1;
			xil_printf("JESD204B-GT[%d,%d]: Invalid status, received(0x%05x), "
					"expected(0x%05x)!\n", 0, n, rdata, status);
		}
		jesd204b_gt_read(gt_1_link, JESD204B_GT_REG_STATUS(n), &rdata);
		if (rdata != status) {
			rstatus = -1;
			xil_printf("JESD204B-GT[%d,%d]: Invalid status, received(0x%05x), "
					"expected(0x%05x)!\n", 1, n, rdata, status);
		}
	}

	return rstatus;
}

/***************************************************************************//**
* @brief adc5_sysref_sw_calibrate
*******************************************************************************/
int32_t adc5_sysref_sw_calibrate(void)
{
	uint32_t n;
	uint32_t rgpio;
	uint32_t rdata;
	uint32_t rstatus = 0;
	uint8_t rdata8_0;
	uint8_t rdata8_1;

	// reconfigure the devices so that sysref is used for synchronization

	ad9625_spi_write(SPI_AD9625_0_SS, 0x072, 0x8b); // CS - overrange + sysref time-stamp. (default is 0x0b)
	ad9625_spi_write(SPI_AD9625_0_SS, 0x03a, 0x02); // Sysref enabled (default is 0x00)
	ad9625_spi_write(SPI_AD9625_0_SS, 0x0ff, 0x01); // Register update
	ad9625_spi_write(SPI_AD9625_1_SS, 0x072, 0x8b); // CS - overrange + sysref time-stamp. (default is 0x0b)
	ad9625_spi_write(SPI_AD9625_1_SS, 0x03a, 0x02); // Sysref enabled (default is 0x00)
	ad9625_spi_write(SPI_AD9625_1_SS, 0x0ff, 0x01); // Register update

	rgpio = Xil_In32(XPAR_GPIO_0_BASEADDR + 0x8) & 0x00ffffff;
	Xil_Out32((XPAR_GPIO_0_BASEADDR + 0x8), (rgpio | (31<<24)));

	ad9625_spi_write(SPI_AD9625_0_SS, 0x13c, 0x60);
	ad9625_spi_write(SPI_AD9625_0_SS, 0x0ff, 0x01);

	for (n = 0; n < 32; n++) {
		adc5_gtlink_control(0);
		if (adc5_gtlink_sysref(0, 0xffff) != 0) {
			xil_printf("[%05d]: Interleaving Synchronization Failed, "
					"Exiting!!\n", __LINE__);
			return -1;
		}

		ad9625_spi_write(SPI_AD9625_0_SS, 0x03a, 0x42);
		ad9625_spi_write(SPI_AD9625_0_SS, 0x0ff, 0x01);
		ad9625_spi_write(SPI_AD9625_0_SS, 0x03a, 0x02);
		ad9625_spi_write(SPI_AD9625_0_SS, 0x0ff, 0x01);

		ad9625_spi_write(SPI_AD9625_1_SS, 0x03a, 0x42);
		ad9625_spi_write(SPI_AD9625_1_SS, 0x0ff, 0x01);
		ad9625_spi_write(SPI_AD9625_1_SS, 0x03a, 0x02);
		ad9625_spi_write(SPI_AD9625_1_SS, 0x0ff, 0x01);

		Xil_Out32((XPAR_GPIO_0_BASEADDR + 0x8), (rgpio | (n<<24)));
		rdata = Xil_In32(XPAR_GPIO_0_BASEADDR + 0x8);
		if ((rdata & 0xff000000) != ((n<<24) | 0x20000000)) {
		xil_printf("[%05d]: SYSREF Delay Control Failed(%d, 0x%08x), Exiting!!\n", __LINE__, n, rdata);
		return(-1);
		}

		ad9625_spi_read(SPI_AD9625_0_SS, 0x100, &rdata8_0);
		ad9625_spi_read(SPI_AD9625_1_SS, 0x100, &rdata8_1);
		if (((rdata8_0 & 0x04) != 0x00) || ((rdata8_1 & 0x04) != 0x00)) {
		xil_printf("[%05d]: SYSREF Clear Status Failed(%d, 0x%02x, 0x%02x), Exiting!!\n", __LINE__, n, rdata8_0, rdata8_1);
		return(-1);
		}

		adc5_gtlink_control(1);
		if (adc5_gtlink_sysref(1, 0x1ffff) != 0) {
		xil_printf("[%05d]: Interleaving Synchronization Failed, Exiting!!\n", __LINE__);
		return(-1);
		}

		ad9625_spi_read(SPI_AD9625_0_SS, 0x100, &rdata8_0);
		ad9625_spi_read(SPI_AD9625_1_SS, 0x100, &rdata8_1);

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
		return -1;
	}

	return 0;
}

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	adc_core ad9625_0_core;
	adc_core ad9625_1_core;

	if (adc5_gpio_ctl(GPIO_DEVICE_ID))
		return -1;

	ad9625_setup(SPI_DEVICE_ID, SPI_AD9625_0_SS);
	ad9625_setup(SPI_DEVICE_ID, SPI_AD9625_1_SS);

	jesd204b_gt_initialize(gt_0_link);
	jesd204b_setup(XPAR_AXI_AD9625_0_JESD_BASEADDR, jesd204b_st);
	jesd204b_gt_setup(gt_0_link);

	jesd204b_gt_initialize(gt_1_link);
	jesd204b_setup(XPAR_AXI_AD9625_1_JESD_BASEADDR, jesd204b_st);
	jesd204b_gt_setup(gt_1_link);

	ad9625_0_core.adc_baseaddr = AD9625_CORE_0_BASEADDR;
	ad9625_0_core.dmac_baseaddr = AD9625_DMA_BASEADDR;
	ad9625_0_core.no_of_channels = 1;
	ad9625_0_core.resolution = 12;

	ad9625_1_core.adc_baseaddr = AD9625_CORE_1_BASEADDR;
	ad9625_1_core.dmac_baseaddr = 0;
	ad9625_1_core.no_of_channels = 1;
	ad9625_0_core.resolution = 12;

	// setup above works independently, here we need to start again- so that
	// both cores receive sysref at the same time.
	// reset the data path from master

	adc5_sysref_sw_calibrate();

	// data path must be active now, bring cores out of reset

	adc_setup(ad9625_0_core);
	adc_setup(ad9625_1_core);

	// check prbs on both channels-

	ad9625_spi_write(SPI_AD9625_0_SS, AD9625_REG_TEST_CNTRL, 0x5);
	ad9625_spi_write(SPI_AD9625_0_SS, AD9625_REG_OUTPUT_MODE, 0x0);
	ad9625_spi_write(SPI_AD9625_0_SS, AD9625_REG_TRANSFER, 0x1);

	ad9625_spi_write(SPI_AD9625_1_SS, AD9625_REG_TEST_CNTRL, 0x5);
	ad9625_spi_write(SPI_AD9625_1_SS, AD9625_REG_OUTPUT_MODE, 0x0);
	ad9625_spi_write(SPI_AD9625_1_SS, AD9625_REG_TRANSFER, 0x1);

	if (adc_pn_mon(ad9625_0_core, ADC_PN23A) != 0)
		return -1;
	if (adc_pn_mon(ad9625_1_core, ADC_PN23A) != 0)
		return -1;

	// no prbs errors, now sync the 2 devices

	ad9625_spi_write(SPI_AD9625_0_SS, AD9625_REG_TEST_CNTRL, 0x0);
	ad9625_spi_write(SPI_AD9625_0_SS, AD9625_REG_OUTPUT_MODE, 0x1);
	ad9625_spi_write(SPI_AD9625_0_SS, AD9625_REG_TRANSFER, 0x1);
	adc_write(ad9625_0_core, ADC_REG_CHAN_CNTRL(0), 0x51);

	ad9625_spi_write(SPI_AD9625_1_SS, AD9625_REG_TEST_CNTRL, 0x0);
	ad9625_spi_write(SPI_AD9625_1_SS, AD9625_REG_OUTPUT_MODE, 0x1);
	ad9625_spi_write(SPI_AD9625_1_SS, AD9625_REG_TRANSFER, 0x1);
	adc_write(ad9625_1_core, ADC_REG_CHAN_CNTRL(0), 0x51);

	xil_printf("Initialization done.\n");

	adc_capture(ad9625_0_core, 32768, ADC_DDR_BASEADDR);

	xil_printf("Capture done.\n");

	return 0;
}
