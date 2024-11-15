/***************************************************************************//**
 *   @file   iio_ada4250.c
 *   @brief  Implementation of ada4250 IIO Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "iio_ada4250.h"
#include "ada4250.h"

const struct iio_device ada4250_iio_descriptor = {
	.debug_reg_read = (int32_t (*)())ada4250_read,
	.debug_reg_write = (int32_t (*)())ada4250_write,
};
