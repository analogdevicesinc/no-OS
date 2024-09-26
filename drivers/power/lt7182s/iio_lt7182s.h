/***************************************************************************//**
 *   @file   iio_lt7182s.h
 *   @brief  Header file for the LT7182S IIO Driver
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
#ifndef IIO_LT7182S_H
#define IIO_LT7182S_H

#include <stdbool.h>
#include "iio.h"
#include "lt7182s.h"

/**
 * @brief Structure holding the LT7182S IIO device descriptor
*/
struct lt7182s_iio_desc {
	struct lt7182s_dev *lt7182s_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LT7182S IIO initialization parameter.
*/
struct lt7182s_iio_desc_init_param {
	struct lt7182s_init_param *lt7182s_init_param;
};

/** Initializes the LT7182S IIO descriptor. */
int lt7182s_iio_init(struct lt7182s_iio_desc **,
		     struct lt7182s_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int lt7182s_iio_remove(struct lt7182s_iio_desc *);

#endif /* IIO_LT7182S_H */
