/***************************************************************************//**
 *   @file   iio_ad7091r8.h
 *   @brief  Header file of IIO AD7091R8 driver header file.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_AD7091R8_H
#define IIO_AD7091R8_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio.h"

/******************************************************************************/
/***************************** Define Section *********************************/
/******************************************************************************/
#define ad7091r8_iio_device(chans) {					\
	.num_ch = NO_OS_ARRAY_SIZE(chans),				\
	.channels = chans,						\
	.pre_enable = (int32_t (*)())ad7091r8_buffer_preenable,		\
	.trigger_handler = (int32_t (*)())ad7091r8_trigger_handler,	\
	.debug_reg_read = (int32_t (*)())ad7091r8_iio_read_reg,		\
	.debug_reg_write = (int32_t (*)())ad7091r8_iio_write_reg	\
}

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
extern struct iio_trigger ad7091r8_iio_timer_trig_desc;

/** @struct ad7091r8_iio_dev
 *  @brief AD7091r8 IIO device descriptor structure
 */
struct ad7091r8_iio_dev {
	struct ad7091r8_dev *ad7091r8_dev;
	struct iio_device *iio_dev;
};

/** @struct ad7091r8_iio_dev_init_param
 *  @brief AD7091r8 IIO device initial parameters structure
 */
struct ad7091r8_iio_dev_init_param {
	struct ad7091r8_init_param *ad7091r8_dev_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*! Function to be called to initialize a AD7091R-2/-4/-8 device. */
int ad7091r8_iio_init(struct ad7091r8_iio_dev **iio_dev,
		      struct ad7091r8_iio_dev_init_param *init_param);

/*! Function to be called to remove a AD7091R-2/-4/-8 device. */
int ad7091r8_iio_remove(struct ad7091r8_iio_dev *desc);

#endif /** IIO_AD7091R8_H */
