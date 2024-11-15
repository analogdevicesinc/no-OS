/***************************************************************************//**
*   @file   iio_adpd188.h
*   @brief  Header of the ADPD188 IIO driver.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_ADPD188_H
#define IIO_ADPD188_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "adpd188.h"
#include "iio.h"

#define ADPD1080_CHANNEL_NO		8
#define ADPD1080_WORD_BIT_SIZE		16
#define ADPD1080_BITS_PER_SAMPLE	32
#define ADPD1080_WORDS_PER_SAMPLE	\
	(ADPD1080_BITS_PER_SAMPLE / ADPD1080_WORD_BIT_SIZE)

struct adpd188_iio_init_param {
	struct adpd188_init_param drv_init_param;
};

struct adpd188_iio_desc {
	struct adpd188_dev *drv_dev;
	uint8_t ch_mask;
};

extern struct iio_device iio_adpd188_device;

int32_t adpd188_iio_init(struct adpd188_iio_desc **device,
			 struct adpd188_iio_init_param *init_param);
int32_t adpd188_iio_remove(struct adpd188_iio_desc *dev);

#endif /* IIO_ADPD188_H */

