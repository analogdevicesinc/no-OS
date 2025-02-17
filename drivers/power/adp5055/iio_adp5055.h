/***************************************************************************//**
 *   @file   iio_adp5055.h
 *   @brief  Header file for the ADP5055 IIO Driver
 *   @author Angelo Catapang (angelo.catapang@analog.com)
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
#ifndef IIO_ADP5055_H
#define IIO_ADP5055_H

#include <stdbool.h>
#include "iio.h"
#include "adp5055.h"

/**
 * @brief Structure holding the ADP5055 IIO device descriptor
*/
struct adp5055_iio_desc {
	struct adp5055_desc *adp5055_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the ADP5055 IIO initalization parameter.
*/
struct adp5055_iio_desc_init_param {
	struct adp5055_init_param *adp5055_init_param;
};

/** Initializes the ADP5055 IIO descriptor. */
int adp5055_iio_init(struct adp5055_iio_desc **,
		     struct adp5055_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int adp5055_iio_remove(struct adp5055_iio_desc *);

#endif /* IIO_ADP5055_H */
