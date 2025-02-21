/***************************************************************************//**
* @file ad9434.h
* @brief Header file of AD9434 Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef AD9434_H_
#define AD9434_H_

#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_spi.h"

#define AD9434_REG_CHIP_PORT_CONF	0x00
#define AD9434_REG_CHIP_ID		0x01
#define AD9434_REG_CHIP_GRADE		0x02
#define AD9434_REG_CHAN_INDEX		0x05
#define AD9434_REG_TRANSFER		0xFF
#define AD9434_REG_MODES		0x08
#define AD9434_REG_TEST_IO		0x0D
#define AD9434_REG_ADC_INPUT		0x0F
#define AD9434_REG_OFFSET		0x10
#define AD9434_REG_OUTPUT_MODE		0x14
#define AD9434_REG_OUTPUT_ADJUST	0x15
#define AD9434_REG_OUTPUT_PHASE		0x16
#define AD9434_REG_OUTPUT_DELAY		0x17
#define AD9434_REG_VREF			0x18
#define AD9434_REG_ANALOG_INPUT		0x2C

/* ADC_REG_TRANSFER */
#define TRANSFER_SYNC			0x1

/* AD9434_REG_TEST_IO */
#define TESTMODE_OFF			0x0
#define TESTMODE_MIDSCALE_SHORT		0x1
#define TESTMODE_POS_FULLSCALE		0x2
#define TESTMODE_NEG_FULLSCALE		0x3
#define TESTMODE_ALT_CHECKERBOARD	0x4
#define TESTMODE_PN23_SEQ		0x5
#define TESTMODE_PN9_SEQ		0x6
#define TESTMODE_ONE_ZERO_TOGGLE	0x7
#define TESTMODE_USER_DEFINED		0x8

/* ADC_REG_OUTPUT_MODE */
#define OUTPUT_MODE_OFFSET_BINARY	0x0
#define OUTPUT_MODE_TWOS_COMPLEMENT	0x1
#define OUTPUT_MODE_GRAY_CODE		0x2

/* ADC_REG_OUTPUT_PHASE */
#define OUTPUT_EVEN_ODD_MODE_EN		0x20
#define INVERT_DCO_CLK			0x80

#define AD9434_CHIP_ID			0x6A
#define AD9434_DEF_OUTPUT_MODE		0x00

struct ad9434_dev {
	/* SPI */
	struct no_os_spi_desc *spi_desc;
};

struct ad9434_init_param {
	/* SPI */
	struct no_os_spi_init_param spi_init;
};

int32_t ad9434_spi_read(struct ad9434_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data);
int32_t ad9434_spi_write(struct ad9434_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data);
int32_t ad9434_testmode_set(struct ad9434_dev *dev,
			    uint8_t mode);
int32_t ad9434_setup(struct ad9434_dev **device,
		     struct ad9434_init_param init_param);
int32_t ad9434_outputmode_set(struct ad9434_dev *dev,
			      uint8_t mode);
int32_t ad9434_remove(struct ad9434_dev *dev);
#endif
