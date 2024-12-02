/***************************************************************************//**
 *   @file   iio_ltm4686.h
 *   @brief  Header file for the LTM4686 IIO Driver
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
#ifndef IIO_LTM4686_H
#define IIO_LTM4686_H

#include <stdbool.h>
#include "iio.h"
#include "ltm4686.h"

/**
 * @brief Structure holding the LTM4686 IIO device descriptor
*/
struct ltm4686_iio_desc {
	struct ltm4686_dev *ltm4686_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LTM4686 IIO initialization parameter.
*/
struct ltm4686_iio_desc_init_param {
	struct ltm4686_init_param *ltm4686_init_param;
};

/** Initializes the LTM4686 IIO descriptor. */
int ltm4686_iio_init(struct ltm4686_iio_desc **,
		     struct ltm4686_iio_desc_init_param *);

/** Free resources allocated by the initialization function. */
int ltm4686_iio_remove(struct ltm4686_iio_desc *);

#endif /* IIO_LTM4686_H */
