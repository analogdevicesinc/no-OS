/***************************************************************************//**
 *   @file   iio_adf4368.h
 *   @brief  Implementation of IIO ADF4368 Driver.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
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
#ifndef IIO_ADF4368_H
#define IIO_ADF4368_H

#include "iio.h"

struct adf4368_iio_dev {
	struct adf4368_dev *adf4368_dev;
	struct iio_device *iio_dev;
};

struct adf4368_iio_dev_init_param {
	struct adf4368_init_param *adf4368_dev_init;
};

enum adf4368_iio_ch_attr_id {
	ADF4368_IIO_CH_ATTR_ENABLE,
	ADF4368_IIO_CH_ATTR_FREQ,
	ADF4368_IIO_CH_ATTR_OPWR,
	ADF4368_IIO_CH_ATTR_PHASE,
};

enum adf4368_iio_dev_attr_id {
	ADF4368_IIO_DEV_ATTR_BLEED,
	ADF4368_IIO_DEV_ATTR_CP,
	ADF4368_IIO_DEV_ATTR_CP_AVAIL,
	ADF4368_IIO_DEV_ATTR_DEF_REG,
	ADF4368_IIO_DEV_ATTR_REF_DIV,
	ADF4368_IIO_DEV_ATTR_REF_DB,
	ADF4368_IIO_DEV_ATTR_REF_FREQ,
	ADF4368_IIO_DEV_ATTR_SYNC_SETUP,
	ADF4368_IIO_DEV_ATTR_SW_SYNC,
	ADF4368_IIO_DEV_ATTR_TEMP,
};

int adf4368_iio_init(struct adf4368_iio_dev **iio_dev,
		     struct adf4368_iio_dev_init_param *init_param);

int adf4368_iio_remove(struct adf4368_iio_dev *desc);

#endif /* IIO_ADF4368_H */
