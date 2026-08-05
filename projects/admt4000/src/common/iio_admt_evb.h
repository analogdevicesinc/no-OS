/***************************************************************************//**
 *   @file   iio_admt_evb.h
 *   @brief  Header file of IIO wrapper for the evaluation peripherals.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#ifdef IIO_SUPPORT
#ifndef IIO_ADMT_EVB_H
#define IIO_ADMT_EVB_H

#include "iio.h"
#include <errno.h>

enum admt_evb_attr {
	ADMT_EVB_SHDN,
	ADMT_EVB_V_EN,
	ADMT_COIL_RS,
};

struct admt_evb_iio_desc {
	struct iio_device *iio_dev;
	struct no_os_gpio_desc *gpio_v_en_desc;
	struct no_os_gpio_desc *gpio_shdn_n_desc;
	struct no_os_gpio_desc *gpio_coil_rs_desc;
};

struct admt_evb_iio_init_param {
	struct no_os_gpio_init_param gpio_v_en_ip;
	struct no_os_gpio_init_param gpio_shdn_n_ip;
	struct no_os_gpio_init_param gpio_coil_rs_ip;
};

int admt_evb_iio_init(struct admt_evb_iio_desc **,
		      struct admt_evb_iio_init_param *);

int admt_evb_iio_remove(struct admt_evb_iio_desc *);

#endif
#endif
