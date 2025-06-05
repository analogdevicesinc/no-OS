/***************************************************************************//**
 *   @file   max31889.h
 *   @brief  Temperature sensor functionalities
 *   @author Robert Budai (robert.budai@analog.com)
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

#ifndef MAX31889_H
#define MAX31889_H

#include <errno.h>
#include "no_os_i2c.h"

#define MAX31889_REG_STATUS				0x00
#define MAX31889_REG_INT_ENABLE			0x01
#define MAX31889_REG_FIFO_WRITE_PTR		0x04
#define MAX31889_REG_FIFO_READ_PTR		0x05
#define MAX31889_REG_OVERL_CNT			0x06
#define MAX31889_REG_DATA_CNT 			0x07
#define MAX31889_REG_FIFO_DATA			0x08
#define MAX31889_REG_FIFO_CONFIG 		0x09
#define MAX31889_REG_FIFO_CONFIG2		0x0A
#define MAX31889_REG_SYS_CNTRL 			0x0C
#define MAX31889_REG_ALARM_HIGH_MSB		0x10
#define MAX31889_REG_ALARM_HIGH_LSB		0x11
#define MAX31889_REG_ALARM_LOW_MSB		0x12
#define MAX31889_REG_ALARM_LOW_LSB		0x13
#define MAX31889_REG_TEMP_SETUP			0x14
#define MAX31889_REG_GPIO_SETUP 		0x20
#define MAX31889_REG_GPIO_CNTRL  		0x21
#define MAX31889_REG_PART_ID 			0xFF

#define MAX31889_MAX_CONV_STEPS     	10
#define MAX31889_CONV_RATE          	0.005

#define MAX31889_MEASUREMENT_TRIG_VALUE	0xC1
#define TEMP_MEAS_WAIT_UNTIL_ERROR_MS	1000

struct max31889_desc {
	struct no_os_i2c_desc *i2c_desc;
};

struct max31889_init_param {
	struct no_os_i2c_init_param *i2c_ip;
};

int32_t max31889_trig_conversion(struct max31889_desc*);
int32_t max31889_write_reg(struct max31889_desc*, uint8_t, uint8_t*, uint8_t);
int32_t max31889_read_reg(struct max31889_desc*, uint8_t, uint8_t*, uint8_t);
int32_t max31889_trig_and_read_temp(struct max31889_desc*, double*);
int32_t max31889_init(struct max31889_desc**, struct max31889_init_param*);
int32_t max31889_remove(struct max31889_desc*);

#endif
