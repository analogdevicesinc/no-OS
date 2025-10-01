/***************************************************************************//**
 *   @file   ssd_1306.c
 *   @brief  Implementation of ssd_1306 Driver.
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

#include <stdbool.h>
#include "ssd_1306.h"
#include "no_os_error.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_delay.h"
#include <string.h>

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
	.remove = &ssd_1306_remove,
	.print_buffer = &ssd_1306_print_buffer
};

extern const uint8_t no_os_chr_8x8[128][8];

int32_t ssd1306_buffer_transmit(ssd_1306_extra *extra_param, uint8_t *command,
				int length, enum transmit_type is_data)
{
	switch (extra_param->comm_type) {
	case SSD1306_I2C: {
		uint8_t buff_tmp[length + 1];
		if (is_data == SSD1306_DATA)	/* Data transmit */
			buff_tmp[0] = 0x40;
		else							/* Command transmit */
			buff_tmp[0] = 0x00;
		memcpy(&buff_tmp[1], command, length);
		return no_os_i2c_write(extra_param->i2c_desc, buff_tmp, length + 1, 1);
	}
	case SSD1306_SPI: {
		int32_t ret;
		if (is_data == SSD1306_DATA) {	/* Data transmit */
			if (extra_param->dc_pin) {
				ret = no_os_gpio_set_value(extra_param->dc_pin, SSD1306_DC_DATA);
				if (ret != 0)
					return -EFAULT;
			}
		} else {						/* Command transmit */
			if (extra_param->dc_pin) {
				ret = no_os_gpio_set_value(extra_param->dc_pin, SSD1306_DC_CMD);
				if (ret != 0)
					return -EFAULT;
			}
		}
		return no_os_spi_write_and_read(extra_param->spi_desc, command, length);
	}
	default:
		return -EFAULT;
	}
}


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
	switch (extra->comm_type) {
	case SSD1306_SPI:
		ret = no_os_spi_init(&extra->spi_desc, extra->spi_ip);
		break;
	case SSD1306_I2C:
		ret = no_os_i2c_init(&extra->i2c_desc, extra->i2c_ip);
		break;
	default:
		ret = -EFAULT;
		break;
	}
	if (ret != 0)
		return ret;

	if (extra->dc_pin_ip) {
		ret = no_os_gpio_get(&extra->dc_pin, extra->dc_pin_ip);
		if (ret != 0)
			return -1;
	}
	if (extra->reset_pin_ip) {
		ret = no_os_gpio_get(&extra->reset_pin, extra->reset_pin_ip);
		if (ret != 0)
			return -1;
	}

	// initial pin state
	if (extra->reset_pin) {
		ret = no_os_gpio_direction_output(extra->reset_pin, SSD1306_RST_OFF);
		if (ret != 0)
			return -1;
	}

	command[0] = 0xAE;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	if (extra->reset_pin) {
		ret = no_os_gpio_set_value(extra->reset_pin, SSD1306_RST_ON);
		if (ret != 0)
			return -1;

	}
	// Post reset delay, Treset=3us (See datasheet -> power on sequence)
	no_os_udelay(3U);
	if (extra->reset_pin) {
		ret = no_os_gpio_set_value(extra->reset_pin, SSD1306_RST_OFF);
		if (ret != 0)
			return -1;
	}

	no_os_udelay(3U);
	command[0] = 0xA8;
	command[1] = 0x3F;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0xD3;
	command[1] = 0x00;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0x40;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0xA1;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0xC8;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0xDA;
	command[1] = 0xD2;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0x81;
	command[1] = 0x7F;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0xA4;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0xA7;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0xD5;
	command[1] = 0x80;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0x8D;
	command[1] = 0x14;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = 0xAF;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	// set addressing mode
	no_os_udelay(3U);
	command[0] = 0x20;
	command[1] = 0x00;
	return ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
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

	command = (on_off == true) ? SSD1306_DISP_ON : SSD1306_DISP_OFF;
	return ssd1306_buffer_transmit(extra, &command, 1U, SSD1306_CMD);
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

	command[0] = 0x21;
	command[1] = column * 8;
	command[2] = device->cols_nb * 8 - 1U;
	ret = ssd1306_buffer_transmit(extra, command, 3U, SSD1306_CMD);
	if (ret != 0)
		return -EFAULT;
	command[0] = 0x22;
	command[1] = row;
	command[2] = device->rows_nb - 1U;
	ret = ssd1306_buffer_transmit(extra, command, 3U, SSD1306_CMD);
	if (ret != 0)
		return -EFAULT;
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
		return -EFAULT;

	return ssd1306_buffer_transmit(extra, ch, SSD1306_CHARSZ, SSD1306_DATA);
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
	if (extra->reset_pin) {
		ret = no_os_gpio_remove(extra->reset_pin);
		if (ret != 0)
			return -EFAULT;
	}
	if (extra->dc_pin) {
		ret = no_os_gpio_remove(extra->dc_pin);
		if (ret != 0)
			return -EFAULT;
	}

	if (extra->comm_type == SSD1306_SPI)
		return no_os_spi_remove(extra->spi_desc);
	else if (extra->comm_type == SSD1306_I2C)
		return no_os_i2c_remove(extra->i2c_desc);

	return 0;
}

/**
 * @brief Print entire screen buffer
 *
 * @param device - The device structure.
 * @param buffer - The buffer to be printed.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t ssd_1306_print_buffer(struct display_dev *device, char *buffer)
{
	int32_t ret;

	ssd_1306_extra *extra;

	if (!device || !buffer)
		return -ENOMEM;

	extra = device->extra;

	ret = ssd_1306_move_cursor(device, 0, 0);
	if (ret != 0)
		return ret;

	uint32_t nr_of_frame_bytes = device->cols_nb * device->rows_nb / 8;
	uint8_t nr_of_sendable_frames = nr_of_frame_bytes / 254;
	uint8_t last_frame_bytes = nr_of_frame_bytes % 254;

	for (int i = 0; i < nr_of_sendable_frames; i++) {
		ret = ssd1306_buffer_transmit(extra, &buffer[i * 254], 254, SSD1306_DATA);
		if (ret != 0)
			return ret;
	}
	if (last_frame_bytes > 0) {
		ret = ssd1306_buffer_transmit(extra, &buffer[nr_of_sendable_frames * 254],
					      last_frame_bytes, SSD1306_DATA);
		if (ret != 0)
			return ret;
	}

	return 0;
}
