/***************************************************************************//**
 *   @file   iio_ad9680.c
 *   @brief  Implementation of AD9680 iio.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "iio_ad9680.h"

/** IIO Descriptor */
static int ad9680_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	uint8_t val;
	int ret;

	ret = ad9680_spi_read(dev, (uint16_t)reg, &val);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

static int ad9680_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	return ad9680_spi_write(dev, (uint16_t)reg, (uint8_t)writeval);
}

struct iio_device const ad9680_iio_descriptor = {
	.debug_reg_read = ad9680_iio_reg_read,
	.debug_reg_write = ad9680_iio_reg_write,
};

