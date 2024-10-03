/***************************************************************************//**
 *   @file   iio_max14916.h
 *   @brief  Header file of IIO MAX14916 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
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
*******************************************************************************/
#ifndef IIO_MAX14916_H
#define IIO_MAX14916_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "iio_types.h"
#include "max14916.h"

/**
 * @brief MAX14916 specific IIO descriptor.
*/
struct max14916_iio_desc {
	struct max149x6_desc *max14916_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief Initialization parameter for the MAX14916 IIO descriptor.
*/
struct max14916_iio_desc_init_param {
	struct max149x6_init_param *max14916_init_param;
	bool ch_enable[MAX14916_CHANNELS];
};

/** Initializes the MAX14916 IIO descriptor */
int max14916_iio_init(struct max14916_iio_desc **,
		      struct max14916_iio_desc_init_param *);

/** Free resources allocated by the init function. */
int max14916_iio_remove(struct max14916_iio_desc *);

#endif /* IIO_MAX14916_H */
