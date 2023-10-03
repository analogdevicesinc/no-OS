/***************************************************************************//**
 *   @file   iio_max22190.h
 *   @brief  Header file of MAX22190 IIO Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef IIO_MAX22190_H
#define IIO_MAX22190_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "iio_types.h"
#include "max22190.h"

enum max22190_iio_attr {
	MAX22190_IIO_FAULT1,
	MAX22190_IIO_FAULT2,
};

/**
 * @brief MAX22190 IIO descriptor
*/
struct max22190_iio_desc {
	struct max22190_desc *max22190_desc;
	struct iio_device *iio_dev;
};

/**
 * @brief MAX22190 IIO initalization parameter
*/
struct max22190_iio_desc_init_param {
	struct max22190_init_param *max22190_init_param;
};

/** Initializes the MAX22190 IIO descriptor. */
int max22190_iio_init(struct max22190_iio_desc **,
		      struct max22190_iio_desc_init_param *);

/** Free resources allocated by the initalization function. */
int max22190_iio_remove(struct max22190_iio_desc *);

#endif /* IIO_MAX22190_H */
