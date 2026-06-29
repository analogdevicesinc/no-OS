/***************************************************************************//**
 *   @file   iio_adxl37x.h
 *   @brief  Header file of IIO ADXL37x Driver.
 *   @author Raul Georgian (Raul.Georgian@analog.com)
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
#ifndef IIO_ADXL37X_H
#define IIO_ADXL37X_H

#include "iio.h"
#include "adxl37x.h"

#define ADXL37X_FIFO_MAX_SAMPLES   170
#define ADXL37X_MAX_BUFFER_SAMPLES 400


struct adxl37x_iio_dev {
	struct adxl37x_dev *adxl37x_dev;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

struct adxl37x_iio_dev_init_param {
	struct adxl37x_init_param *adxl37x_dev_init;
};

int adxl37x_iio_init(struct adxl37x_iio_dev **iio_dev,
		     struct adxl37x_iio_dev_init_param *init_param);

int adxl37x_iio_remove(struct adxl37x_iio_dev *desc);

#endif /* IIO_ADXL37X_H */
