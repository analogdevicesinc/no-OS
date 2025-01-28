// /***************************************************************************//**
//  *   @file   iio_ltc7871.h
//  *   @brief  Header file for the LTC7871 IIO Driver
//  *   @author Aldrin Abacan (aldrin.abacan@analog.com)
//  *******************************************************************************
//  * Copyright 2024(c) Analog Devices, Inc.
//  *
//  * Redistribution and use in source and binary forms, with or without
//  * modification, are permitted provided that the following conditions are met:
//  *
//  * 1. Redistributions of source code must retain the above copyright notice,
//  *    this list of conditions and the following disclaimer.
//  *
//  * 2. Redistributions in binary form must reproduce the above copyright notice,
//  *    this list of conditions and the following disclaimer in the documentation
//  *    and/or other materials provided with the distribution.
//  *
//  * 3. Neither the name of Analog Devices, Inc. nor the names of its
//  *    contributors may be used to endorse or promote products derived from this
//  *    software without specific prior written permission.
//  *
//  * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
//  * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
//  * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
//  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
//  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  ******************************************************************************/

#ifndef __IIO_LTC7871_H__
#define __IIO_LTC7871_H__

#include <stdbool.h>
// #include "iio/iio.h"
#include "iio.h"
#include "ltc7871.h"

/**
 * @struct ltc7871_iio_dev
 * @brief Structure holding the LTC7871 IIO device descriptor
 */
struct ltc7871_iio_dev {
	struct ltc7871_dev *ltc7871_dev;
	struct iio_device *iio_dev;
};

/**
 * @struct ltc7871_iio_dev_init_param
 * @brief Structure holding the LTC7871 IIO initalization parameter.
 */
struct ltc7871_iio_dev_init_param {
	struct ltc7871_init_param *ltc7871_init_param;
};

/**
 * @brief Initializes the LTC7871 IIO descriptor.
 * @param iio_dev - The iio device descriptor.
 * @param init_param - The structure that contains the device
 * 		initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int ltc7871_iio_init(struct ltc7871_iio_dev **iio_dev,
		     struct ltc7871_iio_dev_init_param *init_param);

/**
 * @brief Free resources allocated by the init function.
 * @param desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int ltc7871_iio_remove(struct ltc7871_iio_dev *desc);

#endif /* __IIO_LTC7871_H__ */
