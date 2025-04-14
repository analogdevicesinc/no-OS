/***************************************************************************//**
 *   @file   ssd_1306.h
 *   @brief  Header file for ssd_1306 Driver.
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

#ifndef SSD_1306_H
#define SSD_1306_H

#include <stdint.h>
#include <stdlib.h>
#include "display.h"
#include "no_os_gpio.h"

enum comm_type {
	SSD1306_I2C,
	SSD1306_SPI
};

enum transmit_type {
	SSD1306_CMD,
	SSD1306_DATA
};

#define SSD1306_I2C_ADDR	0x3C

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
	/* I2C initial param*/
	struct no_os_i2c_init_param      *i2c_ip;
	/* I2C descriptor*/
	struct no_os_i2c_desc           *i2c_desc;
	/* Communication type */
	enum comm_type                   comm_type;
} ssd_1306_extra;

extern const struct display_controller_ops ssd1306_ops;
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

/** Print entire screen buffer */
int32_t ssd_1306_print_buffer(struct display_dev *device, char *buffer);

#endif
