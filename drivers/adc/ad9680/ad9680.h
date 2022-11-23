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
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_spi.h"
#include "jesd204.h"
#include "no_os_util.h"

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
#define AD9680_REG_JESD_LINK_CTRL1_REG    			0x571
#define AD9680_REG_JESD204B_MF_CTRL				0x58d
#define AD9680_REG_JESD204B_CSN_CONFIG				0x58f
#define AD9680_REG_JESD204B_SUBCLASS_CONFIG			0x590
#define AD9680_REG_JESD204B_LANE_SERD_OUT0_ASSIGN		0x5b2
#define AD9680_REG_JESD204B_LANE_SERD_OUT1_ASSIGN		0x5b3
#define AD9680_REG_JESD204B_LANE_SERD_OUT2_ASSIGN		0x5b5
#define AD9680_REG_JESD204B_LANE_SERD_OUT3_ASSIGN		0x5b6

#define AD9680_CHIP_ID						0x0C5
#define AD9680_TEST_OFF						0x000
#define AD9680_TEST_PN9						0x006
#define AD9680_TEST_PN23					0x005
#define AD9680_TEST_RAMP					0x00f
#define AD9680_FORMAT_2S_COMPLEMENT				0x001
#define AD9680_FORMAT_OFFSET_BINARY				0x000
#define AD9680_JESD_LINK_PDN     	     NO_OS_BIT(0)

#define AD9680_SYSREF_NONE 0	/* No SYSREF Support */
#define AD9680_SYSREF_ONESHOT 2	/* ONE-SHOT SYSREF */
#define AD9680_SYSREF_CONT 1	/* Continuous Sysref Synchronisation */
#define AD9680_SYSREF_MON 3	/* SYSREF monitor Mode */

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct ad9680_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;

	struct jesd204_dev *jdev;
	struct jesd204_link jesd204_link;

	unsigned long long sampling_frequency_hz;
	unsigned long dcm;

	unsigned long sysref_mode;
};

struct ad9680_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* Device Settings */
	uint32_t	lane_rate_kbps;

	struct jesd204_link jesd204_link;
	unsigned long long sampling_frequency_hz;
	unsigned long dcm;
	unsigned long sysref_mode;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t ad9680_spi_read(struct ad9680_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data);

int32_t ad9680_spi_write(struct ad9680_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data);

int32_t ad9680_setup(struct ad9680_dev **device,
		     const struct ad9680_init_param *init_param);

/* Initialize ad9680_dev, JESD FSM ON */
int32_t ad9680_setup_jesd_fsm(struct ad9680_dev **device,
			      const struct ad9680_init_param *init_param);

int32_t ad9680_remove(struct ad9680_dev *dev);

int32_t ad9680_test(struct ad9680_dev *dev,
		    uint32_t test_mode);

#endif
