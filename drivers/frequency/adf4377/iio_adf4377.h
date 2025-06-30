/***************************************************************************//**
 *   @file   iio_adf4377.h
 *   @brief  Implementation of IIO ADF4377 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
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
#ifndef IIO_ADF4377_H
#define IIO_ADF4377_H

#include "iio_types.h"
#include "iio.h"

struct adf4377_iio_dev {
	struct adf4377_dev *adf4377_dev;
	struct iio_device *iio_dev;
};

struct adf4377_iio_dev_init_param {
	struct adf4377_init_param *adf4377_dev_init;
};

enum adf4377_iio_ch_attr_id {
	ADF4377_IIO_CH_ATTR_FREQ,
	ADF4377_IIO_CH_ATTR_OPWR,
	ADF4377_IIO_CH_ATTR_EN,
};

enum adf4377_iio_dev_attr_id {
	ADF4377_IIO_DEV_ATTR_REF_CLK,
	ADF4377_IIO_DEV_ATTR_REF_DIV,
	ADF4377_IIO_DEV_ATTR_RFOUT_DIV,
	ADF4377_IIO_DEV_ATTR_RFOUT_DIV_AVAIL,
	ADF4377_IIO_DEV_ATTR_CP_I,
	ADF4377_IIO_DEV_ATTR_CP_AVAIL,
	ADF4377_IIO_DEV_ATTR_BLEED_CURRENT,
	ADF4377_IIO_DEV_ATTR_REF_DOUBLER_EN,
	ADF4377_IIO_DEV_ATTR_NDEL,
	ADF4377_IIO_DEV_ATTR_RDEL,
	ADF4377_IIO_DEV_ATTR_SR_DEL_ADJ,
	ADF4377_IIO_DEV_ATTR_SR_INV_ADJ,
	ADF4377_IIO_DEV_ATTR_SR_MONITORING,
	ADF4377_IIO_DEV_ATTR_DEFAULT_REG,
};

int adf4377_iio_init(struct adf4377_iio_dev **iio_dev,
		     struct adf4377_iio_dev_init_param *init_param);

int adf4377_iio_remove(struct adf4377_iio_dev *desc);

#endif
