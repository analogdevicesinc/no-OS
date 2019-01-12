/***************************************************************************//**
 * @file jesd_core.c
 * @brief Implementation of JESD Core Driver.
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
#include "jesd_core.h"

/***************************************************************************//**
 * @brief xcvr_write
 *******************************************************************************/
void xcvr_write(xcvr_device *dev,
				uint32_t reg,
				uint32_t val)
{
	IOWR_32DIRECT(dev->xcvr_base_addr, reg, val);
}

/***************************************************************************//**
 * @brief xcvr_read
 *******************************************************************************/
uint32_t xcvr_read(xcvr_device *dev,
				   uint32_t reg_addr)
{
	return IORD_32DIRECT(dev->xcvr_base_addr, reg_addr);
}

/***************************************************************************//**
 * @brief atx_pll_write
 *******************************************************************************/
void atx_pll_write(xcvr_device *dev,
				   uint32_t reg,
				   uint32_t val)
{
	IOWR_32DIRECT(dev->atx_pll_base_addr, reg, val);
}

/***************************************************************************//**
 * @brief atx_pll_read
 *******************************************************************************/
uint32_t atx_pll_read(xcvr_device *dev,
				      uint32_t reg_addr)
{
	return IORD_32DIRECT(dev->atx_pll_base_addr, reg_addr);
}

/***************************************************************************//**
 * @brief adxcfg_write
 *******************************************************************************/
void adxcfg_write(xcvr_device *dev,
				  uint8_t lane,
				  uint32_t reg,
				  uint32_t val)
{
	IOWR_32DIRECT(dev->adxcfg_base_addr[lane], reg, val);
}

/***************************************************************************//**
 * @brief adxcfg_read
 *******************************************************************************/
uint32_t adxcfg_read(xcvr_device *dev,
				 	 uint8_t lane,
				 	 uint32_t reg_addr)
{
	return IORD_32DIRECT(dev->adxcfg_base_addr[lane], reg_addr);
}

/***************************************************************************//**
 * @brief jesd_write
 *******************************************************************************/
void jesd_write(jesd_device *dev,
				uint32_t reg,
				uint32_t val)
{
	IOWR_32DIRECT(dev->base_addr, reg, val);
}

/***************************************************************************//**
 * @brief jesd_read
 *******************************************************************************/
uint32_t jesd_read(jesd_device *dev,
				   uint32_t reg_addr)
{
	return IORD_32DIRECT(dev->base_addr, reg_addr);
}

/***************************************************************************//**
 * @brief atx_pll_calib
 *******************************************************************************/
uint8_t atx_pll_calib(xcvr_device *dev)
{
	uint8_t link = dev->link_num;
	uint32_t addr;
	uint8_t mask;
	uint8_t val;
	uint32_t write_val;
	uint32_t read_val;

	/* Get AVMM Interface */
	addr = XCVR_REG_ARBITRATION(link);
	mask = XCVR_ARBITRATION_MASK;
	val = XCVR_ARBITRATION_GET_AVMM;
	write_val = (atx_pll_read(dev, addr) & ~mask) | (val & mask);
	atx_pll_write(dev, addr, write_val);

	/* Initiate re-calibration of ATX_PLL */
	addr = XCVR_REG_CALIB_ATX_PLL_EN(link);
	mask = XCVR_CALIB_ATX_PLL_EN_MASK;
	val = XCVR_CALIB_ATX_PLL_EN;
	write_val = (atx_pll_read(dev, addr) & ~mask) | (val & mask);
	atx_pll_write(dev, addr, write_val);

	/* Release AVMM Interface to PreSICE */
	addr = XCVR_REG_ARBITRATION(link);
	mask = XCVR_ARBITRATION_MASK;
	val = XCVR_ARBITRATION_RELEASE_AVMM;
	write_val = (atx_pll_read(dev, addr) & ~mask) | (val & mask);
	atx_pll_write(dev, addr, write_val);

	mdelay(100);	// Wait 100ms for cal_busy to de-assert

	/* Read ATX PLL calibration status from capability register */
	addr = XCVR_REG_CAPAB_ATX_PLL_STAT(link);
	read_val = atx_pll_read(dev, addr);
	if ((read_val & XCVR_CAPAB_ATX_PLL_CAL_BSY_MASK) ==
			XCVR_CAPAB_ATX_PLL_CAL_DONE) {
		printf("ATX PLL calibration OK\n");
		return 0;
	}
	else {
		printf("ATX PLL calibration error\n");
		return 1;
	}
}

/***************************************************************************//**
 * @brief xcvr_calib_tx
 *******************************************************************************/
uint8_t xcvr_calib_tx(xcvr_device *dev)
{
	uint8_t link = dev->link_num;
	uint8_t lane;
	uint32_t addr;
	uint8_t mask;
	uint8_t val;
	uint32_t write_val;
	uint32_t read_val;
	uint8_t err = 0;

	for (lane = 0; lane < dev->lanes_per_link; lane++) {
		/* Get AVMM InterfaceeSICE through arbitration register */
		addr = XCVR_REG_ARBITRATION(link);
		mask = XCVR_ARBITRATION_MASK;
		val = XCVR_ARBITRATION_GET_AVMM;
		write_val = (adxcfg_read(dev, lane, addr) & ~mask) | (val & mask);
		adxcfg_write(dev, lane, addr, write_val);

		/* Perform TX termination & Vod calibration through
		   PMA calibration enable register */
		addr = XCVR_REG_CALIB_PMA_EN(link);
		mask = XCVR_CALIB_TX_TERM_VOD_MASK;
		val = XCVR_CALIB_TX_TERM_VOD_EN;
		write_val = (adxcfg_read(dev, lane, addr) & ~mask) | (val & mask);
		adxcfg_write(dev, lane, addr, write_val);

		/* Disable rx_cal_busy and enable tx_cal_busy output through
		   capability register */
		addr = XCVR_REG_CAPAB_PMA(link);
		mask = XCVR_CAPAB_RX_CAL_BUSY_EN_MASK | XCVR_CAPAB_TX_CAL_BUSY_EN_MASK;
		val = XCVR_CAPAB_RX_CAL_BUSY_DIS | XCVR_CAPAB_TX_CAL_BUSY_EN;
		write_val = (adxcfg_read(dev, lane, addr) & ~mask) | (val & mask);
		adxcfg_write(dev, lane, addr, write_val);

		/* Release AVMM Interface to PreSICE */
		addr = XCVR_REG_ARBITRATION(link);
		mask = XCVR_ARBITRATION_MASK;
		val = XCVR_ARBITRATION_RELEASE_AVMM;
		write_val = (adxcfg_read(dev, lane, addr) & ~mask) | (val & mask);
		adxcfg_write(dev, lane, addr, write_val);

		mdelay(100);	// Wait 100ms for cal_busy to de-assert

		/* Read PMA calibration status from capability register */
		addr = XCVR_REG_CAPAB_PMA(link);
		read_val = adxcfg_read(dev, lane, addr);
		if ((read_val & XCVR_CAPAB_TX_CAL_BUSY_MASK) == XCVR_CAPAB_TX_CAL_DONE) {
			printf("ch %d TX termination and VOD calib OK\n", lane);
		} else {
			printf("ch %d TX termination and VOD calib error\n", lane);
			err |= 1;
		}
	}

	return err;
}

/***************************************************************************//**
 * @brief xcvr_calib_tx
 *******************************************************************************/
uint8_t xcvr_calib_rx(xcvr_device *dev)
{
	uint8_t link = dev->link_num;
	uint8_t lane;
	uint32_t addr;
	uint8_t mask;
	uint8_t val;
	uint32_t write_val;
	uint32_t read_val;
	uint8_t err = 0;

	for (lane = 0; lane < dev->lanes_per_link; lane++) {
		/* Get AVMM Interface from PreSICE through arbitration register */
		addr = XCVR_REG_ARBITRATION(link);
		mask = XCVR_ARBITRATION_MASK;
		val = XCVR_ARBITRATION_GET_AVMM;
		write_val = (adxcfg_read(dev, lane, addr) & ~mask) | (val & mask);
		adxcfg_write(dev, lane, addr, write_val);

		/* Perform CDR/CMU PLL and RX offset cancellation calibration through
		   PMA calibration enable register */
		addr = XCVR_REG_CALIB_PMA_EN(link);
		mask = XCVR_CALIB_CMU_CDR_PLL_EN_MASK;
		val = XCVR_CALIB_CMU_CDR_PLL_EN;
		write_val = (adxcfg_read(dev, lane, addr) & ~mask) | (val & mask);
		adxcfg_write(dev, lane, addr, write_val);

		/* Set rate switch flag register for CDR charge pump calibration */
		addr = XCVR_REG_RATE_SWITCH_FLAG(link);
		mask = XCVR_RATE_SWITCH_FLAG_MASK;
		val = XCVR_RATE_SWITCH_FLAG_NO_RATE_SWITCH;
		write_val = (adxcfg_read(dev, lane, addr) & ~mask) | (val & mask);
		adxcfg_write(dev, lane, addr, write_val);

		/* Disable tx_cal_busy and enable rx_cal_busy output through
		   capability register */
		addr = XCVR_REG_CAPAB_PMA(link);
		mask = XCVR_CAPAB_RX_CAL_BUSY_EN_MASK | XCVR_CAPAB_TX_CAL_BUSY_EN_MASK;
		val = XCVR_CAPAB_RX_CAL_BUSY_EN | XCVR_CAPAB_TX_CAL_BUSY_DIS;
		write_val = (adxcfg_read(dev, lane, addr) & ~mask) | (val & mask);
		adxcfg_write(dev, lane, addr, write_val);

		/* Release AVMM Interface to PreSICE */
		addr = XCVR_REG_ARBITRATION(link);
		mask = XCVR_ARBITRATION_MASK;
		val = XCVR_ARBITRATION_RELEASE_AVMM;
		write_val = (adxcfg_read(dev, lane, addr) & ~mask) | (val & mask);
		adxcfg_write(dev, lane, addr, write_val);

		mdelay(100);	// Wait 100ms for cal_busy to de-assert

		/* Read PMA calibration status from capability register */
		addr = XCVR_REG_CAPAB_PMA(link);
		read_val = adxcfg_read(dev, lane, addr);
		if ((read_val & XCVR_CAPAB_RX_CAL_BUSY_MASK) == XCVR_CAPAB_RX_CAL_DONE) {
			printf("ch %d CDR/CMU PLL & RX offset calib OK\n", lane);
		} else {
			printf("ch %d CDR/CMU PLL & RX offset calib error\n", lane);
			err |= 1;
		}
	}

	return err;
}

/***************************************************************************//**
 * @brief xcvr_calib_tx
 *******************************************************************************/
int32_t xcvr_setup(mykonosDevice_t *myk_dev)
{
	uint32_t status = 0;
	int32_t timeout;
	int8_t i;
	int32_t	ret = 0;
	xcvr_device rx_xcvr;
	xcvr_device tx_xcvr;
	xcvr_device rx_os_xcvr;

	rx_xcvr.xcvr_base_addr = AD9371_RX_JESD204_LINK_MANAGEMENT_BASE;
	rx_xcvr.link_num = 0;
	rx_xcvr.lanes_per_link = 2;
	rx_xcvr.adxcfg_base_addr[0] = AVL_ADXCFG_0_RCFG_S1_BASE;
	rx_xcvr.adxcfg_base_addr[1] = AVL_ADXCFG_1_RCFG_S1_BASE;

	rx_os_xcvr.xcvr_base_addr = AD9371_RX_OS_JESD204_LINK_MANAGEMENT_BASE;
	rx_os_xcvr.link_num = 0;
	rx_os_xcvr.lanes_per_link = 2;
	rx_os_xcvr.adxcfg_base_addr[0] = AVL_ADXCFG_2_RCFG_S1_BASE;
	rx_os_xcvr.adxcfg_base_addr[1] = AVL_ADXCFG_3_RCFG_S1_BASE;

	tx_xcvr.xcvr_base_addr = AD9371_TX_JESD204_LINK_MANAGEMENT_BASE;
	tx_xcvr.atx_pll_base_addr = AD9371_TX_JESD204_LANE_PLL_RECONFIG_BASE;
	tx_xcvr.link_num = 0;
	tx_xcvr.lanes_per_link = 4;
	tx_xcvr.adxcfg_base_addr[0] = AVL_ADXCFG_0_RCFG_S0_BASE;
	tx_xcvr.adxcfg_base_addr[1] = AVL_ADXCFG_1_RCFG_S0_BASE;
	tx_xcvr.adxcfg_base_addr[2] = AVL_ADXCFG_2_RCFG_S0_BASE;
	tx_xcvr.adxcfg_base_addr[3] = AVL_ADXCFG_3_RCFG_S0_BASE;

	xcvr_write(&tx_xcvr, ADXCVR_REG_RESETN, 0);
	if (atx_pll_calib(&tx_xcvr))
		printf("ATX PLL NOT ready\n");
	if (xcvr_calib_tx(&tx_xcvr))
		printf("TX calib error\n");
	xcvr_write(&tx_xcvr, ADXCVR_REG_RESETN, ADXCVR_RESETN);

	xcvr_write(&rx_xcvr, ADXCVR_REG_RESETN, 0);
	if (xcvr_calib_rx(&rx_xcvr)) {
		printf("RX calib error\n");
	}
	xcvr_write(&rx_xcvr, ADXCVR_REG_RESETN, ADXCVR_RESETN);

	xcvr_write(&rx_os_xcvr, ADXCVR_REG_RESETN, 0);
	if (xcvr_calib_rx(&rx_os_xcvr))
		printf("RX obs calib error\n");
	xcvr_write(&rx_os_xcvr, ADXCVR_REG_RESETN, ADXCVR_RESETN);

	mdelay(1);

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		status = xcvr_read(&rx_xcvr, ADXCVR_REG_STATUS);
		if (status == ADXCVR_STATUS)
			break;
	}
	if (status == 0) {
		status = xcvr_read(&rx_xcvr, ADXCVR_REG_STATUS2);
		printf("RX_XCVR: Link activation error:\n");

		printf("\tRX_XCVR: Link PLL %slocked\n",
			(status & ADXCVR_STATUS2_XCVR(rx_xcvr.lanes_per_link)) ?
				"" : "not ");
		for (i = 0; i < rx_xcvr.lanes_per_link; i++) {
			printf("\tRX_XCVR: Lane %d transceiver %sready\n", i,
				(status & ADXCVR_STATUS2_XCVR(i)) ?
					"" : "not ");
		}
		ret--;
	} else
		printf("RX_XCVR initialization OK\n");

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		status = xcvr_read(&tx_xcvr, ADXCVR_REG_STATUS);
		if (status == ADXCVR_STATUS)
			break;
	}
	if (status == 0) {
		status = xcvr_read(&tx_xcvr, ADXCVR_REG_STATUS2);
		printf("TX_XCVR: Link activation error:\n");

		printf("\tTX_XCVR: Link PLL %slocked\n",
			(status & ADXCVR_STATUS2_XCVR(rx_xcvr.lanes_per_link)) ?
				"" : "not ");
		for (i = 0; i < tx_xcvr.lanes_per_link; i++) {
			printf("\tTX_XCVR: Lane %d transceiver %sready\n", i,
				(status & ADXCVR_STATUS2_XCVR(i)) ?
					"" : "not ");
		}
		ret--;
	} else
		printf("TX_XCVR initialization OK\n");

	timeout = 100;
	while (timeout > 0) {
		mdelay(1);
		timeout--;
		status = xcvr_read(&rx_os_xcvr, ADXCVR_REG_STATUS);
		if (status == ADXCVR_STATUS)
			break;
	}
	if (status == 0) {
		status = xcvr_read(&tx_xcvr, ADXCVR_REG_STATUS2);
		printf("RX_OS_XCVR: Link activation error:\n");

		printf("\tRX_OS_XCVR: Link PLL %slocked\n",
			(status & ADXCVR_STATUS2_XCVR(rx_xcvr.lanes_per_link)) ?
				"" : "not ");
		for (i = 0; i < tx_xcvr.lanes_per_link; i++) {
			printf("\tRX_OS_XCVR: Lane %d transceiver %sready\n", i,
				(status & ADXCVR_STATUS2_XCVR(i)) ?
					"" : "not ");
		}
		ret--;
	} else
		printf("RX_OS_XCVR initialization OK\n");

	return 0;
	return ret;
}

/***************************************************************************//**
 * @brief jesd_setup
 *******************************************************************************/
int32_t jesd_setup(mykonosDevice_t *myk_dev)
{
	jesd_device	rx_jesd;
	jesd_device	tx_jesd;
	jesd_device	rx_os_jesd;
	uint32_t octets_per_frame;
	uint32_t octets_per_multiframe;

	rx_jesd.base_addr = AD9371_RX_JESD204_LINK_RECONFIG_BASE;
	tx_jesd.base_addr = AD9371_TX_JESD204_LINK_RECONFIG_BASE;
	rx_os_jesd.base_addr = AD9371_RX_OS_JESD204_LINK_RECONFIG_BASE;

	octets_per_frame = 0x4;
	octets_per_multiframe = octets_per_frame * myk_dev->rx->framer->K;
	jesd_write(&rx_jesd, JESD204_REG_LINK_CONF0,
			((octets_per_frame-1) << 16) | (octets_per_multiframe - 1));
	jesd_write(&rx_jesd, JESD204_REG_LINK_DISABLE, 0x1);

	octets_per_frame = 0x2;
	octets_per_multiframe = octets_per_frame * myk_dev->tx->deframer->K;
	jesd_write(&tx_jesd, JESD204_REG_LINK_CONF0,
			((octets_per_frame-1) << 16) | (octets_per_multiframe - 1));
	jesd_write(&tx_jesd, JESD204_REG_LINK_DISABLE, 0x1);

	octets_per_frame = 0x2;
	octets_per_multiframe = octets_per_frame * myk_dev->obsRx->framer->K;
	jesd_write(&rx_os_jesd, JESD204_REG_LINK_CONF0,
			((octets_per_frame-1) << 16) | (octets_per_multiframe - 1));
	jesd_write(&rx_os_jesd, JESD204_REG_LINK_DISABLE, 0x1);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_tx_enable
 *******************************************************************************/
int32_t jesd_tx_enable(mykonosDevice_t *myk_dev)
{
	jesd_device	tx_jesd;

	tx_jesd.base_addr = AD9371_TX_JESD204_LINK_RECONFIG_BASE;

	jesd_write(&tx_jesd, JESD204_REG_LINK_DISABLE, 0x0);

	return 0;
}

/***************************************************************************//**
 * @brief jesd_rx_enable
 *******************************************************************************/
int32_t jesd_rx_enable(mykonosDevice_t *myk_dev)
{
	jesd_device	rx_jesd;
	jesd_device	rx_os_jesd;

	rx_jesd.base_addr = AD9371_RX_JESD204_LINK_RECONFIG_BASE;
	rx_os_jesd.base_addr = AD9371_RX_OS_JESD204_LINK_RECONFIG_BASE;

	jesd_write(&rx_jesd, JESD204_REG_LINK_DISABLE, 0x0);
	jesd_write(&rx_os_jesd, JESD204_REG_LINK_DISABLE, 0x0);

	return 0;
}
