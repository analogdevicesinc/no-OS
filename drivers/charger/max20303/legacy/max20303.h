/***************************************************************************//**
 *   @file   max20303.h
 *   @brief  Header file for the MAX20303 battery charger driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#ifndef MAX20303_H_
#define MAX20303_H_

#include <stdint.h>
#include "no_os_i2c.h"

#define MAX20303_I2C_ADDR	0x28
#define MAX20303_FG_I2C_ADDR	0x36

#define MAX20303_VCELL_REG	0x02
#define MAX20303_SOC_REG	0x04
#define MAX20303_MODE_REG	0x06
#define MAX20303_VERSION_REG	0x08
#define MAX20303_HIBRT_REG	0x0A
#define MAX20303_CONFIG_REG	0x0C
#define MAX20303_VALRT_REG	0x14
#define MAX20303_CRATE_REG	0x16
#define MAX20303_VRESET_REG	0x18
#define MAX20303_STATUS_REG	0x1A
#define MAX20303_TABLE_REG(x)	(0x40 + (x))
#define MAX20303_CMD_REG	0xFE

#define MAX20303_VCELL_LSB_nV	78125

#define MAX20303_HIBERNATE_EN	0xFFFF
#define MAX20303_HIBERNATE_DIS	0x0

struct max20303_init_param {
	struct no_os_i2c_init_param *comm_param;
};

struct max20303_desc {
	struct no_os_i2c_desc *comm_desc;
	struct no_os_i2c_desc *fg_comm_desc;
};

int max20303_init(struct max20303_desc **, struct max20303_init_param *);
int max20303_remove(struct max20303_desc *);
int max20303_reg_read(struct max20303_desc *, uint8_t, uint16_t *);
int max20303_reg_write(struct max20303_desc *, uint8_t, uint16_t);

#endif