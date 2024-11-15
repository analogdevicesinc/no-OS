/***************************************************************************//**
 *   @file   iio_ad9083.c
 *   @brief  Implementation of iio_ad9083.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef IIO_SUPPORT

#include "ad9083.h"
#include "iio_ad9083.h"

struct iio_device ad9083_iio_descriptor = {
	.debug_reg_read = (int32_t (*)())ad9083_reg_get,
	.debug_reg_write = (int32_t (*)())ad9083_reg_set,
};

#endif /* IIO_SUPPORT */