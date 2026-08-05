/**************************************************************************//**
*   @file   iio_ad7490.h
*   @brief  Header file of iio_ad7490
*   @author Radu Sabau (radu.sabau@analog.com)
*******************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#ifndef IIO_AD7490_H
#define IIO_AD7490_H

#include "iio.h"
#include "iio_types.h"
#include "ad7490.h"

struct ad7490_iio_desc {
	struct ad7490_desc *ad7490_desc;
	struct iio_device *iio_dev;

	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

struct ad7490_iio_init_param {
	struct ad7490_init_param *ad7490_init;

	enum ad7490_dout_state dout_state;
	enum ad7490_range range;
	enum ad7490_coding coding;
};

int ad7490_iio_init(struct ad7490_iio_desc **iio_desc,
		    struct ad7490_iio_init_param *init_param);

int ad7490_iio_remove(struct ad7490_iio_desc *iio_desc);

#endif /** IIO_AD7490_H */
