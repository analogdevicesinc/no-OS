/***************************************************************************//**
 *   @file   iio_max11205.h
 *   @brief  Header file of iio max11205 driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
 ******************************************************************************/
#ifndef __IIO_MAX11205_H__
#define __IIO_MAX11205_H__

#include "iio.h"

enum max11205_dev_id {
	MAX11205A,
	MAX11205B,
};

struct max11205_iio_dev {
	/** MAX11205 device descriptor */
	struct max11205_dev 	*max11205_dev;
	/** IIO device descriptor */
	struct iio_device 	*iio_dev;
	/** MAX11205 device sampling frequency */
	int32_t 		sampling_frequency;
};

struct max11205_iio_dev_init_param {
	/** MAX11205 device initialization data */
	struct max11205_init_param *max11205_dev_init;
	/** MAX11205 device id */
	enum max11205_dev_id dev_id;
};

int max11205_iio_init(struct max11205_iio_dev **iio_dev,
		      struct max11205_iio_dev_init_param *init_param);
int max11205_iio_remove(struct max11205_iio_dev *desc);

#endif /* __IIO_MAX11205_H__ */
