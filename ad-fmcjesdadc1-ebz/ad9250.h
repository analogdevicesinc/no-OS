/***************************************************************************//**
* @file ad9250.h
* @brief Header file of AD9250 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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
#ifndef AD9250_H_
#define AD9250_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD9250_REG_CHIP_PORT_CONF				0x00
#define AD9250_REG_CHIP_ID					0x01
#define AD9250_REG_POWER_MODE					0x08
#define AD9250_REG_PLL_STATUS					0x0A
#define AD9250_REG_TEST_CNTRL					0x0D
#define AD9250_REG_OUTPUT_MODE					0x14
#define AD9250_REG_OUTPUT_ADJUST				0x15
#define AD9250_REG_SYSREF_CONTROL				0x3A
#define AD9250_REG_JESD204B_QUICK_CONFIG			0x5E
#define AD9250_REG_JESD204B_LINK_CNTRL_1			0x5F
#define AD9250_REG_204B_LID_CONFIG_0				0x66
#define AD9250_REG_204B_LID_CONFIG_1				0x67
#define AD9250_REG_204B_PARAM_SCRAMBLE_LANES			0x6E
#define AD9250_REG_204B_PARAM_K					0x70
#define AD9250_REG_JESD204B_CONFIGURATION			0x72
#define AD9250_REG_JESD204B_LANE_POWER_MODE			0x80
#define AD9250_REG_TRANSFER					0xFF

#define AD9250_CHIP_ID						0xB9

#define AD9250_TEST_OFF						0x00
#define AD9250_TEST_MID_SCALE					0x01
#define AD9250_TEST_POS_FSCALE					0x02
#define AD9250_TEST_NEG_FSCALE					0x03
#define AD9250_TEST_CHECKBOARD					0x04
#define AD9250_TEST_PNLONG					0x05
#define AD9250_TEST_ONE2ZERO					0x07
#define AD9250_TEST_PATTERN					0x08
#define AD9250_TEST_RAMP					0x0F

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct ad9250_dev {
	/* SPI */
	spi_desc      *spi_dev;
	/* Device properties */
	uint8_t       id_no;
};


struct ad9250_init_param {
	/* SPI */
	spi_init_param spi_init;
	/* Device properties */
	uint32_t	   lane_rate_kbps;
	uint8_t		   id_no;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad9250_spi_read(struct ad9250_dev *dev,
						uint16_t reg_addr,
						uint8_t *reg_data);
int32_t ad9250_spi_write(struct ad9250_dev *dev,
						 uint16_t reg_addr,
						 uint8_t reg_data);
int32_t ad9250_setup(struct ad9250_dev **device,
					 struct ad9250_init_param init_param);
int32_t ad9250_test(struct ad9250_dev *dev,
					uint32_t test_mode);
int32_t ad9250_remove(struct ad9250_dev *dev);

#endif
