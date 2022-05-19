/***************************************************************************//**
 *   @file   ssd_1306.c
 *   @brief  Implementation of ssd_1306 Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include "ssd_1306.h"
#include "no_os_error.h"
#include "no_os_spi.h"
#include "no_os_delay.h"
#include <string.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* @defines for ssd_1306 pins signal level */
#define SSD1306_RST_ON     	0U
#define SSD1306_RST_OFF    	1U
#define SSD1306_DC_DATA    	1U
#define SSD1306_DC_CMD     	0U
#define SSD1306_DISP_ON    	0xAFU
#define SSD1306_DISP_OFF   	0xAEU
#define SSD1306_CHARSZ  	8U

const struct display_controller_ops ssd1306_ops = {
	.init = &ssd_1306_init,
	.display_on_off = &ssd_1306_display_on_off,
	.move_cursor = &ssd_1306_move_cursor,
	.print_char = &ssd_1306_print_ascii,
	.remove = &ssd_1306_remove
};

extern const uint8_t no_os_chr_8x8[128][8];

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes ssd_1306 for display screening.
 *
 * @param device - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t ssd_1306_init(struct display_dev *device)
{
	int32_t	ret;
	uint8_t command[3];
	ssd_1306_extra *extra;

	extra = device->extra;
	ret = no_os_spi_init(&extra->spi_desc, extra->spi_ip);
	if (ret != 0)
		return -1;
	ret = no_os_gpio_get(&extra->dc_pin, extra->dc_pin_ip);
	if (ret != 0)
		return -1;
	ret = no_os_gpio_get(&extra->reset_pin, extra->reset_pin_ip);
	if (ret != 0)
		return -1;

	// initial pin state
	ret = no_os_gpio_direction_output(extra->dc_pin, SSD1306_DC_CMD);
	if (ret != 0)
		return -1;
	ret = no_os_gpio_direction_output(extra->reset_pin, SSD1306_RST_OFF);
	if (ret != 0)
		return -1;

	command[0] = 0xAE;
	ret = no_os_spi_write_and_read(extra->spi_desc, command, 1U);
	if (ret != 0)
		return -1;
	ret = no_os_gpio_set_value(extra->reset_pin, SSD1306_RST_ON);
	if (ret != 0)
		return -1;
	// Post reset delay, Treset=3us (See datasheet -> power on sequence)
	no_os_udelay(3U);
	ret = no_os_gpio_set_value(extra->reset_pin, SSD1306_RST_OFF);
	if (ret != 0)
		return -1;
	// charge pump
	command[0] = 0x8D;
	command[1] = 0x14;
	ret = no_os_spi_write_and_read(extra->spi_desc, command, 2U);
	if (ret != 0)
		return -1;
	// pre-charge
	command[0] = 0xD9;
	command[1] = 0xF1;
	ret = no_os_spi_write_and_read(extra->spi_desc, command, 2U);
	if (ret != 0)
		return -1;
	// set contrast
	command[0] = 0x81;
	command[1] = 0xFF;
	ret = no_os_spi_write_and_read(extra->spi_desc, command, 2U);
	if (ret != 0)
		return -1;
	// set segment remap
	command[0] = 0xA0;
	ret = no_os_spi_write_and_read(extra->spi_desc, command, 1U);
	if (ret != 0)
		return -1;
	// set scan direction
	command[0] = 0xC0;
	ret = no_os_spi_write_and_read(extra->spi_desc, command, 1U);
	if (ret != 0)
		return -1;
	// set COM pin
	command[0] = 0xDA;
	command[1] = 0x00;
	ret = no_os_spi_write_and_read(extra->spi_desc, command, 2U);
	if (ret != 0)
		return -1;
	// show written memory on screen
	command[0] = 0xA4;
	ret = no_os_spi_write_and_read(extra->spi_desc, command, 1U);
	if (ret != 0)
		return -1;
	command[0] = 0x20;	// memory addressing mode
	command[1] = 0x00;	// horizontal addressing
	return no_os_spi_write_and_read(extra->spi_desc, command, 2U);
}

/***************************************************************************//**
 * @brief  ssd_1306 turns display on/off.
 *
 * @param device - The device structure
 * @param on_off - Display state
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t ssd_1306_display_on_off(struct display_dev *device, uint8_t on_off)
{
	int32_t	ret;
	uint8_t command;
	ssd_1306_extra *extra;

	extra = device->extra;
	ret = no_os_gpio_set_value(extra->dc_pin, SSD1306_DC_CMD);
	if (ret != 0)
		return -1;
	command = (on_off == true) ? SSD1306_DISP_ON : SSD1306_DISP_OFF;
	return no_os_spi_write_and_read(extra->spi_desc, &command, 1U);
}

/***************************************************************************//**
 * @brief  Moves cursor to desired row/column.
 *
 * @param device - The device structure
 * @param row    - row
 * @param column - column
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t ssd_1306_move_cursor(struct display_dev *device, uint8_t row,
			     uint8_t column)
{
	int32_t	ret;
	uint8_t command[3];
	ssd_1306_extra *extra;

	extra = device->extra;
	ret = no_os_gpio_set_value(extra->dc_pin, SSD1306_DC_CMD);
	if (ret != 0)
		return -1;
	command[0] = 0x21;
	command[1] = column*8;
	command[2] = device->cols_nb * 8 - 1U;
	ret = no_os_spi_write_and_read(extra->spi_desc, command, 3U);
	if (ret != 0)
		return -1;
	command[0] = 0x22;
	command[1] = row;
	command[2] = device->rows_nb - 1U;
	return no_os_spi_write_and_read(extra->spi_desc, command, 3U);;
}

/***************************************************************************//**
 * @brief Prints character at selected position.
 *
 * @param device - The device structure.
 * @param ascii  - corresponding number to ascii table
 * @param row    - row
 * @param column - column
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t ssd_1306_print_ascii(struct display_dev *device, uint8_t ascii,
			     uint8_t row, uint8_t column)
{
	int32_t ret;
	ssd_1306_extra *extra;
	uint8_t ch[SSD1306_CHARSZ];
	memcpy(ch, no_os_chr_8x8[ascii], SSD1306_CHARSZ);

	extra = device->extra;
	ret = ssd_1306_move_cursor(device, row, column);
	if (ret != 0)
		return -1;
	ret = no_os_gpio_set_value(extra->dc_pin, SSD1306_DC_DATA);
	if (ret != 0)
		return -1;
	return no_os_spi_write_and_read(extra->spi_desc, ch, SSD1306_CHARSZ);
}

/***************************************************************************//**
 * @brief Removes resources allocated by device.
 *
 * @param device - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t ssd_1306_remove(struct display_dev *device)
{
	int32_t ret;
	ssd_1306_extra *extra;

	extra = device->extra;
	ret = no_os_gpio_remove(extra->reset_pin);
	if (ret != 0)
		return -1;
	ret = no_os_gpio_remove(extra->dc_pin);
	if (ret != 0)
		return -1;
	return no_os_spi_remove(extra->spi_desc);
}
