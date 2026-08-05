/*******************************************************************************
 *   @file   iio_ad405x.h
 *   @brief  Header file of IIO AD405X Driver.
 *   @author George Mois (george.mois@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_AD405X_H
#define IIO_AD405X_H

#include "iio.h"

/**
 * @struct ad405x_iio_debug_attrs
 * @brief ADIS IIO debug attributes enumeration
 */
enum ad405x_iio_debug_attrs {
	AD405X_DEBUG_SAMPLE_RATE,
	AD405X_DEBUG_AVG_FILTER_LEN
};

/**
 * @struct ad405x_iio_dev
 * @brief AD405X IIO device structure
 */
struct ad405x_iio_dev {
	/** AD405X driver handler */
	struct ad405x_dev *ad405x_dev;
	/** Generic IIO device handler */
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
};

/**
 * @struct ad405x_iio_init_param
 * @brief AD405X IIO initialization structure
 */
struct ad405x_iio_dev_init_param {
	struct ad405x_init_param *ad405x_dev_init;
};

/*! Callback for iio debug attributes reading. */
int ad405x_iio_read_debug_attrs(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
/*! Callback for iio debug attributes writing. */
int ad405x_iio_write_debug_attrs(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the write request for sampling frequency attribute. */
int ad405x_iio_write_sample_rate(void *dev, char *buf,
				 uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the read request for sampling frequency attribute. */
int ad405x_iio_read_sample_rate(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the write request for sampling frequency attribute. */
int ad405x_iio_write_avg_filter_length(void *dev, char *buf,
				       uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the read request for sampling frequency attribute. */
int ad405x_iio_read_avg_filter_length(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
/** Allocate memory for AD405X IIO handler. */
int ad405x_iio_init(struct ad405x_iio_dev **iio_dev,
		    struct ad405x_iio_dev_init_param *init_param);
/** Free memory allocated by ad405x_iio_init(). */
int ad405x_iio_remove(struct ad405x_iio_dev *desc);

#endif /** IIO_AD405X_H */
