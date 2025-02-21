/***************************************************************************//**
 *   @file   max538x.h
 *   @brief  Header file of max538x Family Driver.
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef __MAX538x_H__
#define __MAX538x_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "no_os_util.h"

/* MAX5380 default resolution */
#define MAX538X_RESOLUTION 		0xFF

enum max538x_type {
	MAX5380L,
	MAX5380M,
	MAX5380N,
	MAX5380K,
	MAX5381L,
	MAX5381M,
	MAX5381N,
	MAX5381K,
	MAX5382L,
	MAX5382M,
	MAX5382N,
	MAX5382K,
};

struct max538x_chip_info {
	uint8_t vfactor;
	uint8_t addr;
};

struct max538x_dev {
	/* I2C */
	struct no_os_i2c_desc	*i2c_desc;
	/* Device Settings*/
	enum max538x_type active_device;
	/* Device VDD (for MAX5382 variant) */
	uint8_t max538x_vdd ;
	/* Device VREF */
	uint8_t max538x_vref ;
};

struct max538x_init_param {
	/* I2C */
	struct no_os_i2c_init_param	i2c_init;
	/* Device Settings*/
	enum max538x_type active_device;
	/* Device VDD (for MAX5382 variant) */
	uint8_t max538x_vdd ;
	/* Device VREF */
	uint8_t max538x_vref ;
};

extern const struct max538x_chip_info chip_info[];

/* Initializes the communications peripheral and checks if the device is present. */
int max538x_init(struct max538x_dev **device,
		 struct max538x_init_param init_param);

/* Free the resources allocated by max538x_init(). */
int max538x_remove(struct max538x_dev *dev);

/* Sets the DAC output using VOUT */
int max538x_set_voutput(struct max538x_dev *dev, float vout);

#endif	/* __MAX538x_H__ */
