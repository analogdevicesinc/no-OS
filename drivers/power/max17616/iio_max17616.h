/***************************************************************************//**
*   @file   iio_max17616.h
*   @brief  Header file of the MAX17616 IIO Driver
*   @author Carlos Jones (carlosjr.jones@analog.com)
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
#ifndef __IIO_MAX17616_H__
#define __IIO_MAX17616_H__

#include <stdbool.h>
#include "iio.h"
#include "max17616.h"

/**
 * @brief Structure holding the MAX17616 IIO device descriptor
*/
struct max17616_iio_desc {
	struct max17616_dev *max17616_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the MAX17616 IIO initialization parameter.
*/
struct max17616_iio_desc_init_param {
	struct max17616_init_param *max17616_init_param;
};

/** Initializes the MAX17616 IIO descriptor. */
int max17616_iio_init(struct max17616_iio_desc **iio_desc,
		      struct max17616_iio_desc_init_param *init_param);

/** Free resources allocated by the initialization function. */
int max17616_iio_remove(struct max17616_iio_desc *desc);

#endif /* __IIO_MAX17616_H__ */
