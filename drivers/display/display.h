/***************************************************************************//**
 *   @file   display.h
 *   @brief  Header file for display Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"

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
	/** Print screen buffer */
	int32_t (*print_buffer)(struct display_dev *, char *);
};

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

/** Print display buffer on entire screen */
int32_t display_print_buffer(struct display_dev *device, char *buffer);

#endif
