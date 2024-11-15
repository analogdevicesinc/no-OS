/***************************************************************************//**
*   @file   iio_hmc630x.h
*   @brief  Header file of hmc6300 and hmc6301 driver extension for IIOD.
*   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
* Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_HMC630X_H
#define IIO_HMC630X_H

#include "iio.h"
#include "hmc630x.h"

struct hmc630x_iio_dev {
	struct hmc630x_dev *dev;
	struct iio_device *iio_dev;
};

struct hmc630x_iio_init_param {
	struct hmc630x_init_param *ip;
};

enum hmc630x_iio_attr_id {
	HMC630X_IIO_ATTR_ENABLED,
	HMC630X_IIO_ATTR_TEMP_EN,
	HMC630X_IIO_ATTR_VCO,
	HMC630X_IIO_ATTR_VCO_AVAILABLE,
	HMC630X_IIO_ATTR_VCO_BAND,
	HMC630X_IIO_ATTR_VCO_LOCK,
	HMC630X_IIO_ATTR_IF_ATTN,
	HMC6300_IIO_ATTR_RF_ATTN,
	HMC6301_IIO_ATTR_RF_LNA_GAIN,
	HMC6301_IIO_ATTR_BB_ATTN1,
	HMC6301_IIO_ATTR_BB_ATTN2,
	HMC6301_IIO_ATTR_BB_ATTNI_FINE,
	HMC6301_IIO_ATTR_BB_ATTNQ_FINE,
	HMC6301_IIO_ATTR_BB_LPC,
	HMC6301_IIO_ATTR_BB_HPC,
};

int32_t hmc630x_iio_init(struct hmc630x_iio_dev **iio_dev,
			 struct hmc630x_iio_init_param *init_param);
int32_t hmc630x_iio_remove(struct hmc630x_iio_dev *desc);

#endif /** IIO_HMC630X_H */
