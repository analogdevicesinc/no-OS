/***************************************************************************//**
 *   @file   AD799x.h
 *   @brief  Header file of AD799x Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __AD799X_H__
#define __AD799X_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_i2c.h"

/******************************************************************************/
/******************************** AD799x **************************************/
/******************************************************************************/

/* Configuration Register definition. */
#define AD799X_CHANNEL(ch)        ((1 << ch) << 4)
#define AD799X_REF_SEL	          (1 << 3)
#define AD799X_FLTR		  (1 << 2)
#define AD799X_BIT_TRIAL_DELAY	  (1 << 1)
#define AD799X_SAMPLE_DELAY	  (1 << 0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Supported devices */
enum ad799x_type {
	AD7991,
	AD7995,
	AD7999
};

struct ad799x_dev {
	/* I2C */
	struct no_os_i2c_desc	*i2c_desc;
	/* Device Settings */
	uint8_t         bits_number;
};

struct ad799x_init_param {
	/* I2C */
	struct no_os_i2c_init_param		i2c_init;
	/* Device Settings */
	enum ad799x_type	part_number;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes I2C. */
int8_t ad799x_init(struct ad799x_dev **device,
		   struct ad799x_init_param init_param);

/*! Free the resources allocated by ad799x_init(). */
int32_t ad799x_remove(struct ad799x_dev *dev);

/*! Writes data into the Configuration Register. */
void ad799x_set_configuration_reg(struct ad799x_dev *dev,
				  uint8_t register_value);

/*! Reads the High byte and the Low byte of the conversion. */
void ad799x_get_conversion_result(struct ad799x_dev *dev,
				  int16_t* conv_value,
				  int8_t* channel);

/*! Converts a raw sample to volts.*/
float ad799x_convert_to_volts(struct ad799x_dev *dev,
			      int16_t raw_sample,
			      float v_ref);

#endif /* __AD799X_H__ */
