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

/******************************************************************************/
/********************** SSD1306 Command Definitions *************************/
/******************************************************************************/

/* Display Control Commands */
#define SSD1306_CMD_DISPLAY_OFF                 0xAE  /* Turn display OFF */
#define SSD1306_CMD_DISPLAY_ON                  0xAF  /* Turn display ON */
#define SSD1306_CMD_DISPLAY_NORMAL              0xA6  /* Normal display (non-inverted) */
#define SSD1306_CMD_DISPLAY_INVERT              0xA7  /* Inverted display */
#define SSD1306_CMD_DISPLAY_ALL_ON_RESUME       0xA4  /* Resume to RAM content display */
#define SSD1306_CMD_DISPLAY_ALL_ON              0xA5  /* Entire display ON (ignore RAM) */

/* Addressing Commands */
#define SSD1306_CMD_SET_MEMORY_MODE             0x20  /* Set Memory Addressing Mode */
#define SSD1306_CMD_SET_COLUMN_ADDR             0x21  /* Set Column Address */
#define SSD1306_CMD_SET_PAGE_ADDR               0x22  /* Set Page Address */
#define SSD1306_CMD_SET_PAGE_START_ADDR         0xB0  /* Set Page Start Address (0xB0-0xB7) */

/* Hardware Configuration Commands */
#define SSD1306_CMD_SET_DISPLAY_START_LINE      0x40  /* Set Display Start Line (0x40-0x7F) */
#define SSD1306_CMD_SET_SEGMENT_REMAP_0         0xA0  /* Set Segment Re-map (column 0 mapped to SEG0) */
#define SSD1306_CMD_SET_SEGMENT_REMAP_127       0xA1  /* Set Segment Re-map (column 127 mapped to SEG0) */
#define SSD1306_CMD_SET_MULTIPLEX_RATIO         0xA8  /* Set Multiplex Ratio */
#define SSD1306_CMD_SET_COM_OUTPUT_SCAN_UP      0xC0  /* Set COM Output Scan Direction (normal) */
#define SSD1306_CMD_SET_COM_OUTPUT_SCAN_DOWN    0xC8  /* Set COM Output Scan Direction (remapped) */
#define SSD1306_CMD_SET_DISPLAY_OFFSET          0xD3  /* Set Display Offset */
#define SSD1306_CMD_SET_COM_PINS_CONFIG         0xDA  /* Set COM Pins Hardware Configuration */

/* Timing & Driving Scheme Commands */
#define SSD1306_CMD_SET_DISPLAY_CLOCK_DIV       0xD5  /* Set Display Clock Divide Ratio/Oscillator Frequency */
#define SSD1306_CMD_SET_PRECHARGE_PERIOD        0xD9  /* Set Pre-charge Period */
#define SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL    0xDB  /* Set VCOMH Deselect Level */

/* Charge Pump Commands */
#define SSD1306_CMD_CHARGE_PUMP_SETTING         0x8D  /* Charge Pump Setting */

/* Contrast Control Commands */
#define SSD1306_CMD_SET_CONTRAST                0x81  /* Set Contrast Control */

/* I2C Control Bytes */
#define SSD1306_I2C_CMD_BYTE                    0x00  /* I2C command byte */
#define SSD1306_I2C_DATA_BYTE                   0x40  /* I2C data byte */

/* Memory Addressing Modes */
#define SSD1306_MEMORY_MODE_HORIZONTAL          0x00  /* Horizontal Addressing Mode */
#define SSD1306_MEMORY_MODE_VERTICAL            0x01  /* Vertical Addressing Mode */
#define SSD1306_MEMORY_MODE_PAGE                0x02  /* Page Addressing Mode */

/* Common Parameter Values */
#define SSD1306_MULTIPLEX_RATIO_64              0x3F  /* 64MUX (for 128x64 display) */
#define SSD1306_MULTIPLEX_RATIO_32              0x1F  /* 32MUX (for 128x32 display) */
#define SSD1306_COM_PINS_CONFIG_SEQUENTIAL      0x02  /* Sequential COM pin configuration */
#define SSD1306_COM_PINS_CONFIG_ALTERNATIVE     0x12  /* Alternative COM pin configuration */
#define SSD1306_COM_PINS_CONFIG_ALTERNATIVE_64  0xD2  /* Alternative COM pin config for 64-row displays */
#define SSD1306_CHARGE_PUMP_ENABLE              0x14  /* Enable charge pump */
#define SSD1306_CHARGE_PUMP_DISABLE             0x10  /* Disable charge pump */
#define SSD1306_DISPLAY_OFFSET_0                0x00  /* No display offset */
#define SSD1306_DISPLAY_START_LINE_0            0x40  /* Start line 0 */
#define SSD1306_CONTRAST_DEFAULT                0x7F  /* Default contrast value */
#define SSD1306_CLOCK_DIV_DEFAULT               0x80  /* Default clock divide ratio */

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
			buff_tmp[0] = SSD1306_I2C_DATA_BYTE;
		else							/* Command transmit */
			buff_tmp[0] = SSD1306_I2C_CMD_BYTE;
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

	command[0] = SSD1306_CMD_DISPLAY_OFF;
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
	command[0] = SSD1306_CMD_SET_MULTIPLEX_RATIO;
	command[1] = SSD1306_MULTIPLEX_RATIO_64;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_SET_DISPLAY_OFFSET;
	command[1] = SSD1306_DISPLAY_OFFSET_0;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_SET_DISPLAY_START_LINE;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_SET_SEGMENT_REMAP_127;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_SET_COM_OUTPUT_SCAN_DOWN;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_SET_COM_PINS_CONFIG;
	command[1] = SSD1306_COM_PINS_CONFIG_ALTERNATIVE_64;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_SET_CONTRAST;
	command[1] = SSD1306_CONTRAST_DEFAULT;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_DISPLAY_ALL_ON_RESUME;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_DISPLAY_INVERT;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_SET_DISPLAY_CLOCK_DIV;
	command[1] = SSD1306_CLOCK_DIV_DEFAULT;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_CHARGE_PUMP_SETTING;
	command[1] = SSD1306_CHARGE_PUMP_ENABLE;
	ret = ssd1306_buffer_transmit(extra, command, 2U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_DISPLAY_ON;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	no_os_udelay(3U);
	command[0] = SSD1306_CMD_DISPLAY_NORMAL;
	ret = ssd1306_buffer_transmit(extra, command, 1U, SSD1306_CMD);
	if (ret != 0)
		return -1;

	// set addressing mode
	no_os_udelay(3U);
	command[0] = SSD1306_CMD_SET_MEMORY_MODE;
	command[1] = SSD1306_MEMORY_MODE_HORIZONTAL;
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

	command[0] = SSD1306_CMD_SET_COLUMN_ADDR;
	command[1] = column * 8;
	command[2] = device->cols_nb * 8 - 1U;
	ret = ssd1306_buffer_transmit(extra, command, 3U, SSD1306_CMD);
	if (ret != 0)
		return -EFAULT;
	command[0] = SSD1306_CMD_SET_PAGE_ADDR;
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
