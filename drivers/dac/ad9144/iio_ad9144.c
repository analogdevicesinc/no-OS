/***************************************************************************//**
 *   @file   iio_ad9144.c
 *
 *   @brief  Implementation of AD9144 IIO Driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
*******************************************************************************/

#include "iio_ad9144.h"

static int ad9144_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	uint8_t val;
	int ret;

	ret = ad9144_spi_read(dev, (uint16_t)reg, &val);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

static int ad9144_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	return ad9144_spi_write(dev, (uint16_t)reg, (uint8_t)writeval);
}

const struct iio_device ad9144_iio_descriptor = {
	.debug_reg_read = ad9144_iio_reg_read,
	.debug_reg_write = ad9144_iio_reg_write,
};

