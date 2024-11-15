/**************************************************************************//**
*   @file   ad3552r.h
*   @brief  IIO Header file of ad3552r Driver
*   @author Mihail Chindris (Mihail.Chindris@analog.com)
*
*******************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_AD3552R_H
#define IIO_AD3552R_H

#include "ad3552r.h"
#include "iio_types.h"

struct iio_ad3552r_desc;

int32_t iio_ad3552r_init(struct iio_ad3552r_desc **iio_dac,
			 struct ad3552r_init_param *param);

int32_t iio_ad3552r_remove(struct iio_ad3552r_desc *iio_dac);

void iio_ad3552r_get_descriptor(struct iio_ad3552r_desc *iio_dac,
				struct iio_device **desc);

#endif /* IIO_AD3552R_H */
