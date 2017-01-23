/***************************************************************************//**
* @file ad9152.c
* @brief Implementation of AD9152 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2015-2016(c) Analog Devices, Inc.
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
#include "ad9152.h"

/***************************************************************************//**
* @brief ad9152_spi_read
*******************************************************************************/
int32_t ad9152_spi_read(spi_device *dev,
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
* @brief ad9152_spi_write
*******************************************************************************/
int32_t ad9152_spi_write(spi_device *dev,
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
* @brief ad9152_setup
*******************************************************************************/
int32_t ad9152_setup(spi_device *dev,
					 ad9152_init_param init_param)
{
	uint8_t chip_id;
	uint8_t pll_stat;
	int32_t ret;

  ret = 0;

	ad9152_spi_read(dev, REG_SPI_PRODIDL, &chip_id);
	if(chip_id != AD9152_CHIP_ID)
	{
		ad_printf("AD9152: Invalid CHIP ID (0x%x)!\n", chip_id);
		return -1;
	}

	// power-up and dac initialization
	mdelay(5);

	ad9152_spi_write(dev, REG_SPI_INTFCONFA, SOFTRESET_M | SOFTRESET);	// reset
	ad9152_spi_write(dev, REG_SPI_INTFCONFA, 0x00);	// reset

	mdelay(4);

	ad9152_spi_write(dev, 0x011, 0x00);	// dacs - power up everything
	ad9152_spi_write(dev, 0x080, 0x00);	// clocks - power up everything
	ad9152_spi_write(dev, 0x081, 0x00);	// sysref - power up/falling edge

	// digital data path

	ad9152_spi_write(dev, 0x112, 0x00);	// interpolation (bypass)
	ad9152_spi_write(dev, 0x110, 0x00);	// 2's complement

	// transport layer

	ad9152_spi_write(dev, 0x200, 0x00);	// phy - power up
	ad9152_spi_write(dev, 0x201, 0x00);	// phy - power up
	ad9152_spi_write(dev, 0x230, 0x28);	// half-rate CDR
	ad9152_spi_write(dev, 0x312, 0x20);	// half-rate CDR
	ad9152_spi_write(dev, 0x300, 0x01);	// single link - link 0
	ad9152_spi_write(dev, 0x450, 0x00);	// device id (0x400)
	ad9152_spi_write(dev, 0x451, 0x00);	// bank id (0x401)
	ad9152_spi_write(dev, 0x452, 0x04);	// lane-id (0x402)
	ad9152_spi_write(dev, 0x453, 0x83);	// descrambling, 4 lanes
	ad9152_spi_write(dev, 0x454, 0x00);	// octects per frame per lane (1)
	ad9152_spi_write(dev, 0x455, 0x1f);	// mult-frame - framecount (32)
	ad9152_spi_write(dev, 0x456, 0x01);	// no-of-converters (2)
	ad9152_spi_write(dev, 0x457, 0x0f);	// no CS bits, 16bit dac
	ad9152_spi_write(dev, 0x458, 0x2f);	// subclass 1, 16bits per sample
	ad9152_spi_write(dev, 0x459, 0x20);	// jesd204b, 1 samples per converter per device
	ad9152_spi_write(dev, 0x45a, 0x80);	// HD mode, no CS bits
	ad9152_spi_write(dev, 0x45d, 0x49);	// check-sum of 0x450 to 0x45c
	ad9152_spi_write(dev, 0x478, 0x01);	// ilas mf count
	ad9152_spi_write(dev, 0x46c, 0x0f);	// enable deskew for all lanes
	ad9152_spi_write(dev, 0x476, 0x01);	// frame - bytecount (1)
	ad9152_spi_write(dev, 0x47d, 0x0f);	// enable all lanes

	// physical layer
	ad9152_spi_write(dev, 0x2a6, 0x08);
	ad9152_spi_write(dev, 0x248, 0xaa);
	ad9152_spi_write(dev, 0x2aa, 0xb7);	// jesd termination
	ad9152_spi_write(dev, 0x2ab, 0x87);	// jesd termination
	ad9152_spi_write(dev, 0x2a7, 0x01);	// input termination calibration
	ad9152_spi_write(dev, 0x314, 0x01);	// pclk == qbd master clock
	ad9152_spi_write(dev, 0x230, 0x28);	// cdr mode - halfrate, no division
	ad9152_spi_write(dev, 0x206, 0x00);	// cdr reset
	ad9152_spi_write(dev, 0x206, 0x01);	// cdr reset
	ad9152_spi_write(dev, 0x289, 0x04);	// data-rate == 10Gbps
	ad9152_spi_write(dev, 0x280, 0x01);	// enable serdes pll
	ad9152_spi_write(dev, 0x280, 0x05);	// enable serdes calibration
	mdelay(20);

	ad9152_spi_read(dev, 0x281, &pll_stat);
  if (pll_stat == 0)
  {
	  ad_printf("AD9152: PLL is NOT locked!.\n");
    ret = -1;
  }

	ad9152_spi_write(dev, 0x268, 0x62);	// equalizer


	// data link layer

	ad9152_spi_write(dev, 0x301, 0x01);	// subclass-1
	ad9152_spi_write(dev, 0x304, 0x00);	// lmfc delay
	ad9152_spi_write(dev, 0x306, 0x0a);	// receive buffer delay
	ad9152_spi_write(dev, 0x03a, 0x01);	// sync-oneshot mode
	ad9152_spi_write(dev, 0x03a, 0x81);	// sync-enable
	ad9152_spi_write(dev, 0x03a, 0xc1);	// sysref-armed
	ad9152_spi_write(dev, 0x300, 0x01);	// enable link

	ad9152_spi_write(dev, 0x0e7, 0x30);	// turn off cal clock

	return ret;
}

/***************************************************************************//**
* @brief ad9152_setup
*******************************************************************************/
int32_t ad9152_datapath_prbs(spi_device *dev, uint32_t prbs_type)
{

  uint8_t status;
  int32_t ret;

  status = 0;
  ret = 0;

  ad9152_spi_write(dev, 0x008, 0x01); // dac-sel
	ad9152_spi_write(dev, REG_PRBS, ((prbs_type << 2) | 0x03));
  mdelay(1);

	ad9152_spi_write(dev, REG_PRBS, ((prbs_type << 2) | 0x01));
  mdelay(100);

  ad9152_spi_read(dev, REG_PRBS, &status);
  if ((status & 0xc0) != 0xc0)
  {
	  ad_printf("AD9152: PRBS OUT OF SYNC (%x)!.\n", status);
    ret = -1;
  }
  ad9152_spi_read(dev, REG_PRBS_ERROR_I, &status);
  if (status != 0x00)
  {
	  ad_printf("AD9152: PRBS I channel ERRORS (%x)!.\n", status);
    ret = -1;
  }
  ad9152_spi_read(dev, REG_PRBS_ERROR_Q, &status);
  if (status != 0x00)
  {
	  ad_printf("AD9152: PRBS Q channel ERRORS (%x)!.\n", status);
    ret = -1;
  }

  return(ret);
}

/***************************************************************************//**
* @brief ad9152_setup
*******************************************************************************/
int32_t ad9152_status(spi_device *dev)
{

  uint8_t status;
  int32_t ret;

  status = 0;
  ret = 0;

  // check for jesd status on all lanes
  // failures on top are 100% guaranteed to make subsequent status checks fail

  ad9152_spi_read(dev, REG_CODEGRPSYNCFLG, &status);
  if (status != 0x0f)
  {
	  ad_printf("AD9152: CGS NOT received (%x)!.\n", status);
    ret = -1;
  }
  ad9152_spi_read(dev, REG_INITLANESYNCFLG, &status);
  if (status != 0x0f)
  {
	  ad_printf("AD9152: ILAS NOT received (%x)!.\n", status);
    ret = -1;
  }
  ad9152_spi_read(dev, REG_FRAMESYNCFLG, &status);
  if (status != 0x0f)
  {
	  ad_printf("AD9152: framer OUT OF SYNC (%x)!.\n", status);
    ret = -1;
  }
  ad9152_spi_read(dev, REG_GOODCHKSUMFLG, &status);
  if (status != 0x0f)
  {
	  ad_printf("AD9152: check-sum MISMATCH (%x)!.\n", status);
    ret = -1;
  }

  return(ret);
}

