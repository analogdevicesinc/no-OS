/***************************************************************************//**
 * @file ad9144.c
 * @brief Implementation of AD9144 Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
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
#include "ad9144.h"

/***************************************************************************//**
 * @brief ad9144_spi_read
 *******************************************************************************/
int32_t ad9144_spi_read(spi_device *dev,
		uint16_t reg_addr,
		uint8_t *reg_data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	ret = ad_spi_xfer(dev, buf, 3);
	*reg_data = buf[2];

	return ret;
}

/***************************************************************************//**
 * @brief ad9144_spi_write
 *******************************************************************************/
int32_t ad9144_spi_write(spi_device *dev,
		uint16_t reg_addr,
		uint8_t reg_data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	ret = ad_spi_xfer(dev, buf, 3);

	return ret;
}

/***************************************************************************//**
 * @brief ad9144_setup
 *******************************************************************************/
int32_t ad9144_setup(spi_device *dev,
		ad9144_init_param init_param)
{
	uint8_t chip_id;
	uint8_t scratchpad;
	uint8_t pll_stat;
	uint8_t cal_stat;
	int32_t ret;

	ad9144_spi_read(dev, REG_SPI_PRODIDL, &chip_id);
	if(chip_id != AD9144_CHIP_ID)
	{
		xil_printf("Error: Invalid CHIP ID (0x%x).\n\r", chip_id);
		return -1;
	}

	ad9144_spi_write(dev, REG_SPI_SCRATCHPAD, 0xAD);
	ad9144_spi_read(dev, REG_SPI_SCRATCHPAD, &scratchpad);
	if(scratchpad != 0xAD)
	{
		xil_printf("Error: scratchpad (0x%x).\n\r", scratchpad);
		return -1;
	}

	// power-up and dac initialization
	mdelay(5);

	ad9144_spi_write(dev, REG_SPI_INTFCONFA, SOFTRESET_M | SOFTRESET);	// reset
	ad9144_spi_write(dev, REG_SPI_INTFCONFA, 0x00);	// reset

	mdelay(4);

	ad9144_spi_write(dev, 0x011, 0x00);	// dacs - power up everything
	ad9144_spi_write(dev, 0x080, 0x00);	// clocks - power up everything
	ad9144_spi_write(dev, 0x081, 0x00);	// sysref - power up/falling edge

	// required device configurations

	ad9144_spi_write(dev, 0x12d, 0x8b);	// data-path
	ad9144_spi_write(dev, 0x146, 0x01);	// data-path
	ad9144_spi_write(dev, 0x2a4, 0xff);	// clock
	ad9144_spi_write(dev, 0x1c4, 0x73);	// dac-pll
	ad9144_spi_write(dev, 0x291, 0x49);	// serde-pll
	ad9144_spi_write(dev, 0x29c, 0x24);	// serde-pll
	ad9144_spi_write(dev, 0x29f, 0x73);	// serde-pll
	ad9144_spi_write(dev, 0x232, 0xff);	// jesd
	ad9144_spi_write(dev, 0x333, 0x01);	// jesd

	// digital data path

	ad9144_spi_write(dev, 0x112, 0x00);	// interpolation (bypass)
	ad9144_spi_write(dev, 0x110, 0x00);	// 2's complement

	// transport layer

	ad9144_spi_write(dev, 0x200, 0x00);	// phy - power up
	ad9144_spi_write(dev, 0x201, 0x00);	// phy - power up
	ad9144_spi_write(dev, 0x300, 0x01);	// single link - link 0
	ad9144_spi_write(dev, 0x450, 0x00);	// device id (0x400)
	ad9144_spi_write(dev, 0x451, 0x00);	// bank id (0x401)
	ad9144_spi_write(dev, 0x452, 0x04);	// lane-id (0x402)
	ad9144_spi_write(dev, 0x453, 0x83);	// descrambling, 4 lanes
	ad9144_spi_write(dev, 0x454, 0x00);	// octects per frame per lane (1)
	ad9144_spi_write(dev, 0x455, 0x1f);	// mult-frame - framecount (32)
	ad9144_spi_write(dev, 0x456, 0x01);	// no-of-converters (2)
	ad9144_spi_write(dev, 0x457, 0x0f);	// no CS bits, 16bit dac
	ad9144_spi_write(dev, 0x458, 0x2f);	// subclass 1, 16bits per sample
	ad9144_spi_write(dev, 0x459, 0x20);	// jesd204b, 1 samples per converter per device
	ad9144_spi_write(dev, 0x45a, 0x80);	// HD mode, no CS bits
	ad9144_spi_write(dev, 0x45d, 0x49);	// check-sum of 0x450 to 0x45c
	ad9144_spi_write(dev, 0x46c, 0x0f);	// enable deskew for all lanes
	ad9144_spi_write(dev, 0x476, 0x01);	// frame - bytecount (1)
	ad9144_spi_write(dev, 0x47d, 0x0f);	// enable all lanes

	// physical layer

	ad9144_spi_write(dev, 0x2aa, 0xb7);	// jesd termination
	ad9144_spi_write(dev, 0x2ab, 0x87);	// jesd termination
	ad9144_spi_write(dev, 0x2b1, 0xb7);	// jesd termination
	ad9144_spi_write(dev, 0x2b2, 0x87);	// jesd termination
	ad9144_spi_write(dev, 0x2a7, 0x01);	// input termination calibration
	ad9144_spi_write(dev, 0x2ae, 0x01);	// input termination calibration
	ad9144_spi_write(dev, 0x314, 0x01);	// pclk == qbd master clock
	if (init_param.lane_rate_kbps < 2880000)
		ad9144_spi_write(dev, 0x230, 0x2A);		// CDR_OVERSAMP
	else
		if (init_param.lane_rate_kbps > 5520000)
			ad9144_spi_write(dev, 0x230, 0x28);	// ENHALFRATE
		else
			ad9144_spi_write(dev, 0x230, 0x08);
	ad9144_spi_write(dev, 0x206, 0x00);	// cdr reset
	ad9144_spi_write(dev, 0x206, 0x01);	// cdr reset
	if (init_param.lane_rate_kbps < 2880000)
		ad9144_spi_write(dev, 0x289, 0x06);		// data-rate < 2.88 Gbps
	else
		if (init_param.lane_rate_kbps > 5520000)
			ad9144_spi_write(dev, 0x289, 0x04);	// data-rate > 5.52 Gbps
		else
			ad9144_spi_write(dev, 0x289, 0x05);
	ad9144_spi_write(dev, 0x280, 0x01);	// enable serdes pll
	ad9144_spi_write(dev, 0x280, 0x05);	// enable serdes calibration
	mdelay(20);

	ad9144_spi_read(dev, 0x281, &pll_stat);
	xil_printf("AD9144 PLL/link %s.\n\r", pll_stat & 0x01 ? "ok" : "errors");

	ad9144_spi_write(dev, 0x268, 0x62);	// equalizer

	// cross-bar

	ad9144_spi_write(dev, REG_XBAR_LN_0_1, SRC_LANE0(init_param.jesd_xbar_lane0_sel) |
			SRC_LANE1(init_param.jesd_xbar_lane1_sel));	// lane selects
	ad9144_spi_write(dev, REG_XBAR_LN_2_3, SRC_LANE2(init_param.jesd_xbar_lane2_sel) |
			SRC_LANE3(init_param.jesd_xbar_lane3_sel));	// lane selects

	// data link layer

	ad9144_spi_write(dev, 0x301, 0x01);	// subclass-1
	ad9144_spi_write(dev, 0x304, 0x00);	// lmfc delay
	ad9144_spi_write(dev, 0x305, 0x00);	// lmfc delay
	ad9144_spi_write(dev, 0x306, 0x0a);	// receive buffer delay
	ad9144_spi_write(dev, 0x307, 0x0a);	// receive buffer delay
	ad9144_spi_write(dev, 0x03a, 0x01);	// sync-oneshot mode
	ad9144_spi_write(dev, 0x03a, 0x81);	// sync-enable
	ad9144_spi_write(dev, 0x03a, 0xc1);	// sysref-armed
	ad9144_spi_write(dev, 0x300, 0x01);	// enable link

	// dac calibration

	ad9144_spi_write(dev, 0x0e7, 0x38);	// set calibration clock to 1m
	ad9144_spi_write(dev, 0x0ed, 0xa6);	// use isb reference of 38 to set cal
	ad9144_spi_write(dev, 0x0e8, 0x03);	// cal 2 dacs at once
	ad9144_spi_write(dev, 0x0e9, 0x01);	// single cal enable
	ad9144_spi_write(dev, 0x0e9, 0x03);	// single cal start
	mdelay(10);

	ad9144_spi_write(dev, 0x0e8, 0x01);	// read dac-0
	ad9144_spi_read(dev, 0x0e9, &cal_stat);
	xil_printf("AD9144 dac-0 calibration %s.\n\r", (cal_stat & 0xc0) == 0x80 ? "ok" : "failed");

	ad9144_spi_write(dev, 0x0e8, 0x02);	// read dac-1
	ad9144_spi_read(dev, 0x0e9, &cal_stat);
	xil_printf("AD9144 dac-1 calibration %s.\n\r", (cal_stat & 0xc0) == 0x80 ? "ok" : "failed");

	ad9144_spi_write(dev, 0x0e7, 0x30);	// turn off cal clock

	xil_printf("AD9144 successfully initialized.\n\r");

	return ret;
}
