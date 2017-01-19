/***************************************************************************//**
 * @file ad9680.h
 * @brief Header file of AD9680 Driver.
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
#ifndef AD9680_H_
#define AD9680_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "altxil_base.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD9680_REG_INTERFACE_CONF_A				0x000
#define AD9680_REG_INTERFACE_CONF_B				0x001
#define AD9680_REG_CHIP_ID_LOW					0x004
#define AD9680_REG_CHIP_ID_HIGH					0x005
#define AD9680_REG_DEVICE_INDEX					0x008
#define AD9680_REG_CHIP_DEC_RATIO				0x201
#define AD9680_REG_ADC_TEST_MODE				0x550
#define AD9680_REG_OUTPUT_MODE					0x561
#define AD9680_REG_LINK_CONTROL					0x571
#define AD9680_REG_JESD204B_LANE_RATE_CTRL			0x56e
#define AD9680_REG_JESD204B_PLL_LOCK_STATUS			0x56f
#define AD9680_REG_JESD204B_QUICK_CONFIG			0x570
#define AD9680_REG_JESD204B_MF_CTRL			      0x58d
#define AD9680_REG_JESD204B_CSN_CONFIG				0x58f
#define AD9680_REG_JESD204B_SUBCLASS_CONFIG		0x590
#define AD9680_REG_JESD204B_LANE_SERD_OUT0_ASSIGN		0x5b2
#define AD9680_REG_JESD204B_LANE_SERD_OUT1_ASSIGN		0x5b3
#define AD9680_REG_JESD204B_LANE_SERD_OUT2_ASSIGN		0x5b5
#define AD9680_REG_JESD204B_LANE_SERD_OUT3_ASSIGN		0x5b6

#define AD9680_CHIP_ID						0x0C5

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

typedef struct {
	uint32_t	lane_rate_kbps;
} ad9680_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad9680_spi_read(spi_device *dev,
		uint16_t reg_addr,
		uint8_t *reg_data);
int32_t ad9680_spi_write(spi_device *dev,
		uint16_t reg_addr,
		uint8_t reg_data);
int32_t ad9680_setup(spi_device *dev,
		ad9680_init_param init_param);

#endif
