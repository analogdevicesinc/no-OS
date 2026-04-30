/***************************************************************************//**
 *   @file   iio_ltm4700.h
 *   @brief  Header file for the LTM4700 IIO Driver
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#ifndef __IIO_LTM4700_H__
#define __IIO_LTM4700_H__

#include <stdbool.h>
#include "iio.h"
#include "ltm4700.h"

/**
 * @brief Structure holding the LTM4700 IIO device descriptor
 */
struct ltm4700_iio_desc {
	struct ltm4700_dev *ltm4700_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Structure holding the LTM4700 IIO initialization parameter.
 */
struct ltm4700_iio_desc_init_param {
	struct ltm4700_init_param *ltm4700_init_param;
};

/* LTM4700 IIO attributes */
enum ltm4700_iio_attr_id {
	LTM4700_IIO_VIN,
	LTM4700_IIO_IIN,
	LTM4700_IIO_VOUT,
	LTM4700_IIO_IOUT,
	LTM4700_IIO_TEMP_EXT,
	LTM4700_IIO_TEMP_IC,
	LTM4700_IIO_FREQ,
	LTM4700_IIO_POUT,
	LTM4700_IIO_PIN,
	LTM4700_IIO_VOUT_COMMAND,
	LTM4700_IIO_VOUT_MAX,
	LTM4700_IIO_VOUT_MARGIN_HIGH,
	LTM4700_IIO_VOUT_MARGIN_LOW,
	LTM4700_IIO_OPERATION,
	LTM4700_IIO_CLEAR_PEAKS,
	LTM4700_IIO_DEVICE_ID,
};

/**
 * @brief Initialize the LTM4700 IIO driver
 * @param iio_desc - Pointer to IIO descriptor pointer
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_iio_init(struct ltm4700_iio_desc **iio_desc,
		     struct ltm4700_iio_desc_init_param *init_param);

/**
 * @brief Free resources allocated by the init function
 * @param desc - IIO descriptor to free
 * @return 0 in case of success, negative error code otherwise
 */
int ltm4700_iio_remove(struct ltm4700_iio_desc *desc);

#endif /* __IIO_LTM4700_H__ */
