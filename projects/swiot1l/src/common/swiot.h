/***************************************************************************//**
 *   @file   swiot.h
 *   @brief  Header file for the swiot IIO device.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef IIO_SWIOT_H
#define IIO_SWIOT_H

#include <stdbool.h>
#include "no_os_gpio.h"
#include "ad74413r.h"
#include "iio_ad74413r.h"
#include "max14906.h"
#include "iio_max14906.h"
#include "adin1110.h"

#define SWIOT_CHANNELS	4

static const char *const ad74413r_function_available[] = {
	[AD74413R_HIGH_Z] = "high_z",
	[AD74413R_VOLTAGE_OUT] = "voltage_out",
	[AD74413R_CURRENT_OUT] = "current_out",
	[AD74413R_VOLTAGE_IN] = "voltage_in",
	[AD74413R_CURRENT_IN_EXT] = "current_in_ext",
	[AD74413R_CURRENT_IN_LOOP] = "current_in_loop",
	[AD74413R_RESISTANCE] = "resistance",
	[AD74413R_DIGITAL_INPUT] = "digital_input",
	[AD74413R_DIGITAL_INPUT_LOOP] = "digital_input_loop",
	[AD74413R_CURRENT_IN_EXT_HART] = "current_in_ext_hart",
	[AD74413R_CURRENT_IN_LOOP_HART] = "current_in_loop_hart"
};

enum swiot_device {
	SWIOT_AD74413R,
	SWIOT_MAX14906,
	SWIOT_DEV_CNT,
};

enum swiot_mode {
	SWIOT_CONFIG,
	SWIOT_RUNTIME,
};

struct swiot_config_state {
	bool enabled;
	enum swiot_device device;
	uint32_t function;
};

struct swiot_iio_desc {
	struct ad74413r_iio_desc *ad74413r;
	struct max149x6_iio_desc *max14906;
	struct adin1110_desc *adin1110;
	struct iio_device *iio_dev;
	uint32_t active_channels;
	uint8_t no_of_active_channels;

	enum swiot_mode mode;
	bool mode_change;
	struct no_os_gpio_desc *psu_gpio;
	struct no_os_gpio_desc *identify_gpio;

	struct max14906_ch_config max14906_configs[MAX14906_CHANNELS];
	struct ad74413r_channel_config ad74413r_configs[AD74413R_N_CHANNELS];
	struct ad74413r_diag_channel_config
		ad74413r_diag_configs[AD74413R_N_DIAG_CHANNELS];
};

struct swiot_iio_desc_init_param {
	struct ad74413r_iio_desc *ad74413r;
	struct max149x6_iio_desc *max14906;
	struct no_os_gpio_init_param psu_gpio_param;
	struct no_os_gpio_init_param identify_gpio_param;
	enum swiot_mode mode;
};

int swiot_iio_init(struct swiot_iio_desc **,
		   struct swiot_iio_desc_init_param *);
int swiot_iio_remove(struct swiot_iio_desc *);

#endif
