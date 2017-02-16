/***************************************************************************//**
 * @file ad9625.h
 * @brief Header file of AD9625 Driver.
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
#ifndef AD9625_H_
#define AD9625_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD9625_REG_CHIP_PORT_CONF				0x000
#define AD9625_REG_CHIP_ID					0x001
#define AD9625_REG_POWER_MODE					0x008
#define AD9625_REG_PLL_STATUS					0x00A
#define AD9625_REG_TEST_CNTRL					0x00D
#define AD9625_REG_OUTPUT_MODE					0x014
#define AD9625_REG_OUTPUT_ADJUST				0x015
#define AD9625_REG_JESD204B_LINK_CNTRL_1			0x05F
#define AD9625_REG_JESD204B_LANE_POWER_MODE			0x080
#define AD9625_REG_TRANSFER					0x0FF
#define AD9625_REG_DIVCLK_OUT_CNTRL				0x120

#define AD9625_CHIP_ID						0x041

#define AD9625_TEST_OFF						0x000
#define AD9625_TEST_MID_SCALE					0x001
#define AD9625_TEST_POS_FSCALE					0x002
#define AD9625_TEST_NEG_FSCALE					0x003
#define AD9625_TEST_CHECKBOARD					0x004
#define AD9625_TEST_PNLONG					0x005
#define AD9625_TEST_ONE2ZERO					0x007
#define AD9625_TEST_PATTERN					0x008
#define AD9625_TEST_RAMP					0x00F

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef struct {
	/* Device Settings */
	uint32_t	lane_rate_kbps;
	uint32_t	test_samples[4];
} ad9625_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad9625_spi_read(spi_device *dev,
		uint16_t reg_addr,
		uint8_t *reg_data);
int32_t ad9625_spi_write(spi_device *dev,
		uint16_t reg_addr,
		uint8_t reg_data);
int32_t ad9625_setup(spi_device *dev);

int32_t ad9625_test(spi_device *dev, uint32_t test_mode);

#endif
