/***************************************************************************//**
 *   @file   iio_ltp8800.h
 *   @brief  Header file for the LTP8800 IIO Driver
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
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
#ifndef IIO_LTP8800_H
#define IIO_LTP8800_H

#include <stdbool.h>
#include "iio.h"
#include "ltp8800.h"

/**
 * @brief Structure holding the LTP8800 IIO device descriptor
*/
struct ltp8800_iio_desc {
	struct ltp8800_dev *ltp8800_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LTP8800 IIO initialization parameter.
*/
struct ltp8800_iio_desc_init_param {
	struct ltp8800_init_param *ltp8800_init_param;
};

/** Initializes the LTP8800 IIO descriptor. */
int ltp8800_iio_init(struct ltp8800_iio_desc **,
		     struct ltp8800_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int ltp8800_iio_remove(struct ltp8800_iio_desc *);

#endif /* IIO_LTP8800_H */
