/***************************************************************************//**
 *   @file   iio_adf5611.h
 *   @brief  Implementation of IIO ADF5611 Driver.
 *   @author josemene (jude.osemene@analog.com)
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
#ifndef IIO_ADF5611_H
#define IIO_ADF5611_H

#include "iio_types.h"
#include "iio.h"

struct adf5611_iio_dev {
	struct adf5611_dev *adf5611_dev;
	struct iio_device *iio_dev;
};

struct adf5611_iio_dev_init_param {
	struct adf5611_init_param *adf5611_dev_init;
};

enum adf5611_iio_ch_attr_id {
	ADF5611_IIO_CH_ATTR_FREQ,
	ADF5611_IIO_CH_ATTR_OPWR,
};

enum adf5611_iio_dev_attr_id {
	ADF5611_IIO_DEV_ATTR_REF_CLK,
	ADF5611_IIO_DEV_ATTR_REF_DIV,
	ADF5611_IIO_DEV_ATTR_CP_I,
	ADF5611_IIO_DEV_ATTR_CP_AVAIL,
	ADF5611_IIO_DEV_ATTR_RFOUTDIV_PWR,
	ADF5611_IIO_DEV_ATTR_RFOUTDIV_DIV,
	ADF5611_IIO_DEV_ATTR_EN_RFOUTDIV,
	ADF5611_IIO_DEV_ATTR_RFOUTDIV_DIV_AVAIL,
};

int adf5611_iio_init(struct adf5611_iio_dev **iio_dev,
		     struct adf5611_iio_dev_init_param *init_param);

int adf5611_iio_remove(struct adf5611_iio_dev *desc);

#endif
