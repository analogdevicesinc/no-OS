/***************************************************************************//**
 *   @file   iio_admt4000.h
 *   @brief  Header file of IIO ADMT4000 driver.
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 *   @author Louijie Compo  (louijie.compo@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef IIO_ADMT4000_H
#define IIO_ADMT4000_H

#include "iio.h"
#include <errno.h>

/** @enum admt4000_iio_chan_type
 *  @brief ADMT4000 IIO channels enumeration
 */
enum admt4000_iio_chan_type {
	ADMT4000_TURN_COUNT,
	ADMT4000_ANGLE_10BIT,
	ADMT4000_ANGLE_12BIT,
	ADMT4000_TEMP,
	ADMT4000_COSINE,
	ADMT4000_SINE,
};

/** @enum admt4000_iio_harmonics_corr_type
 *  @brief ADMT4000 IIO harmonics correction coefficients enumeration
*/
enum admt4000_iio_harmonics_corr_type {
	ADMT4000_H1MAG_CORR = 0,
	ADMT4000_H1PH_CORR = 1,
	ADMT4000_H2MAG_CORR = 2,
	ADMT4000_H2PH_CORR = 3,
	ADMT4000_H3MAG_CORR = 4,
	ADMT4000_H3PH_CORR = 5,
	ADMT4000_H8MAG_CORR = 14,
	ADMT4000_H8PH_CORR = 15,
};

struct admt4000_iio_dev {
	struct admt4000_dev *admt4000_desc;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
	uint16_t data[12];
	struct iio_hw_trig *hw_trig_desc;
};

struct admt4000_iio_dev_init_param {
	struct admt4000_init_param *admt4000_init_param;
};

int admt4000_iio_init(struct admt4000_iio_dev **,
		      struct admt4000_iio_dev_init_param *);
int admt4000_iio_remove(struct admt4000_iio_dev *);

extern struct iio_trigger admt4000_iio_trig_desc;

#endif
