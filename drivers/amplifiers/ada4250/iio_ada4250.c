/***************************************************************************//**
 *   @file   iio_ada4250.c
 *   @brief  Implementation of ada4250 IIO Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
*******************************************************************************/

#include "iio_ada4250.h"
#include "ada4250.h"

static int ada4250_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	uint8_t val;
	int ret;

	ret = ada4250_read(dev, (uint8_t)reg, &val);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

static int ada4250_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	return ada4250_write(dev, (uint8_t)reg, (uint8_t)writeval);
}

const struct iio_device ada4250_iio_descriptor = {
	.debug_reg_read = ada4250_iio_reg_read,
	.debug_reg_write = ada4250_iio_reg_write,
};
