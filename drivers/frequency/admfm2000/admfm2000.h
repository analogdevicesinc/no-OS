/***************************************************************************//**
 *   @file   admfm2000.h
 *   @brief  Header file for admfm2000 Driver.
 *   @author Ramona Nechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef SRC_ADMFM2000_H_
#define SRC_ADMFM2000_H_

#include <stdint.h>
#include "no_os_gpio.h"

#define ADMFM2000_MIXER_MODE		0
#define ADMFM2000_DIRECT_IF_MODE	1
#define ADMFM2000_DSA_GPIOS		5
#define ADMFM2000_MODE_GPIOS		2
#define ADMFM2000_MAX_GAIN		0
#define ADMFM2000_MIN_GAIN		-31000
#define ADMFM2000_MAX_GAIN_RAW		31
#define ADMFM2000_MIN_GAIN_RAW		0
#define ADMFM2000_DEFAULT_GAIN		-0x20
#define ADMFM2000_NUM_CHANNELS		2

struct admfm2000_init_param {
	/* Mixer Mode */
	uint8_t mixer_mode;
	/* GAIN */
	int32_t dsa_gain;
	/* GPIO Control Switch chan 0&1 */
	struct no_os_gpio_init_param *gpio_sw_param[2][2];
	/* GPIO Control DSA chan 0&1 */
	struct no_os_gpio_init_param *gpio_dsa_param[2][5];
};

struct admfm2000_dev {
	/* GPIO Control Switch chan 0&1 */
	struct no_os_gpio_desc *gpio_sw[2][2];
	/* GPIO Control DSA chan 0&1 */
	struct no_os_gpio_desc *gpio_dsa[2][5];
};

int admfm2000_init(struct admfm2000_dev **device,
		   struct admfm2000_init_param *init_param);

int admfm2000_remove(struct admfm2000_dev *dev);

int admfm2000_set_dsa_raw(struct admfm2000_dev *dev, uint8_t chan,
			  int32_t dsa_raw);

int admfm2000_get_dsa_raw(struct admfm2000_dev *dev, uint8_t chan,
			  int32_t *dsa_raw);

int admfm2000_get_channel_mode(struct admfm2000_dev *dev, uint8_t mode);

int admfm2000_set_channel_config(struct admfm2000_dev *dev, uint8_t config);

#endif /* SRC_ADMFM2000_H_ */
