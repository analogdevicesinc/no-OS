/***************************************************************************//**
 * @file ad9656.h
 * @brief Header file of AD9656 Driver.
 * @author DHotolea (dan.hotoleanu@analog.com)
 ********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#ifndef AD9656_H_
#define AD9656_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD9656_SPI_CONFIG				        0x000
#define AD9656_REG_CHIP_ID					0x001
#define AD9656_REG_DEVICE_INDEX					0x005
#define AD9656_REG_ADC_TEST_MODE				0x00D
#define AD9656_REG_OUTPUT_MODE					0x014
#define AD9656_REG_USER_TEST_PATTERN_1_LSB			0x19
#define AD9656_REG_USER_TEST_PATTERN_1_MSB      		0x1A
#define AD9656_REG_USER_TEST_PATTERN_2_LSB 			0x1B
#define AD9656_REG_USER_TEST_PATTERN_2_MSB			0x1C
#define AD9656_REG_LINK_CONTROL					0x05F
#define AD9656_REG_JESD204B_LANE_RATE_CTRL			0x021
#define AD9656_REG_JESD204B_PLL_LOCK_STATUS			0x00A
#define AD9656_REG_JESD204B_QUICK_CONFIG			0x05E
#define AD9656_REG_JESD204B_SCR_L                               0x06E
#define AD9656_REG_JESD204B_MF_CTRL				0x070
#define AD9656_REG_JESD204B_M_CTRL                              0x071
#define AD9656_REG_JESD204B_CSN_CONFIG				0x072
#define AD9656_REG_JESD204B_SUBCLASS_CONFIG		        0x073
#define AD9656_REG_JESD204B_LANE_SERD_OUT1_OUT0_ASSIGN		0x082
#define AD9656_REG_JESD204B_LANE_SERD_OUT3_OUT2_ASSIGN		0x083

#define AD9656_CHIP_ID						0x0C0
#define AD9656_TEST_OFF						0x000
#define AD9656_TEST_PN9						0x006
#define AD9656_TEST_PN23					0x005
#define AD9656_TEST_USER_INPUT					0x048
#define AD9656_FORMAT_2S_COMPLEMENT				0x001
#define AD9656_FORMAT_OFFSET_BINARY				0x000

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct ad9656_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
};

struct ad9656_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* Device Settings */
	uint32_t	lane_rate_kbps;
};

struct ad9656_user_input_test_pattern {
	/* User input test pattern 1 */
	uint16_t user_test_pattern1;
	/* User input test pattern 2 */
	uint16_t user_test_pattern2;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t ad9656_reg_read(struct ad9656_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data);

int32_t ad9656_reg_write(struct ad9656_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data);

int32_t ad9656_setup(struct ad9656_dev **device,
		     const struct ad9656_init_param *init_param);

int32_t ad9656_remove(struct ad9656_dev *dev);

int32_t ad9656_JESD204_test(struct ad9656_dev *dev,
			    uint32_t test_mode);

int32_t ad9656_user_input_test(struct ad9656_dev *dev, uint32_t test_mode,
			       struct ad9656_user_input_test_pattern user_input_test_pattern);

#endif
