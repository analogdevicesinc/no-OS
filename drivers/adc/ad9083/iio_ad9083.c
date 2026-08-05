/***************************************************************************//**
 *   @file   iio_ad9083.c
 *   @brief  Implementation of iio_ad9083.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifdef IIO_SUPPORT

#include "ad9083.h"
#include "iio_ad9083.h"

static int ad9083_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	uint8_t val;
	int ret;

	ret = ad9083_reg_get(dev, reg, &val);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

static int ad9083_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	return ad9083_reg_set(dev, reg, (uint8_t)writeval);
}

struct iio_device ad9083_iio_descriptor = {
	.debug_reg_read = ad9083_iio_reg_read,
	.debug_reg_write = ad9083_iio_reg_write,
};

#endif /* IIO_SUPPORT */
