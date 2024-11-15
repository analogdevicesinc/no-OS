/***************************************************************************//**
 *   @file   ltc2358.h
 *   @brief  Implementation of LTC2358 Driver.
 *   @author Kim Seer Paller (kimseer.paller@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LTC2358_H
#define LTC2358_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define LTC2358_BYTES_PER_CH    3
#define LTC2358_CHANNEL_MSK     NO_OS_GENMASK(2, 0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct ltc2358_init_param
 * @brief Structure holding the parameters for ltc2358 initialization.
 */
struct ltc2358_init_param {
	/* SPI Initialization structure. */
	struct no_os_spi_init_param *spi_init;
};

/**
 * @struct ltc2358_dev
 * @brief Structure holding ltc2358 descriptor.
 */
struct ltc2358_dev {
	/* SPI handler */
	struct no_os_spi_desc *spi_desc;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Creates 24-bit configuration word for the 8 channels. */
void ltc2358_create_config_word(uint8_t channel, uint8_t config_number,
				uint32_t *config_word);

/* Parse single channel data. */
int32_t ltc2358_channel_data(struct ltc2358_dev *dev, uint32_t config_word,
			     uint8_t data_array[24], uint8_t channel, uint32_t *readval);

/* Initializes the LTC2358. */
int32_t ltc2358_init(struct ltc2358_dev **device,
		     struct ltc2358_init_param *init_param);

/* Free the resources allocated by ltc2358_init(). */
int32_t ltc2358_remove(struct ltc2358_dev *dev);

#endif /* LTC2358_H */
