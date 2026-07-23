/***************************************************************************//**
 *   @file   ad7291.h
 *   @brief  Header file of AD7291 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
 *
*******************************************************************************/
#ifndef _AD7291_H_
#define _AD7291_H_

#include <stdint.h>
#include "no_os_i2c.h"

#define AD7291_NUM_CHANNELS		8
#define AD7291_RESOLUTION		12
#define AD7291_INTERNAL_VREF_MV		2500

#define AD7291_REG_COMMAND			0x00
#define AD7291_REG_VOLTAGE			0x01
#define AD7291_REG_T_SENSE			0x02
#define AD7291_REG_T_AVERAGE			0x03
#define AD7291_REG_CH0_DATA_HIGH		0x04
#define AD7291_REG_CH0_DATA_LOW			0x05
#define AD7291_REG_CH0_HYST			0x06
#define AD7291_REG_CH1_DATA_HIGH		0x07
#define AD7291_REG_CH1_DATA_LOW			0x08
#define AD7291_REG_CH1_HYST			0x09
#define AD7291_REG_CH2_DATA_HIGH		0x0A
#define AD7291_REG_CH2_DATA_LOW			0x0B
#define AD7291_REG_CH2_HYST			0x0C
#define AD7291_REG_CH3_DATA_HIGH		0x0D
#define AD7291_REG_CH3_DATA_LOW			0x0E
#define AD7291_REG_CH3_HYST			0x0F
#define AD7291_REG_CH4_DATA_HIGH		0x10
#define AD7291_REG_CH4_DATA_LOW			0x11
#define AD7291_REG_CH4_HYST			0x12
#define AD7291_REG_CH5_DATA_HIGH		0x13
#define AD7291_REG_CH5_DATA_LOW			0x14
#define AD7291_REG_CH5_HYST			0x15
#define AD7291_REG_CH6_DATA_HIGH		0x16
#define AD7291_REG_CH6_DATA_LOW			0x17
#define AD7291_REG_CH6_HYST			0x18
#define AD7291_REG_CH7_DATA_HIGH		0x19
#define AD7291_REG_CH7_DATA_LOW			0x1A
#define AD7291_REG_CH7_HYST			0x2B
#define AD7291_REG_T_SENSE_HIGH			0x1C
#define AD7291_REG_T_SENSE_LOW			0x1D
#define AD7291_REG_T_SENSE_HYST			0x1E
#define AD7291_REG_VOLTAGE_ALERT_STATUS		0x1F
#define AD7291_REG_T_ALERT_STATUS		0x20

#define AD7291_COMMAND_CH(x)		(1 << (15 - (x)))
#define AD7291_COMMAND_TSENSE		(1 << 7)
#define AD7291_COMMAND_DELAY		(1 << 5)
#define AD7291_COMMAND_EXT_REF		(1 << 4)
#define AD7291_COMMAND_ALERT_POL_LOW	(1 << 3)
#define AD7291_COMMAND_ALERT_POL_HIGH	(0 << 3)
#define AD7291_COMMAND_CLR_ALERT	(1 << 2)
#define AD7291_COMMAND_RESET		(1 << 1)
#define AD7291_COMMAND_AUTOCYCLE	(1 << 0)

#define AD7291_VOLTAGE_CH_MASK		NO_OS_GENMASK(15, 12)
#define AD7291_VOLTAGE_DATA_MASK	NO_OS_GENMASK(11, 0)
#define AD7291_TSENSE_DATA_MASK		NO_OS_GENMASK(11, 0)
#define AD7291_TSENSE_SIGN_BIT		11

struct ad7291_init_param {
	struct no_os_i2c_init_param i2c_init;
	uint16_t vref_mv;
};

struct ad7291_desc {
	struct no_os_i2c_desc *i2c_desc;
	uint16_t vref_mv;
};

int ad7291_init(struct ad7291_desc **desc,
		struct ad7291_init_param *init_param);

int ad7291_remove(struct ad7291_desc *desc);

int ad7291_reg_read(struct ad7291_desc *desc, uint8_t addr, uint16_t *val);

int ad7291_reg_write(struct ad7291_desc *desc, uint8_t addr, uint16_t val);

int ad7291_read_channel_voltage(struct ad7291_desc *desc, uint8_t channel,
				int32_t *millivolts);

int ad7291_read_temp(struct ad7291_desc *desc, int32_t *millidegrees);

#endif /* _AD7291_H_ */
