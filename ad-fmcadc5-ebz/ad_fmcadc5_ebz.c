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

// ##############################################################################
// ##############################################################################

int32_t jesd204b_gt_setup_modified(jesd204b_gt_link link_param) {

  int32_t lane;

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--) {

    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_PLL_RSTN(lane), 0);
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_GT_RSTN(lane), 0);
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_RSTN(lane), 0);
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_USER_READY(lane), 0);
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_SYNC_CTL(lane), 0);
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_SYSREF_CTL(lane), 0);
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_LPM_CPLL_PD(lane),
      JESD204B_GT_LPM_DFE(link_param.lpm_or_dfe));
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_CLK_SEL(lane),
      JESD204B_GT_SYS_CLK_SEL(link_param.sys_clk_sel) |
      JESD204B_GT_OUT_CLK_SEL(link_param.out_clk_sel));
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_PLL_RSTN(lane),
      JESD204B_GT_PLL_RSTN);
  }

  // check pll status

  if(jesd204b_gt_txrx_status(link_param.tx_or_rx, link_param.first_lane,
    link_param.last_lane, JESD204B_GT_STATUS_PLL_LOCKED) != 0)
    return(-1);

  // bring phy out of reset

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--) {
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_GT_RSTN(lane),
      JESD204B_GT_RSTN);
  }
  mdelay(1);

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--) {
    jesd204b_gt_txrx_write(link_param.tx_or_rx, JESD204B_GT_REG_USER_READY(lane),
      JESD204B_GT_USER_READY);
  }

  // check reset-done & pll status

  if(jesd204b_gt_txrx_status(link_param.tx_or_rx, link_param.first_lane,
    link_param.last_lane, JESD204B_GT_STATUS_RST_DONE) != 0)
    return(-1);

  return(0);
}

int32_t gtlink_control(uint32_t data) {

  int32_t n;

  for (n = 7; n >= 0; n--) {
    Xil_Out32((XPAR_AXI_FMCADC5_0_GT_BASEADDR + JESD204B_GT_REG_RSTN(n)), data);
    Xil_Out32((XPAR_AXI_FMCADC5_0_GT_BASEADDR + JESD204B_GT_REG_SYNC_CTL(n)), data);
    Xil_Out32((XPAR_AXI_FMCADC5_1_GT_BASEADDR + JESD204B_GT_REG_RSTN(n)), data);
    Xil_Out32((XPAR_AXI_FMCADC5_1_GT_BASEADDR + JESD204B_GT_REG_SYNC_CTL(n)), data);
  }
  return(0);
}

int32_t gtlink_sysref(uint32_t data, uint32_t status) {

  uint32_t n;
  uint32_t rdata;
  int32_t rstatus;

  Xil_Out32((XPAR_AXI_FMCADC5_0_GT_BASEADDR + JESD204B_GT_REG_SYSREF_CTL(0)), 0x0);
  Xil_Out32((XPAR_AXI_FMCADC5_0_GT_BASEADDR + JESD204B_GT_REG_SYSREF_CTL(0)), data);
  Xil_Out32((XPAR_AXI_FMCADC5_0_GT_BASEADDR + JESD204B_GT_REG_SYSREF_CTL(0)), 0x0);

  // check the status again-

  rstatus = 0;

  for (n = 0; n < 8; n++) {
    rdata = Xil_In32(XPAR_AXI_FMCADC5_0_GT_BASEADDR + JESD204B_GT_REG_STATUS(n));
    if (rdata != status) {
      rstatus = -1;
      xil_printf("JESD204B-GT[%d,%d]: Invalid status, received(0x%05x), expected(0x%05x)!\n", 0, n, rdata, status);
    }
    rdata = Xil_In32(XPAR_AXI_FMCADC5_1_GT_BASEADDR + JESD204B_GT_REG_STATUS(n));
    if (rdata != status) {
      rstatus = -1;
      xil_printf("JESD204B-GT[%d,%d]: Invalid status, received(0x%05x), expected(0x%05x)!\n", 1, n, rdata, status);
    }
  }

  return(rstatus);
}

int32_t ad9625_sysref_status(void) {

  uint32_t n;
  uint8_t rdata8;
  int32_t rstatus;

  rstatus = 0;

  for (n = 0; n < 2; n++) {
    ad9625_spi_read(n, 0x100, &rdata8);
    if ((rdata8 & 0x0c) != 0) {
      rstatus = -1;
      xil_printf("AD9625[%d]: %04x, %02x\n", n, 0x100, rdata8);
    }
  }

  return(rstatus);
}

int32_t ad9625_sysref_simple(void) {

  // reset the data path, but make sure we didn't break anything

  gtlink_control(0);
  if (gtlink_sysref(0, 0xffff) != 0) {
    xil_printf("[%05d]: Interleaving Synchronization Failed, Exiting!!\n", __LINE__);
    return(-1);
  }

  // sysref, both cores now gets sysref at the same time

  gtlink_control(1);
  if (gtlink_sysref(1, 0x1ffff) != 0) {
    xil_printf("[%05d]: Interleaving Synchronization Failed, Exiting!!\n", __LINE__);
    return(-1);
  }

  ad9625_sysref_status();

  return(0);
}

int32_t ad9625_sysref_sw_calibrate(void) {

  uint32_t n;
  uint32_t rgpio;
  uint32_t rdata;
  uint32_t rstatus;
  uint8_t rdata8_0;
  uint8_t rdata8_1;

  rgpio = Xil_In32(XPAR_GPIO_0_BASEADDR + 0x8) & 0x00ffffff;
  Xil_Out32((XPAR_GPIO_0_BASEADDR + 0x8), (rgpio | (31<<24)));

  ad9625_spi_write(0, 0x13c, 0x60);
	ad9625_spi_write(0, 0x0ff, 0x01);

  rstatus = 0;

  for (n = 0; n < 32; n++) {

    gtlink_control(0);
    if (gtlink_sysref(0, 0xffff) != 0) {
      xil_printf("[%05d]: Interleaving Synchronization Failed, Exiting!!\n", __LINE__);
      return(-1);
    }

    ad9625_spi_write(0, 0x03a, 0x42);
	  ad9625_spi_write(0, 0x0ff, 0x01);
    ad9625_spi_write(0, 0x03a, 0x02);
	  ad9625_spi_write(0, 0x0ff, 0x01);

    ad9625_spi_write(1, 0x03a, 0x42);
	  ad9625_spi_write(1, 0x0ff, 0x01);
    ad9625_spi_write(1, 0x03a, 0x02);
	  ad9625_spi_write(1, 0x0ff, 0x01);

    Xil_Out32((XPAR_GPIO_0_BASEADDR + 0x8), (rgpio | (n<<24)));
    rdata = Xil_In32(XPAR_GPIO_0_BASEADDR + 0x8);
    if ((rdata & 0xff000000) != ((n<<24) | 0x20000000)) {
      xil_printf("[%05d]: SYSREF Delay Control Failed(%d, 0x%08x), Exiting!!\n", __LINE__, n, rdata);
      return(-1);
    }

    ad9625_spi_read(0, 0x100, &rdata8_0);
    ad9625_spi_read(1, 0x100, &rdata8_1);
    if (((rdata8_0 & 0x04) != 0x00) || ((rdata8_1 & 0x04) != 0x00)) {
      xil_printf("[%05d]: SYSREF Clear Status Failed(%d, 0x%02x, 0x%02x), Exiting!!\n", __LINE__, n, rdata8_0, rdata8_1);
      return(-1);
    }
    
    gtlink_control(1);
    if (gtlink_sysref(1, 0x1ffff) != 0) {
      xil_printf("[%05d]: Interleaving Synchronization Failed, Exiting!!\n", __LINE__);
      return(-1);
    }

    ad9625_spi_read(0, 0x100, &rdata8_0);
    ad9625_spi_read(1, 0x100, &rdata8_1);

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

  return(0);
}


// ##############################################################################
// ##############################################################################


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
	jesd204b_gt_setup_modified(jesd204b_gt_link);

	jesd204b_gt_initialize(XPAR_AXI_FMCADC5_1_GT_BASEADDR, 8);
	jesd204b_setup(XPAR_AXI_AD9625_1_JESD_BASEADDR, jesd204b_st);
	jesd204b_gt_setup_modified(jesd204b_gt_link);

	ad9625_0.adc_baseaddr = AD9625_CORE_0_BASEADDR;
	ad9625_0.dmac_baseaddr = AD9625_DMA_BASEADDR;

	ad9625_1.adc_baseaddr = AD9625_CORE_1_BASEADDR;
	ad9625_1.dmac_baseaddr = 0;

  // setup above works independently, here we need to start again- so that
  // both cores receive sysref at the same time.
  // reset the data path from master

  // reconfigure the devices so that sysref is used for synchronization

	ad9625_spi_write(0, 0x072, 0x8b); // CS - overrange + sysref time-stamp. (default is 0x0b)
	ad9625_spi_write(0, 0x03a, 0x02); // Sysref enabled (default is 0x00)
	ad9625_spi_write(0, 0x0ff, 0x01); // Register update
	ad9625_spi_write(1, 0x072, 0x8b); // CS - overrange + sysref time-stamp. (default is 0x0b)
	ad9625_spi_write(1, 0x03a, 0x02); // Sysref enabled (default is 0x00)
	ad9625_spi_write(1, 0x0ff, 0x01); // Register update

  // ad9625_sysref_simple();

  ad9625_sysref_sw_calibrate();

  // data path must be active now, bring cores out of reset

	adc_setup(ad9625_0, 1);
	adc_setup(ad9625_1, 1);

  // check prbs on both channels-

	ad9625_spi_write(0, AD9625_REG_TEST_CNTRL, 0x5);
	ad9625_spi_write(0, AD9625_REG_OUTPUT_MODE, 0x0);
	ad9625_spi_write(0, AD9625_REG_TRANSFER, 0x1);

	ad9625_spi_write(1, AD9625_REG_TEST_CNTRL, 0x5);
	ad9625_spi_write(1, AD9625_REG_OUTPUT_MODE, 0x0);
	ad9625_spi_write(1, AD9625_REG_TRANSFER, 0x1);

	if (adc_pn_mon(ad9625_0, 1, ADC_PN23A) != 0) return(-1);
	if (adc_pn_mon(ad9625_1, 1, ADC_PN23A) != 0) return(-1);

  // no prbs errors, now sync the 2 devices

	ad9625_spi_write(0, AD9625_REG_TEST_CNTRL, 0x0);
	ad9625_spi_write(0, AD9625_REG_OUTPUT_MODE, 0x1);
	ad9625_spi_write(0, AD9625_REG_TRANSFER, 0x1);
  adc_write(ad9625_0, ADC_REG_CHAN_CNTRL(0), 0x51);

	ad9625_spi_write(1, AD9625_REG_TEST_CNTRL, 0x0);
	ad9625_spi_write(1, AD9625_REG_OUTPUT_MODE, 0x1);
	ad9625_spi_write(1, AD9625_REG_TRANSFER, 0x1);
  adc_write(ad9625_1, ADC_REG_CHAN_CNTRL(0), 0x51);


	xil_printf("Initialization done.\n");

	adc_capture(ad9625_0, 32768, ADC_DDR_BASEADDR);

	xil_printf("Capture done.\n");

	return 0;
}
