/***************************************************************************//**
 *   @file   AD799x.h
 *   @brief  Header file of AD799x Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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

#ifndef __AD799X_H__
#define __AD799X_H__

#include <stdint.h>
#include "no_os_i2c.h"

/* Configuration Register definition. */
#define AD799X_CHANNEL(ch)        ((1 << ch) << 4)
#define AD799X_REF_SEL	          (1 << 3)
#define AD799X_FLTR		  (1 << 2)
#define AD799X_BIT_TRIAL_DELAY	  (1 << 1)
#define AD799X_SAMPLE_DELAY	  (1 << 0)

/* Supported devices */
enum ad799x_type {
	AD7991,
	AD7995,
	AD7999
};

struct ad799x_dev {
	/* I2C */
	struct no_os_i2c_desc	*i2c_desc;
	/* Device Settings */
	uint8_t         bits_number;
};

struct ad799x_init_param {
	/* I2C */
	struct no_os_i2c_init_param		i2c_init;
	/* Device Settings */
	enum ad799x_type	part_number;
};

/*! Initializes I2C. */
int8_t ad799x_init(struct ad799x_dev **device,
		   struct ad799x_init_param init_param);

/*! Free the resources allocated by ad799x_init(). */
int32_t ad799x_remove(struct ad799x_dev *dev);

/*! Writes data into the Configuration Register. */
void ad799x_set_configuration_reg(struct ad799x_dev *dev,
				  uint8_t register_value);

/*! Reads the High byte and the Low byte of the conversion. */
void ad799x_get_conversion_result(struct ad799x_dev *dev,
				  int16_t* conv_value,
				  int8_t* channel);

/*! Converts a raw sample to volts.*/
float ad799x_convert_to_volts(struct ad799x_dev *dev,
			      int16_t raw_sample,
			      float v_ref);

#endif /* __AD799X_H__ */
