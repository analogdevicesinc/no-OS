/***************************************************************************//**
 *   @file   util.h
 *   @brief  Implementation of utility functions.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#ifndef UTIL_H_
#define UTIL_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define BIT(x)	(1 << (x))

#define ARRAY_SIZE(x) \
	(sizeof(x) / sizeof((x)[0]))

#define DIV_ROUND_UP(x,y) \
	(((x) + (y) - 1) / (y))
#define DIV_ROUND_CLOSEST(x, y) \
	(((x) + (y) / 2) / (y))
#define DIV_ROUND_CLOSEST_ULL(x, y) \
	DIV_ROUND_CLOSEST(x, y)

#define min(x, y) \
	(((x) < (y)) ? (x) : (y))
#define min_t(type, x, y) \
	(type)min((type)(x), (type)(y))

#define max(x, y) \
	(((x) > (y)) ? (x) : (y))
#define max_t(type, x, y) \
	(type)max((type)(x), (type)(y))

#define clamp(val, min_val, max_val) \
	(max(min((val), (max_val)), (min_val)))
#define clamp_t(type, val, min_val, max_val) \
	(type)clamp((type)(val), (type)(min_val), (type)(max_val))

#define abs(x) \
	((x) < 0 ? (-(x)) : (x))

#define swap(x, y) \
	{typeof(x) _tmp_ = (x); (x) = (y); (y) = _tmp_;}

#define round_up(x,y) \
		(((x)+(y)-1)/(y))

#define BITS_PER_LONG 32

#define GENMASK(h, l) ({ 					\
		uint32_t t = (uint32_t)(~0UL);			\
		t = t << (BITS_PER_LONG - (h - l + 1));		\
		t = t >> (BITS_PER_LONG - (h + 1));		\
		t;						\
})

#define bswap_constant_32(x) \
	((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#define U16_MAX		((uint16_t)~0U)
#define S16_MAX		((int16_t)(U16_MAX>>1))

#define DIV_U64(x, y) (x / y)

#define UNUSED_PARAM(x) ((void)x)

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Find first set bit in word. */
uint32_t find_first_set_bit(uint32_t word);
/* Find last set bit in word. */
uint32_t find_last_set_bit(uint32_t word);
/* Locate the closest element in an array. */
uint32_t find_closest(int32_t val,
		      const int32_t *array,
		      uint32_t size);
/* Shift the value and apply the specified mask. */
uint32_t field_prep(uint32_t mask, uint32_t val);
/* Get a field specified by a mask from a word. */
uint32_t field_get(uint32_t mask, uint32_t word);
/* Log base 2 of the given number. */
int32_t log_base_2(uint32_t x);
/* Find greatest common divisor of the given two numbers. */
uint32_t greatest_common_divisor(uint32_t a,
				 uint32_t b);
/* Calculate best rational approximation for a given fraction. */
void rational_best_approximation(uint32_t given_numerator,
				 uint32_t given_denominator,
				 uint32_t max_numerator,
				 uint32_t max_denominator,
				 uint32_t *best_numerator,
				 uint32_t *best_denominator);
/* Calculate the number of set bits. */
uint32_t hweight8(uint32_t word);
/* Calculate the quotient and the remainder of an integer division. */
uint64_t do_div(uint64_t* n,
		uint64_t base);
/* Unsigned 64bit divide with 64bit divisor and remainder */
uint64_t div64_u64_rem(uint64_t dividend, uint64_t divisor,
		       uint64_t *remainder);
/* Unsigned 64bit divide with 32bit divisor with remainder */
uint64_t div_u64_rem(uint64_t dividend, uint32_t divisor, uint32_t *remainder);
/* Unsigned 64bit divide with 32bit divisor */
uint64_t div_u64(uint64_t dividend, uint32_t divisor);
/* Converts from string to int32_t */
int32_t str_to_int32(const char *str);
/* Converts from string to uint32_t */
uint32_t srt_to_uint32(const char *str);
#endif // UTIL_H_

