/***************************************************************************//**
 *   @file   iio_max14906.h
 *   @brief  Header file of MAX14906 IIO Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_MAX14906_H
#define IIO_MAX14906_H

#include <stdint.h>
#include <stdbool.h>
#include "iio.h"
#include "max14906.h"

#define MAX14906_FUNCTION_CNT	3

/**
 * @brief Configuration structure for a MAX14906 channel.
 */
struct max14906_ch_config {
	bool enabled;
	enum max14906_function function;
};

/**
 * @brief Available functions for each channel of the device.
 */
static const char *const max14906_function_avail[MAX14906_FUNCTION_CNT] = {
	[MAX14906_OUT] = "output",
	[MAX14906_IN] = "input",
	[MAX14906_HIGH_Z] = "high_z"
};

/**
 * @brief Available values for the do_mode attribute.
 */
static const char *const max14906_do_mode_avail[4] = {
	[MAX14906_HIGH_SIDE] = "High_side",
	[MAX14906_HIGH_SIDE_INRUSH] = "High_side_2x_inrush",
	[MAX14906_PUSH_PULL_CLAMP] = "Push_pull_clamp",
	[MAX14906_PUSH_PULL] = "Push_pull",
};

/**
 * @brief Available values for the IEC_type attribute.
 */
static const char *const max14906_iec_avail[2] = {
	"Type_1_3",
	"Type_2"
};

/**
 * @brief MAX14906 specific IIO descriptor.
 */
struct max14906_iio_desc {
	struct max149x6_desc *max14906_desc;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint32_t no_active_channels;
	struct max14906_ch_config channel_configs[MAX14906_CHANNELS];
};

/**
 * @brief Initialization parameter for the MAX14906 IIO descriptor.
 */
struct max14906_iio_desc_init_param {
	struct max149x6_init_param *max14906_init_param;
	struct max14906_ch_config channel_configs[MAX14906_CHANNELS];
};

/** Initializes the MAX14906 IIO descriptor */
int max14906_iio_init(struct max14906_iio_desc **,
		      struct max14906_iio_desc_init_param *);

/** Free resources allocated by the init function */
int max14906_iio_remove(struct max14906_iio_desc *);

#endif /* IIO_MAX14906_H */
