/***************************************************************************//**
 *   @file   iio_max14906.h
 *   @brief  Header file of MAX14906 IIO Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
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
