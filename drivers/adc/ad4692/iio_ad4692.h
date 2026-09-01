/**************************************************************************//**
 *   @file   iio_ad4692.h
 *   @brief  Header file of ad4692 IIO driver
 *   @author Radu Sabau (radu.sabau@analog.com)
*******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_AD4692_H
#define IIO_AD4692_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "iio_types.h"
#include "no_os_irq.h"

#include "ad4692.h"

#define AD4692_CONV_START		0x01
#define AD4692_CONV_STOP		0x00

#define AD4692_ACC_MASK1_MASK		NO_OS_GENMASK(7, 0)
#define AD4692_ACC_MASK2_MASK		NO_OS_GENMASK(15, 8)

struct ad4692_iio_desc {
	struct ad4692_desc *ad4692_desc;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

struct ad4692_iio_desc_init_param {
	struct ad4692_init_param *ad4692_init_param;
};

int ad4692_iio_init(struct ad4692_iio_desc **,
		    struct ad4692_iio_desc_init_param *);

int ad4692_iio_remove(struct ad4692_iio_desc *);

#endif /* IIO_AD4692_H */
