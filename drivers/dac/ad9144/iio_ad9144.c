/***************************************************************************//**
 *   @file   iio_ad9144.c
 *
 *   @brief  Implementation of AD9144 IIO Driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "iio_ad9144.h"

const struct iio_device ad9144_iio_descriptor = {
	.debug_reg_read = (int32_t (*)())ad9144_spi_read,
	.debug_reg_write = (int32_t (*)())ad9144_spi_write,
};

