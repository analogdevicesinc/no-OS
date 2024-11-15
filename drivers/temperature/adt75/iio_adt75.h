/***************************************************************************//**
 *   @file   iio_adt75.h
 *   @brief  Header file of the IIO ADT75 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_ADT75_H
#define IIO_ADT75_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio.h"
#include "adt75.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @brief Descriptor that stores an iio specific state.
 */
struct adt75_iio_desc {
	struct adt75_desc *adt75_desc;
	struct iio_device *iio_dev;
	uint32_t active_channels;
};

/**
 * @brief Init parameter for the iio descriptor.
 */
struct adt75_iio_init_param {
	struct adt75_init_param *adt75_init_param;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Intialize the iio descriptor */
int adt75_iio_init(struct adt75_iio_desc **, struct adt75_iio_init_param *);

/** Free the iio descriptor */
int adt75_iio_remove(struct adt75_iio_desc *);

#endif /** IIO_ADT75_H */
