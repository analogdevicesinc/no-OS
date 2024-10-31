/***************************************************************************//**
 *   @file   iio_ltc4162l.h
 *   @brief  Implementation of IIO LTC4162L Driver.
 *   @author Marc Paolo Sosa (marcpaolo.sosa@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#ifndef __IIO_LTC4162L_H__
#define __IIO_LTC4162L_H__

#include "iio.h"

/**
 * @brief Structure holding the LTC4162L IIO device descriptor
*/
struct ltc4162l_iio_device {
	struct ltc4162l_desc *dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_active_channels;
};

/**
 * @brief Structure holding the LTC4162L IIO initialization parameter.
*/
struct ltc4162l_iio_init_param {
	struct ltc4162l_init_param *init_param;
};

/** Initializes the LTC4162L IIO descriptor. */
int ltc4162l_iio_init(struct ltc4162l_iio_device **iio_device,
		      struct ltc4162l_iio_init_param *iio_init_param);

/* Free resources allocated by the initialization function. */
int ltc4162l_iio_remove(struct ltc4162l_iio_device *iio_device);

#endif	/* __IIO_LTC4162L_H__ */
