/***************************************************************************//**
 *   @file   iio_ltc4284.h
 *   @brief  Header file of LTC4284 IIO Driver
 *   @author Carlos Jones Jr <carlos.jones.jr@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#ifndef __IIO_LTC4284_H__
#define __IIO_LTC4284_H__

#include "iio.h"
#include "ltc4284.h"

/**
 * @struct ltc4284_iio_desc
 * @brief LTC4284 IIO descriptor
 */
struct ltc4284_iio_desc {
	struct ltc4284_dev *ltc4284_dev;
	struct iio_device *iio_dev;
};

/**
 * @struct ltc4284_iio_init_param
 * @brief LTC4284 IIO initialization parameters
 */
struct ltc4284_iio_init_param {
	struct ltc4284_init_param *ltc4284_init;
};

/**
 * @enum ltc4284_iio_channels
 * @brief LTC4284 IIO channel enumeration
 */
enum ltc4284_iio_channels {
	LTC4284_IIO_VIN_CHAN,
	LTC4284_IIO_IIN_CHAN,
	LTC4284_IIO_VOUT_CHAN,
	LTC4284_IIO_POWER_CHAN,
	LTC4284_IIO_ENERGY_CHAN,
	LTC4284_IIO_NUM_CHANNELS
};

/* IIO interface functions */
int ltc4284_iio_init(struct ltc4284_iio_desc **iio_desc,
		     const struct ltc4284_iio_init_param *init_param);
int ltc4284_iio_remove(struct ltc4284_iio_desc *desc);

#endif /* __IIO_LTC4284_H__ */
