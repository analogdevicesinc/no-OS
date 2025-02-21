/***************************************************************************//**
 *   @file   iio_adt7420.h
 *   @brief  Header file of IIO ADT7420 Driver.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
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
*******************************************************************************/
#ifndef IIO_ADT7420_H
#define IIO_ADT7420_H

#include <stdint.h>
#include <stdlib.h>
#include "iio.h"

/**
 * @struct adt7420_iio_dev
 * @brief iio device descriptor.
 */
struct adt7420_iio_dev {
	struct adt7420_dev *adt7420_dev;
	struct iio_dev *iio_dev;
	uint32_t active_channels;
	uint8_t no_active_channels;
};

/**
 * @struct adt7420_iio_init_param
 * @brief Configuration structure.
 */
struct adt7420_iio_init_param {
	struct adt7420_init_param *adt7420_dev_init;
};

int adt7420_iio_init(struct adt7420_iio_dev **iio_dev,
		     struct adt7420_iio_init_param *init_param);

int adt7420_iio_remove(struct adt7420_iio_dev *desc);

#endif /** IIO_ADT7420_H **/
