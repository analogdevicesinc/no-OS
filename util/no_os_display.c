/*******************************************************************************
 *   @file   util/no_os_display.c
 *   @brief  Implementation of no-OS display related utilitary functions
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

	for (int i = 0; i < dst_stride; i++) {
		for (int j = 0; j < width; j++) {
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
