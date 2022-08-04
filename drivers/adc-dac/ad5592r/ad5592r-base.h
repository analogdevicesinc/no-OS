/***************************************************************************//**
 *   @file   ad5592r-base.h
 *   @brief  Header file of AD5592R Base Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
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
#ifndef AD5592R_BASE_H_
#define AD5592R_BASE_H_

#include "stdint.h"
#include "no_os_delay.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_util.h"
#include <stdbool.h>

#define CH_MODE_UNUSED			0
#define CH_MODE_ADC			1
#define CH_MODE_DAC			2
#define CH_MODE_DAC_AND_ADC		3
#define CH_MODE_GPI			4
#define CH_MODE_GPO			5

#define CH_OFFSTATE_PULLDOWN		0
#define CH_OFFSTATE_OUT_LOW		1
#define CH_OFFSTATE_OUT_HIGH		2
#define CH_OFFSTATE_OUT_TRISTATE	3

enum ad5592r_registers {
	AD5592R_REG_NOOP		= 0x0,
	AD5592R_REG_DAC_READBACK	= 0x1,
	AD5592R_REG_ADC_SEQ		= 0x2,
	AD5592R_REG_CTRL		= 0x3,
	AD5592R_REG_ADC_EN		= 0x4,
	AD5592R_REG_DAC_EN		= 0x5,
	AD5592R_REG_PULLDOWN		= 0x6,
	AD5592R_REG_LDAC		= 0x7,
	AD5592R_REG_GPIO_OUT_EN		= 0x8,
	AD5592R_REG_GPIO_SET		= 0x9,
	AD5592R_REG_GPIO_IN_EN		= 0xA,
	AD5592R_REG_PD			= 0xB,
	AD5592R_REG_OPEN_DRAIN		= 0xC,
	AD5592R_REG_TRISTATE		= 0xD,
	AD5592R_REG_RESET		= 0xF,
};

#define AD5592R_REG_PD_PD_ALL			    NO_OS_BIT(10)
#define AD5592R_REG_PD_EN_REF			    NO_OS_BIT(9)

#define AD5592R_REG_CTRL_ADC_PC_BUFF		    NO_OS_BIT(9)
#define AD5592R_REG_CTRL_ADC_BUFF_EN		    NO_OS_BIT(8)
#define AD5592R_REG_CTRL_CONFIG_LOCK		    NO_OS_BIT(7)
#define AD5592R_REG_CTRL_W_ALL_DACS		    NO_OS_BIT(6)
#define AD5592R_REG_CTRL_ADC_RANGE		    NO_OS_BIT(5)
#define AD5592R_REG_CTRL_DAC_RANGE		    NO_OS_BIT(4)

#define AD5592R_REG_ADC_SEQ_REP			    NO_OS_BIT(9)
#define AD5592R_REG_ADC_SEQ_TEMP_READBACK	    NO_OS_BIT(8)
#define AD5592R_REG_ADC_SEQ_CODE_MSK(x)		    ((x) & 0x0FFF)

#define AD5592R_REG_GPIO_OUT_EN_ADC_NOT_BUSY	    NO_OS_BIT(8)

#define AD5592R_REG_LDAC_IMMEDIATE_OUT		    0x00
#define AD5592R_REG_LDAC_INPUT_REG_ONLY		    0x01
#define AD5592R_REG_LDAC_INPUT_REG_OUT		    0x02

#define INTERNAL_VREF_VOLTAGE			    2.5

struct ad5592r_dev;

struct ad5592r_rw_ops {
	int32_t (*write_dac)(struct ad5592r_dev *dev, uint8_t chan,
			     uint16_t value);
	int32_t (*read_adc)(struct ad5592r_dev *dev, uint8_t chan,
			    uint16_t *value);
	int32_t(*multi_read_adc)(struct ad5592r_dev *dev,
				 uint16_t chans, uint16_t *value);
	int32_t (*reg_write)(struct ad5592r_dev *dev, uint8_t reg,
			     uint16_t value);
	int32_t (*reg_read)(struct ad5592r_dev *dev, uint8_t reg,
			    uint16_t *value);
	int32_t (*gpio_read)(struct ad5592r_dev *dev, uint8_t *value);
};

struct ad5592r_init_param {
	bool int_ref;
};

struct ad5592r_dev {
	const struct ad5592r_rw_ops *ops;
	struct no_os_i2c_desc *i2c;
	struct no_os_spi_desc *spi;
	uint16_t spi_msg;
	uint8_t num_channels;
	uint16_t cached_dac[8];
	uint16_t cached_gp_ctrl;
	uint8_t channel_modes[8];
	uint8_t channel_offstate[8];
	uint8_t gpio_out;
	uint8_t gpio_in;
	uint8_t gpio_val;
	uint8_t ldac_mode;
};

int32_t ad5592r_base_reg_write(struct ad5592r_dev *dev, uint8_t reg,
			       uint16_t value);
int32_t ad5592r_base_reg_read(struct ad5592r_dev *dev, uint8_t reg,
			      uint16_t *value);
int32_t ad5592r_gpio_get(struct ad5592r_dev *dev, uint8_t offset);
int32_t ad5592r_gpio_set(struct ad5592r_dev *dev, uint8_t offset,
			 int32_t value);
int32_t ad5592r_gpio_direction_input(struct ad5592r_dev *dev, uint8_t offset);
int32_t ad5592r_gpio_direction_output(struct ad5592r_dev *dev,
				      uint8_t offset, int32_t value);
int32_t ad5592r_software_reset(struct ad5592r_dev *dev);
int32_t ad5592r_set_channel_modes(struct ad5592r_dev *dev);
int32_t ad5592r_reset_channel_modes(struct ad5592r_dev *dev);

#endif /* AD5592R_BASE_H_ */
