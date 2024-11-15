/***************************************************************************//**
 *   @file   iio_ad9680.c
 *   @brief  Implementation of AD9680 iio.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "iio_ad9680.h"

/** IIO Descriptor */
struct iio_device const ad9680_iio_descriptor = {
	.debug_reg_read = (int32_t (*)())ad9680_spi_read,
	.debug_reg_write = (int32_t (*)())ad9680_spi_write,
};

