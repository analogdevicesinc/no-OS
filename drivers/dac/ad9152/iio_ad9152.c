/***************************************************************************//**
 *   @file   iio_ad9152.c
 *   @brief  Implementation of AD9152 IIO Driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "iio_ad9152.h"

/** IIO Descriptor */
struct iio_device const ad9152_iio_descriptor = {
	.debug_reg_read = (int32_t (*)())ad9152_spi_read,
	.debug_reg_write = (int32_t (*)())ad9152_spi_write,
};

