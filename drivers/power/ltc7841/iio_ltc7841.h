/***************************************************************************//**
 *   @file   iio_ltc7841.h
 *   @brief  Header file for the LTC7841 IIO Driver
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
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

#ifndef IIO_LTC7841_H
#define IIO_LTC7841_H

#include <stdbool.h>
#include "iio.h"
#include "ltc7841_regs.h"

#define DECIMAL_POINT_SCALE_VOLTAGE              10000

/**
 * @brief Structure holding the LTC7841 IIO device descriptor
*/
struct ltc7841_iio_desc 
{
	struct ltc7841_desc *ltc7841_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LTC7841 IIO initialization parameter.
*/
struct ltc7841_iio_desc_init_param 
{
	struct ltc7841_init_param *ltc7841_init_param;
};

/** Initializes the LTC7841 IIO descriptor. */
int ltc7841_iio_init(struct ltc7841_iio_desc ** iio_desc,
		     struct ltc7841_iio_desc_init_param * init_param);

/** Free resources allocated by the initialization function. */
int ltc7841_iio_remove(struct ltc7841_iio_desc *);

#endif /* IIO_LTC7841_H */
