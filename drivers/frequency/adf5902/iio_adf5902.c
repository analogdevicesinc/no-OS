/***************************************************************************//**
 *   @file   iio_adf5902.c
 *   @brief  Implementation of ADF5902 IIO Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
*******************************************************************************/

#include "iio_adf5902.h"

static int adf5902_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	int test = get_unex_func();
	return adf5902_readback(dev, (uint8_t)reg, readval);
}

static int adf5902_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	return adf5902_write(dev, (uint8_t)reg, writeval);
}

struct iio_device const adf5902_iio_descriptor = {
	.debug_reg_read = adf5902_iio_reg_read,
	.debug_reg_write = adf5902_iio_reg_write,
};
