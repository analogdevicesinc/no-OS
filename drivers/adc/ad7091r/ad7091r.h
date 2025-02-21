/***************************************************************************//**
 *   @file   AD7091R.h
 *   @brief  Header file of AD7091R Driver.
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

#ifndef __AD7091R_H__
#define __AD7091R_H__

#include <stdint.h>
#include "no_os_spi.h"

struct ad7091r_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
};

struct ad7091r_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

/*! Initializes the SPI communication peripheral. */
int8_t ad7091r_init(struct ad7091r_dev **device,
		    struct ad7091r_init_param init_param);

/*! Free the resources allocated by ad7091r_init(). */
int32_t ad5686_remove(struct ad7091r_dev *dev);

/*! Initiate a software reset of the device. */
void ad7091r_software_reset(struct ad7091r_dev *dev);

/*! Initiates one conversion and reads back the result. */
uint16_t ad7091r_read_sample(struct ad7091r_dev *dev);

/*! Puts the device in power-down mode. */
void ad7091r_power_down(struct ad7091r_dev *dev);

/*! Powers up the device. */
void ad7091r_power_up(struct ad7091r_dev *dev);

/*! Converts a 12-bit raw sample to volts. */
float ad7091r_convert_to_volts(int16_t raw_sample, float v_ref);

#endif /* __AD7091R_H__ */
