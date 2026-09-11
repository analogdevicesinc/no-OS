/***************************************************************************//**
 * @file camera_overlay.c
 * @brief Tiny 5x7 RGB565 text renderer for the camera statistics band.
 ******************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#include <stddef.h>
#include <string.h>

#include "camera_overlay.h"

#define GLYPH_WIDTH       5U
#define GLYPH_HEIGHT      7U
#define GLYPH_ADVANCE     6U
#define OVERLAY_FG_BYTE   0xffU
#define OVERLAY_BG_BYTE   0x00U
#define COLOR_KEY_X       272U
#define COLOR_KEY_WIDTH   12U

static const uint8_t digit_glyphs[10][GLYPH_WIDTH] = {
	{ 0x3e, 0x51, 0x49, 0x45, 0x3e },
	{ 0x00, 0x42, 0x7f, 0x40, 0x00 },
	{ 0x42, 0x61, 0x51, 0x49, 0x46 },
	{ 0x21, 0x41, 0x45, 0x4b, 0x31 },
	{ 0x18, 0x14, 0x12, 0x7f, 0x10 },
	{ 0x27, 0x45, 0x45, 0x45, 0x39 },
	{ 0x3c, 0x4a, 0x49, 0x49, 0x30 },
	{ 0x01, 0x71, 0x09, 0x05, 0x03 },
	{ 0x36, 0x49, 0x49, 0x49, 0x36 },
	{ 0x06, 0x49, 0x49, 0x29, 0x1e },
};

static const uint8_t alpha_glyphs[26][GLYPH_WIDTH] = {
	{ 0x7e, 0x11, 0x11, 0x11, 0x7e }, /* A */
	{ 0x7f, 0x49, 0x49, 0x49, 0x36 }, /* B */
	{ 0x3e, 0x41, 0x41, 0x41, 0x22 }, /* C */
	{ 0x7f, 0x41, 0x41, 0x22, 0x1c }, /* D */
	{ 0x7f, 0x49, 0x49, 0x49, 0x41 }, /* E */
	{ 0x7f, 0x09, 0x09, 0x09, 0x01 }, /* F */
	{ 0x3e, 0x41, 0x49, 0x49, 0x7a }, /* G */
	{ 0x7f, 0x08, 0x08, 0x08, 0x7f }, /* H */
	{ 0x00, 0x41, 0x7f, 0x41, 0x00 }, /* I */
	{ 0x20, 0x40, 0x41, 0x3f, 0x01 }, /* J */
	{ 0x7f, 0x08, 0x14, 0x22, 0x41 }, /* K */
	{ 0x7f, 0x40, 0x40, 0x40, 0x40 }, /* L */
	{ 0x7f, 0x02, 0x0c, 0x02, 0x7f }, /* M */
	{ 0x7f, 0x04, 0x08, 0x10, 0x7f }, /* N */
	{ 0x3e, 0x41, 0x41, 0x41, 0x3e }, /* O */
	{ 0x7f, 0x09, 0x09, 0x09, 0x06 }, /* P */
	{ 0x3e, 0x41, 0x51, 0x21, 0x5e }, /* Q */
	{ 0x7f, 0x09, 0x19, 0x29, 0x46 }, /* R */
	{ 0x46, 0x49, 0x49, 0x49, 0x31 }, /* S */
	{ 0x01, 0x01, 0x7f, 0x01, 0x01 }, /* T */
	{ 0x3f, 0x40, 0x40, 0x40, 0x3f }, /* U */
	{ 0x1f, 0x20, 0x40, 0x20, 0x1f }, /* V */
	{ 0x3f, 0x40, 0x38, 0x40, 0x3f }, /* W */
	{ 0x63, 0x14, 0x08, 0x14, 0x63 }, /* X */
	{ 0x07, 0x08, 0x70, 0x08, 0x07 }, /* Y */
	{ 0x61, 0x51, 0x49, 0x45, 0x43 }, /* Z */
};

static void glyph_for(char character, uint8_t glyph[GLYPH_WIDTH])
{
	static const uint8_t blank[GLYPH_WIDTH] = { 0, 0, 0, 0, 0 };
	const uint8_t *source = blank;

	if (character >= '0' && character <= '9')
		source = digit_glyphs[character - '0'];
	else if (character >= 'A' && character <= 'Z')
		source = alpha_glyphs[character - 'A'];
	else {
		switch (character) {
		case ':': {
			static const uint8_t value[5] = { 0, 0x36, 0x36, 0, 0 };
			source = value;
			break;
		}
		case '.': {
			static const uint8_t value[5] = { 0, 0x60, 0x60, 0, 0 };
			source = value;
			break;
		}
		case ',': {
			static const uint8_t value[5] = { 0, 0x80, 0x60, 0, 0 };
			source = value;
			break;
		}
		case '%': {
			static const uint8_t value[5] = { 0x63, 0x13, 0x08, 0x64, 0x63 };
			source = value;
			break;
		}
		case '-': {
			static const uint8_t value[5] = { 0x08, 0x08, 0x08, 0x08, 0x08 };
			source = value;
			break;
		}
		case '/': {
			static const uint8_t value[5] = { 0x20, 0x10, 0x08, 0x04, 0x02 };
			source = value;
			break;
		}
		default:
			break;
		}
	}

	memcpy(glyph, source, GLYPH_WIDTH);
}

static void set_pixel(uint8_t *tile_pixels, uint32_t logical_x, uint32_t row,
		      uint8_t high_byte, uint8_t low_byte)
{
	uint32_t source_x = CAMERA_FRAME_WIDTH - 1U - logical_x;
	uint32_t pixel = (row * CAMERA_FRAME_WIDTH + source_x) *
			 CAMERA_BYTES_PER_PIXEL;

	tile_pixels[pixel] = high_byte;
	tile_pixels[pixel + 1U] = low_byte;
}

void camera_overlay_line(uint8_t *tile_pixels, const char *text)
{
	uint32_t character_x = 1;
	uint32_t index;

	/* Every overlay line owns one complete eight-row camera tile. */
	memset(tile_pixels, OVERLAY_BG_BYTE, CAMERA_TILE_BYTES);

	for (index = 0; text[index] &&
	     character_x + GLYPH_WIDTH < CAMERA_FRAME_WIDTH; index++) {
		uint8_t glyph[GLYPH_WIDTH];
		uint32_t column;

		glyph_for(text[index], glyph);
		for (column = 0; column < GLYPH_WIDTH; column++) {
			uint32_t row;

			for (row = 0; row < GLYPH_HEIGHT; row++) {
				if (!(glyph[column] & (1U << row)))
					continue;
				/*
				 * The MSDK camera-to-TFT helper consumes every RGB565 row
				 * from its last pixel to its first. Pre-mirror generated
				 * glyphs so the physical display reads left-to-right while
				 * leaving the camera stream's established orientation alone.
				 */
				set_pixel(tile_pixels, character_x + column, row,
					  OVERLAY_FG_BYTE, OVERLAY_FG_BYTE);
			}
		}
		character_x += GLYPH_ADVANCE;
	}
}

void camera_overlay_color_key(uint8_t *tile_pixels)
{
	/* RGB565 high-byte/low-byte pairs, in displayed R/G/B/W order. */
	static const uint8_t colors[4][2] = {
		{ 0xf8, 0x00 },
		{ 0x07, 0xe0 },
		{ 0x00, 0x1f },
		{ 0xff, 0xff },
	};
	uint32_t color;

	for (color = 0; color < 4U; color++) {
		uint32_t column;

		for (column = 0; column < COLOR_KEY_WIDTH; column++) {
			uint32_t row;

			for (row = 0; row < GLYPH_HEIGHT; row++)
				set_pixel(tile_pixels,
					  COLOR_KEY_X + color * COLOR_KEY_WIDTH + column,
					  row, colors[color][0], colors[color][1]);
		}
	}
}
