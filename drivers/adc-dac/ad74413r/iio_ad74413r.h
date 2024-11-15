/***************************************************************************//**
 *   @file   iio_ad74413r.h
 *   @brief  Header file of the IIO AD74413r Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_AD74413R_H
#define IIO_AD74413R_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio.h"
#include "ad74413r.h"
#include "iio_trigger.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Diagnostics channel state.
 */
struct ad74413r_diag_channel_config {
	bool enabled;
	enum ad74413r_diag_mode function;
};

static const char * const ad74413r_diag_available[] = {
	[AD74413R_DIAG_AGND] = "agnd",
	[AD74413R_DIAG_TEMP] = "temp",
	[AD74413R_DIAG_AVDD] = "avdd",
	[AD74413R_DIAG_AVSS] = "avss",
	[AD74413R_DIAG_REFOUT] = "refout",
	[AD74413R_DIAG_ALDO_5V] = "aldo_5v",
	[AD74413R_DIAG_ALDO_1V8] = "aldo_1v8",
	[AD74413R_DIAG_DLDO_1V8] = "dldo_1v8",
	[AD74413R_DIAG_DVCC] = "dvcc",
	[AD74413R_DIAG_IOVDD] = "iovdd",
	[AD74413R_SENSEL_A] = "sensel_a",
	[AD74413R_SENSEL_B] = "sensel_b",
	[AD74413R_SENSEL_C] = "sensel_c",
	[AD74413R_SENSEL_D] = "sensel_d",
};

/**
 * @brief Descriptor that stores a iio specific state.
 */
struct ad74413r_iio_desc {
	struct ad74413r_desc *ad74413r_desc;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;
	struct ad74413r_channel_config channel_configs[AD74413R_N_CHANNELS];
	struct iio_hw_trig *trigger;
	enum ad74413r_conv_seq conv_state;
	struct ad74413r_diag_channel_config
		diag_channel_configs[AD74413R_N_DIAG_CHANNELS];
};

/**
 * @brief Init parameter for the iio descriptor.
 */
struct ad74413r_iio_desc_init_param {
	struct ad74413r_init_param *ad74413r_init_param;
	struct ad74413r_channel_config channel_configs[AD74413R_N_CHANNELS];
	struct iio_hw_trig *trigger;
	struct ad74413r_diag_channel_config
		diag_channel_configs[AD74413R_N_DIAG_CHANNELS];
};

/**
 * @brief Structure used to store a set of channel specific to an operation mode.
 */
struct ad74413r_channel_map {
	struct iio_channel *channels;
	uint32_t num_channels;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int ad74413r_iio_init(struct ad74413r_iio_desc **,
		      struct ad74413r_iio_desc_init_param *);
int ad74413r_iio_remove(struct ad74413r_iio_desc *);

#endif /** IIO_AD74413R_H */
