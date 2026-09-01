/***************************************************************************//**
 *   @file   iio_ad4080.h
 *   @brief  Implementation of iio_ad4080.c
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __IIO_AD4080_H__
#define __IIO_AD4080_H__
#include <stddef.h>
#include <errno.h>
#include "no_os_irq.h"
#include "iio.h"
#include "iio_types.h"
#include "ad4080.h"

#define AD4080_IIO_APP_DEVICE_NAME_LEN 	16

/* Developer notes. We pattern a 15 second around the Max32666 Fthr which has
 * a core clock of 96Mhz or ~10.4ns.
 */
#define AD4080_FIFO_TIMEOUT 	1440000000

/**
 * @struct iio_ad4080_fifo_struct
 */
struct iio_ad4080_fifo_struct {
	struct ad4080_dev *ad4080;
	struct no_os_gpio_desc *ff_full;
	struct no_os_irq_ctrl_desc *irq_desc;
	size_t i_gp;

	size_t watermark;
	uint8_t *raw_fifo;
	size_t bufsize;
	uint32_t *formatted_fifo;
	size_t formatted_bufsize;
};

/**
 * @struct iio_ad4080_desc
 */
struct iio_ad4080_desc {
	struct ad4080_dev *ad4080;
	struct iio_ad4080_fifo_struct fifo;
	struct no_os_gpio_desc *afe_ctrl;

	/* Per-instance channel/scan_type so two AD408x parts of different
	 * resolution don't clobber each other's realbits. */
	struct scan_type scan_type;
	struct iio_channel ch;

	uint32_t app_device_count;
	char app_device_name[AD4080_IIO_APP_DEVICE_NAME_LEN];
	unsigned long sampling_frequency_idx;
};

/**
 * @brief Structure holding the AD4080 IIO initialization parameter.
 */
struct iio_ad4080_init_param {
	struct ad4080_init_param *ad4080_init_param;
	struct no_os_gpio_init_param *ff_full_init_param;
	struct no_os_gpio_init_param *afe_ctrl_init_param;
	size_t i_gp;
	size_t watermark;
};

/**
 * @brief Initializes the IIO AD4080 descriptor.
 */
int iio_ad4080_init(struct iio_ad4080_desc **iio_ad4080,
		    struct iio_ad4080_init_param *iio_ad4080_init_param);

/**
 * @brief teardown the ad4080 IIO driver
 */
void iio_ad4080_fini(struct iio_ad4080_desc *iio_ad4080);

/**
 * @brief Checks if the given pointer is a valid AD4080 IIO descriptor.
 */
int ad4080_device(struct iio_ad4080_desc *iio_ad4080,
		  struct ad4080_dev **ad4080);

/**
 * @brief Initializes the IIO device structure for the AD4080 device.
 */
int ad4080_iio_device(struct iio_ad4080_desc *iio_ad4080,
		      struct iio_device *iio_device);

#endif /* _IIO_AD4080_H__ */
