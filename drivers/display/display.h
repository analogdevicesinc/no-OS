/***************************************************************************//**
 *   @file   display.h
 *   @brief  Header file for display Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DISPLAY_H
#define DISPLAY_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct display_dev
 * @brief Display Device Descriptor.
 */
struct display_dev {
	/** Number of columns */
	uint8_t                    cols_nb;
	/** Number of rows */
	uint8_t                    rows_nb;
	/** DISPLAY controller specific ops */
	const struct display_controller_ops *controller_ops;
	/**  Display extra parameters (device specific) */
	void		               *extra;
};

/**
 * @struct display_init_param
 * @brief Display Device Initial Parameters.
 */
struct display_init_param {
	/** Number of columns */
	uint8_t                    cols_nb;
	/** Number of rows */
	uint8_t                    rows_nb;
	/** DISPLAY controller specific ops */
	const struct display_controller_ops *controller_ops;
	/**  Display extra parameters (device specific) */
	void		               *extra;
};

/**
 * @struct display_controller_ops
 * @brief Structure holding display controller function pointers that point to the controller
 * specific function
 */
struct display_controller_ops {
	/** Initialize controller for display usage. */
	int32_t (*init)(struct display_dev *);
	/** Turn display on/off */
	int32_t (*display_on_off)(struct display_dev *, uint8_t);
	/** Move cursor */
	int32_t (*move_cursor)(struct display_dev *, uint8_t, uint8_t);
	/** Print character by ascii number */
	int32_t (*print_char)(struct display_dev *, uint8_t, uint8_t,
			      uint8_t);
	/** Removes resources allocated by device */
	int32_t (*remove)(struct display_dev *);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Initializes the display peripheral. */
int32_t display_init(struct display_dev **device,
		     const struct display_init_param *param);

/* Frees the resources allocated by display_init(). */
int32_t display_remove(struct display_dev *device);

/** Turns display on */
int32_t display_on(struct display_dev *device);

/** Turns display off */
int32_t display_off(struct display_dev *device);

/** Moves cursor to desired position. */
int32_t display_move_cursor(struct display_dev *device, uint8_t row,
			    uint8_t column);

/** Clears data on display. */
int32_t display_clear(struct display_dev *device);

/** Prints char string at selected position. */
int32_t display_print_string(struct display_dev *device, char *msg,
			     uint8_t row, uint8_t column);

/** Prints single char at selected position. */
int32_t display_print_char(struct display_dev *device, char chr,
			   uint8_t row, uint8_t column);

#endif
