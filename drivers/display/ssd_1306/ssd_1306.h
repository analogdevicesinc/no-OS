/***************************************************************************//**
 *   @file   ssd_1306.h
 *   @brief  Header file for ssd_1306 Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SSD_1306_H
#define SSD_1306_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "display.h"
#include "no_os_gpio.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct ssd_1306_extra
 * @brief Extra parameters needed for ssd_1306 usage.
 */
typedef struct ssd_1306_extra {
	/** Data/Command pin gpio initial param */
	struct no_os_gpio_init_param	   *dc_pin_ip;
	/** RESET pin gpio initial param */
	struct no_os_gpio_init_param     *reset_pin_ip;
	/** Data/Command pin gpio desc */
	struct no_os_gpio_desc	       *dc_pin;
	/** RESET pin gpio desc*/
	struct no_os_gpio_desc     	   *reset_pin;
	/* SPI initial param */
	struct no_os_spi_init_param      *spi_ip;
	/* SPI descriptor*/
	struct no_os_spi_desc	           *spi_desc;
} ssd_1306_extra;

extern const struct display_controller_ops ssd1306_ops;
/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Initialize the ssd_1306 peripheral for display operation. */
int32_t ssd_1306_init(struct display_dev *device);

/** ssd_1306 ssd_1306 turns display on/off. */
int32_t ssd_1306_display_on_off(struct display_dev *device, uint8_t on_off);

/*  Moves cursor to desired row/column. */
int32_t ssd_1306_move_cursor(struct display_dev *device, uint8_t row,
			     uint8_t column);

/** Prints character at selected position. */
int32_t ssd_1306_print_ascii(struct display_dev *device, uint8_t ascii,
			     uint8_t row, uint8_t column);

/** Removes resources allocated by device. */
int32_t ssd_1306_remove(struct display_dev *device);

#endif
