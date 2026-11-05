/***************************************************************************//**
 *   @file   maxim_memcpy.c
 *   @brief  Alignment-safe memcpy() for the MAX32657.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stddef.h>
#include <stdint.h>

typedef uint32_t __attribute__((may_alias)) maxim_word_t;

/**
 * @brief Copy memory without unaligned word accesses.
 *
 * The MAX32657 is built with -mno-unaligned-access, so SystemInit() sets
 * SCB->CCR.UNALIGN_TRP and any unaligned word access faults. newlib's memcpy()
 * uses unaligned word accesses when src and dst are misaligned relative to
 * each other. This definition overrides it: word copies are used only when
 * both pointers can be aligned together, bytes otherwise.
 *
 * @param dst - Destination buffer.
 * @param src - Source buffer.
 * @param len - Number of bytes to copy.
 * @return dst
 */
/* Keep GCC from turning these loops back into a memcpy() call. */
__attribute__((optimize("no-tree-loop-distribute-patterns")))
void *memcpy(void *restrict dst, const void *restrict src, size_t len)
{
	uint8_t *d = dst;
	const uint8_t *s = src;
	maxim_word_t *dw;
	const maxim_word_t *sw;

	if ((((uintptr_t)d ^ (uintptr_t)s) & 3U) == 0U) {
		while (len != 0U && ((uintptr_t)d & 3U) != 0U) {
			*d++ = *s++;
			len--;
		}

		dw = (maxim_word_t *)d;
		sw = (const maxim_word_t *)s;
		while (len >= 16U) {
			dw[0] = sw[0];
			dw[1] = sw[1];
			dw[2] = sw[2];
			dw[3] = sw[3];
			dw += 4;
			sw += 4;
			len -= 16U;
		}
		while (len >= 4U) {
			*dw++ = *sw++;
			len -= 4U;
		}
		d = (uint8_t *)dw;
		s = (const uint8_t *)sw;
	}

	while (len != 0U) {
		*d++ = *s++;
		len--;
	}

	return dst;
}
