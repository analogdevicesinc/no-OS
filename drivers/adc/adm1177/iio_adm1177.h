/**************************************************************************//**
*   @file   iio_adm1177.h
*   @brief  Header file of iio_adm1177
*   @author Radu Sabau (radu.sabau@analog.com)
*******************************************************************************
* Copyright 2023(c) Analog Devices, Inc.
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

#ifndef IIO_ADM1177_H
#define IIO_ADM1177_H

#include "iio.h"
#include "iio_types.h"

struct adm1177_iio_dev {
	struct adm1177_dev *adm1177_dev;
	struct iio_device *iio_dev;

	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

struct adm1177_iio_init_param {
	struct adm1177_init_param *adm1177_initial;
};

int adm1177_iio_init(struct adm1177_iio_dev **iio_dev,
		     struct adm1177_iio_init_param *init_param);

int adm1177_iio_remove(struct adm1177_iio_dev *desc);

#endif /** IIO_ADM1177_H */
