/*******************************************************************************
 *   @file   util/no_os_display.c
 *   @brief  Implementation of no-OS display related utilitary functions
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "no_os_display.h"
#include "errno.h"

int32_t no_os_row_major_to_column_major_8bits(uint8_t *src, uint8_t *dst,
		int width, int height)
{
	uint8_t tmp_var;
	uint32_t src_stride = width /
			      8; // In source buffer bits packed in butes on horizontal axes
	uint32_t dst_stride = height /
			      8; // In destination buffer bits packed in butes on vertical axes

	// Handle error cases
	if (!src || !dst)
		return -ENOMEM;
	if (!width || !height)
		return -1;
	if (width % 8 != 0 || height % 8 != 0)
		return -1;

	for (uint32_t i = 0; i < dst_stride; i++) {
		for (uint32_t j = 0; j < (uint32_t)width; j++) {
			tmp_var = 0;
			int byte_index = j / 8;
			int bit_index = 7 - (j % 8);

			// Pack 8 vertical bits into one byte
			for (int k = 0; k < 8; k++) {
				int img_row = (i * 8) + k;
				uint8_t pixel = ((src[img_row * (src_stride) + byte_index] >> bit_index) &
						 0x01);
				tmp_var |= (pixel << k);
			}
			dst[i * width + j] = tmp_var;
		}
	}
	return 0;
}
