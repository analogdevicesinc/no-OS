/***************************************************************************//**
 *   @file   test_iio_max17616_internal.h
 *   @brief  Internal declarations for MAX17616 IIO Driver testing
 *   @author Carlos Jones (carlosjr.jones@analog.com)
 *******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef __TEST_IIO_MAX17616_INTERNAL_H__
#define __TEST_IIO_MAX17616_INTERNAL_H__

#include "iio_types.h"

/*******************************************************************************
 *    TEST-SPECIFIC DECLARATIONS FOR STATIC FUNCTIONS
 ******************************************************************************/

/*
 * Note: These external declarations allow testing of static functions
 * They require TEST to be defined during compilation to make the functions non-static
 */

/* External declaration for testing static function max17616_iio_read_attr */
extern int max17616_iio_read_attr(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

/* External declaration for testing static function max17616_iio_write_attr */
extern int max17616_iio_write_attr(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

/* External declaration for testing static function max17616_iio_read_global_attr */
extern int max17616_iio_read_global_attr(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

/* External declaration for testing static function max17616_iio_write_global_attr */
extern int max17616_iio_write_global_attr(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

/* Channel enumeration from iio_max17616.c - matches actual implementation */
enum max17616_iio_channels {
	MAX17616_IIO_VIN_CHAN,
	MAX17616_IIO_VOUT_CHAN,
	MAX17616_IIO_IOUT_CHAN,
	MAX17616_IIO_TEMP_CHAN,
	MAX17616_IIO_POUT_CHAN,
	MAX17616_IIO_STATUS_WORD_CHAN,
	MAX17616_IIO_STATUS_VOUT_CHAN,
	MAX17616_IIO_STATUS_IOUT_CHAN,
	MAX17616_IIO_STATUS_INPUT_CHAN,
	MAX17616_IIO_STATUS_TEMP_CHAN,
	MAX17616_IIO_STATUS_CML_CHAN,
	MAX17616_IIO_STATUS_MFR_SPECIFIC_CHAN,
	/* Output channels for control settings */
	MAX17616_IIO_CLMODE_CHAN,
	MAX17616_IIO_ISTART_RATIO_CHAN,
	MAX17616_IIO_TSTOC_CHAN,
	MAX17616_IIO_ISTLIM_CHAN,
	/* VOUT UV fault limit configuration channels */
	MAX17616_IIO_NOMINAL_VOLTAGE_CHAN,
	MAX17616_IIO_PGOOD_THRESHOLD_CHAN,
};

#endif /* __TEST_IIO_MAX17616_INTERNAL_H__ */
