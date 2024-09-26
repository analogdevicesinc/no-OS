/***************************************************************************//**
 *   @file   iio_lt8722.h
 *   @brief  Header file for the LT8722 IIO Driver
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
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
#ifndef __IIO_LT8722_H__
#define __IIO_LT8722_H__

#include <stdbool.h>
#include "iio.h"
#include "lt8722.h"

/**
 * @struct lt8722_iio_dev
 * @brief Structure holding the LT8722 IIO device descriptor
 */
struct lt8722_iio_dev {
	struct lt8722_dev *lt8722_dev;
	struct iio_device *iio_dev;
};

/**
 * @struct lt8722_iio_dev_init_param
 * @brief Structure holding the LT8722 IIO initalization parameter.
 */
struct lt8722_iio_dev_init_param {
	struct lt8722_init_param *lt8722_init_param;
};

/** Initializes the LT8722 IIO descriptor. */
int lt8722_iio_init(struct lt8722_iio_dev **,
		    struct lt8722_iio_dev_init_param *);

/** Free resources allocated by the initialization function. */
int lt8722_iio_remove(struct lt8722_iio_dev *);

#endif /* __IIO_LT8722_H__ */
