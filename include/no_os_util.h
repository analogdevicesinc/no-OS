/***************************************************************************//**
 *   @file   no_os_util.h
 *   @brief  Header file of utility functions.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#ifndef _NO_OS_UTIL_H_
#define _NO_OS_UTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#define NO_OS_BIT(x)	(1 << (x))

#define NO_OS_BIT_ULL(x) ((uint64_t) 1 << (x))

#define NO_OS_ARRAY_SIZE(x) \
	(sizeof(x) / sizeof((x)[0]))

#define NO_OS_DIV_ROUND_UP(x,y) \
	(((x) + (y) - 1) / (y))
#define NO_OS_DIV_ROUND_CLOSEST(x, y) \
	(((x) + (y) / 2) / (y))
#define NO_OS_DIV_ROUND_CLOSEST_ULL(x, y) \
	NO_OS_DIV_ROUND_CLOSEST(x, y)

#define no_os_min(x, y) \
	(((x) < (y)) ? (x) : (y))
#define no_os_min_t(type, x, y) \
	(type)no_os_min((type)(x), (type)(y))

#define no_os_max(x, y) \
	(((x) > (y)) ? (x) : (y))
#define no_os_max_t(type, x, y) \
	(type)no_os_max((type)(x), (type)(y))

#define no_os_clamp(val, min_val, max_val) \
	(no_os_max(no_os_min((val), (max_val)), (min_val)))
#define no_os_clamp_t(type, val, min_val, max_val) \
	(type)no_os_clamp((type)(val), (type)(min_val), (type)(max_val))

#define no_os_swap(x, y) \
	{typeof(x) _tmp_ = (x); (x) = (y); (y) = _tmp_;}

#define no_os_round_up(x,y) \
		(((x)+(y)-1)/(y))

#define NO_OS_BITS_PER_LONG 32

#define NO_OS_GENMASK(h, l) ({ 					\
		uint32_t t = (uint32_t)(~0UL);			\
		t = t << (NO_OS_BITS_PER_LONG - (h - l + 1));		\
		t = t >> (NO_OS_BITS_PER_LONG - (h + 1));		\
		t;						\
})
#define NO_OS_GENMASK_ULL(h, l) ({						\
		unsigned long long t = (unsigned long long)(~0ULL);	\
		t = t << (64 - (h - l + 1));				\
		t = t >> (64 - (h + 1));				\
		t;							\
})

#define no_os_bswap_constant_32(x) \
	((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#define no_os_bswap_constant_16(x) ((((x) & (uint16_t)0xff00) >> 8) | \
				 (((x) & (uint16_t)0x00ff) << 8))

#define no_os_bit_swap_constant_8(x) \
	((((x) & 0x80) >> 7) | \
	 (((x) & 0x40) >> 5) | \
	 (((x) & 0x20) >> 3) | \
	 (((x) & 0x10) >> 1) | \
	 (((x) & 0x08) << 1) | \
	 (((x) & 0x04) << 3) | \
	 (((x) & 0x02) << 5) | \
	 (((x) & 0x01) << 7))

#define NO_OS_U16_MAX		((uint16_t)~0U)
#define NO_OS_S16_MAX		((int16_t)(NO_OS_U16_MAX>>1))

#define NO_OS_DIV_U64(x, y) (x / y)

#define NO_OS_UNUSED_PARAM(x) ((void)x)

#define no_os_shift_right(x, s) ((x) < 0 ? -(-(x) >> (s)) : (x) >> (s))

#define no_os_align(x, align) (((x) + ((typeof(x))(align) - 1)) & ~((typeof(x))(align) - 1))

#define no_os_bcd2bin(x)	(((x) & 0x0f) + ((x) >> 4) * 10)
#define no_os_bin2bcd(x)	((((x) / 10) << 4) + (x) % 10)

#define NO_OS_CONTAINER_OF(ptr, type, name) ((type *)((char *)(ptr) - offsetof(type, name)))

/* Check if bit set */
inline int no_os_test_bit(int pos, const volatile void * addr)
{
	return (((const int *)addr)[pos / 32] >> pos) & 1UL;
}

/* Find first set bit in word. */
uint32_t no_os_find_first_set_bit(uint32_t word);
uint64_t no_os_find_first_set_bit_u64(uint64_t word);
/* Find last set bit in word. */
uint32_t no_os_find_last_set_bit(uint32_t word);
/* Locate the closest element in an array. */
uint32_t no_os_find_closest(int32_t val,
			    const int32_t *array,
			    uint32_t size);
/* Shift the value and apply the specified mask. */
uint32_t no_os_field_prep(uint32_t mask, uint32_t val);
uint64_t no_os_field_prep_u64(uint64_t mask, uint64_t val);
/* Get a field specified by a mask from a word. */
uint32_t no_os_field_get(uint32_t mask, uint32_t word);
/* Produce the maximum value representable by a field */
uint32_t no_os_field_max(uint32_t mask);
uint64_t no_os_field_max_u64(uint64_t mask);

/* Log base 2 of the given number. */
int32_t no_os_log_base_2(uint32_t x);
/* Find greatest common divisor of the given two numbers. */
uint32_t no_os_greatest_common_divisor(uint32_t a,
				       uint32_t b);
uint64_t no_os_greatest_common_divisor_u64(uint64_t a,
		uint64_t b);
/* Find lowest common multiple of the given two numbers. */
uint32_t no_os_lowest_common_multiple(uint32_t a, uint32_t b);
/* Calculate best rational approximation for a given fraction. */
void no_os_rational_best_approximation(uint32_t given_numerator,
				       uint32_t given_denominator,
				       uint32_t max_numerator,
				       uint32_t max_denominator,
				       uint32_t *best_numerator,
				       uint32_t *best_denominator);
void no_os_rational_best_approximation_u64(uint64_t given_numerator,
		uint64_t given_denominator,
		uint64_t max_numerator,
		uint64_t max_denominator,
		uint64_t *best_numerator,
		uint64_t *best_denominator);
/* Calculate the number of set bits (8-bit size). */
unsigned int no_os_hweight8(uint8_t word);
/* Calculate the number of set bits (16-bit size). */
unsigned int no_os_hweight16(uint16_t word);
/* Calculate the number of set bits (32-bit size). */
unsigned int no_os_hweight32(uint32_t word);
/* Calculate the quotient and the remainder of an integer division. */
uint64_t no_os_do_div(uint64_t* n,
		      uint64_t base);
/* Unsigned 64bit divide with 64bit divisor and remainder */
uint64_t no_os_div64_u64_rem(uint64_t dividend, uint64_t divisor,
			     uint64_t *remainder);
/* Unsigned 64bit divide with 32bit divisor with remainder */
uint64_t no_os_div_u64_rem(uint64_t dividend, uint32_t divisor,
			   uint32_t *remainder);
int64_t no_os_div_s64_rem(int64_t dividend, int32_t divisor,
			  int32_t *remainder);
/* Unsigned 64bit divide with 32bit divisor */
uint64_t no_os_div_u64(uint64_t dividend, uint32_t divisor);
int64_t no_os_div_s64(int64_t dividend, int32_t divisor);
/* Converts from string to int32_t */
int32_t no_os_str_to_int32(const char *str);
/* Converts from string to uint32_t */
uint32_t no_os_str_to_uint32(const char *str);

void no_os_put_unaligned_be16(uint16_t val, uint8_t *buf);
uint16_t no_os_get_unaligned_be16(uint8_t *buf);
void no_os_put_unaligned_le16(uint16_t val, uint8_t *buf);
uint16_t no_os_get_unaligned_le16(uint8_t *buf);
void no_os_put_unaligned_be24(uint32_t val, uint8_t *buf);
uint32_t no_os_get_unaligned_be24(uint8_t *buf);
void no_os_put_unaligned_le24(uint32_t val, uint8_t *buf);
uint32_t no_os_get_unaligned_le24(uint8_t *buf);
void no_os_put_unaligned_be32(uint32_t val, uint8_t *buf);
uint32_t no_os_get_unaligned_be32(uint8_t *buf);
void no_os_put_unaligned_le32(uint32_t val, uint8_t *buf);
uint32_t no_os_get_unaligned_le32(uint8_t *buf);

int16_t no_os_sign_extend16(uint16_t value, int index);
int32_t no_os_sign_extend32(uint32_t value, int index);
uint64_t no_os_mul_u32_u32(uint32_t a, uint32_t b);
uint64_t no_os_mul_u64_u32_shr(uint64_t a, uint32_t mul, unsigned int shift);
uint64_t no_os_mul_u64_u32_div(uint64_t a, uint32_t mul, uint32_t divisor);

bool no_os_is_big_endian(void);
void no_os_memswap64(void *buf, uint32_t bytes, uint32_t step);

#endif // _NO_OS_UTIL_H_
