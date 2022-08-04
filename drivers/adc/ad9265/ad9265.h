/***************************************************************************//**
* @file ad9265.h
* @brief Header file of AD9265 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
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
#ifndef AD9265_H_
#define AD9265_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD9265_REG_CHIP_PORT_CONF	0x00
#define AD9265_REG_CHIP_ID		0x01
#define AD9265_REG_CHIP_GRADE		0x02
#define AD9265_REG_CHAN_INDEX		0x05
#define AD9265_REG_TRANSFER		0xFF
#define AD9265_REG_MODES		0x08
#define AD9265_REG_TEST_IO		0x0D
#define AD9265_REG_ADC_INPUT		0x0F
#define AD9265_REG_OFFSET		0x10
#define AD9265_REG_OUTPUT_MODE		0x14
#define AD9265_REG_OUTPUT_ADJUST	0x15
#define AD9265_REG_OUTPUT_PHASE		0x16
#define AD9265_REG_OUTPUT_DELAY		0x17
#define AD9265_REG_VREF			0x18
#define AD9265_REG_ANALOG_INPUT		0x2C

/* ADC_REG_TRANSFER */
#define TRANSFER_SYNC			0x1

/* AD9265_REG_TEST_IO */
#define TESTMODE_OFF			0x0
#define TESTMODE_MIDSCALE_SHORT		0x1
#define TESTMODE_POS_FULLSCALE		0x2
#define TESTMODE_NEG_FULLSCALE		0x3
#define TESTMODE_ALT_CHECKERBOARD	0x4
#define TESTMODE_PN23_SEQ		0x5
#define TESTMODE_PN9_SEQ		0x6
#define TESTMODE_ONE_ZERO_TOGGLE	0x7

/* ADC_REG_OUTPUT_MODE */
#define OUTPUT_MODE_OFFSET_BINARY	0x0
#define OUTPUT_MODE_TWOS_COMPLEMENT	0x1
#define OUTPUT_MODE_GRAY_CODE		0x2

/* ADC_REG_OUTPUT_PHASE */
#define OUTPUT_EVEN_ODD_MODE_EN		0x20
#define INVERT_DCO_CLK			0x80

#define AD9265_CHIP_ID			0x64
#define AD9265_DEF_OUTPUT_MODE		0x40

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct ad9265_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* Device Settings */
	uint8_t		output_mode;
	uint8_t		dco;			// data clock output
	uint8_t		dco_en;			// dco enable
	uint8_t		nb_lanes;		// number of lanes
};

struct ad9265_dev {
	/* SPI */
	struct no_os_spi_desc *spi_desc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad9265_testmode_set(struct ad9265_dev *dev,
			    uint8_t mode);
int32_t ad9265_setup(struct ad9265_dev **device,
		     struct ad9265_init_param init_param,
		     struct axi_adc core);
int32_t ad9265_remove(struct ad9265_dev *dev);
int32_t ad9265_calibrate(struct ad9265_dev *dev,
			 struct ad9265_init_param init_param,
			 struct axi_adc core);
int32_t ad9265_outputmode_set(struct ad9265_dev *dev,
			      uint8_t mode);
int32_t ad9265_spi_write(struct ad9265_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data);
int32_t ad9265_spi_read(struct ad9265_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data);
#endif
