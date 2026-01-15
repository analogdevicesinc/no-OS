/***************************************************************************//**
 *   @file   lm75.h
 *   @brief  Header file of LM75 Driver.
 *   @author Niel Acuna (niel.acuna@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#ifndef __LM75_H__
#define __LM75_H__
#include "no_os_util.h"
#include "no_os_i2c.h"

#define LM75_DEVICE_SIGNATURE 	0x7f4c4d75
#define LM75_HW_ADDR 		0b1001000
#define LM75_FRAME_SIZE 	3

#define LM75_ADDR_MASK 		0xFFFF
#define LM75_SENSOR_DATA_MASK 	0xFF80

#define LM75_MIN_TEMP 		-55
#define LM75_MAX_TEMP 		125

#define LM75_MIN_TEMP_MILLICEL	(LM75_MIN_TEMP * 1000)
#define LM75_MAX_TEMP_MILLICEL 	(LM75_MAX_TEMP * 1000)

enum lm75_register {
	lm75_temperature_reg = 0x00,
	lm75_configuration_reg = 0x01,
	lm75_hyst_reg = 0x02,
	lm75_os_reg = 0x03,
};

enum lm75_os_polarity {
	lm75_os_active_low = 0,
	lm75_os_active_high,
};

enum lm75_temperature_type {
	lm75_die_temperature = 0,
	lm75_hysteresis_temperature,
	lm75_over_temperature,
};

enum lm75_mode {
	lm75_comparator_mode = 0,
	lm75_interrupt_mode,
};

enum lm75_fault_queue {
	lm75_1_fault = 0,
	lm75_2_fault,
	lm75_4_fault,
	lm75_6_fault,
};

struct lm75_init_param {
	uint8_t fault_count;
	/* os_polarity here is for comparator mode which is
	 * what the driver will initialize the LM75 by default. */
	enum lm75_os_polarity os_polarity;
	struct no_os_i2c_init_param *i2c_ip;
};

struct lm75_dev {
	uint32_t signature;
	struct no_os_i2c_desc *i2c;
};

#define LM75_CONFIG_SHUTDOWN 	NO_OS_BIT(0)
#define LM75_CONFIG_INT_EN 	NO_OS_BIT(1)
#define LM75_CONFIG_OS_POLARITY NO_OS_BIT(2)
#define LM75_CONFIG_FAULT_QUEUE NO_OS_GENMASK(4, 3)

#define LM75_HYST_POR_VALUE 	0x4b00
#define LM75_OS_POR_VALUE 	0x5000

#define LM75_TEMP_MASK 	NO_OS_GENMASK(15, 7)

static inline bool lm75_is_lm75(struct lm75_dev *dev)
{
	if (!dev)
		return false;
	return dev->signature == LM75_DEVICE_SIGNATURE;
}

int lm75_init(struct lm75_dev **devpp,
	      struct lm75_init_param *ip);
int lm75_remove(struct lm75_dev *dev);

int lm75_read_temperature(struct lm75_dev *dev,
			  enum lm75_temperature_type whence,
			  uint16_t *val);
int lm75_write_temperature(struct lm75_dev *dev,
			   enum lm75_temperature_type whence,
			   uint16_t val);

int lm75_raw_to_millicelsius(const uint16_t raw_temp);
uint16_t lm75_millicelsius_to_raw(const int millicelsius);
int lm75_is_shutdown(struct lm75_dev *dev, bool *flag);
int lm75_shutdown(struct lm75_dev *dev, bool flag);

int lm75_read_mode(struct lm75_dev *dev, enum lm75_mode *mode);
int lm75_write_mode(struct lm75_dev *dev, enum lm75_mode mode);

int lm75_read_os_polarity(struct lm75_dev *dev, enum lm75_os_polarity *pol);
int lm75_write_os_polarity(struct lm75_dev *dev, enum lm75_os_polarity pol);

int lm75_read_fault_queue(struct lm75_dev *dev, enum lm75_fault_queue *fq);
int lm75_write_fault_queue(struct lm75_dev *dev, enum lm75_fault_queue fq);

#endif /* __LM75_H__ */
