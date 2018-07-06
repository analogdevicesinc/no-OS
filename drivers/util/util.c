/***************************************************************************//**
 *   @file   util.c
 *   @brief  Implementation of Util.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "util.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Find first set bit in word.
 */
uint32_t find_first_set_bit(uint32_t word)
{
	uint32_t first_set_bit = 0;

	while (word) {
		if (word & 0x1)
			return first_set_bit;
		word >>= 1;
		first_set_bit ++;
	}

	return 32;
}

/**
 * Find last set bit in word.
 */
uint32_t find_last_set_bit(uint32_t word)
{
	uint32_t bit = 0;
	uint32_t last_set_bit = 32;

	while (word) {
		if (word & 0x1)
			last_set_bit = bit;
		word >>= 1;
		bit ++;
	}

	return last_set_bit;
}

/**
 * Log base 2 of the given number.
 */
int32_t log_base_2(uint32_t x)
{
	return find_last_set_bit(x);
}

/**
 * Find greatest common divisor of the given two numbers.
 */
uint32_t greatest_common_divisor(uint32_t a,
				 uint32_t b)
{
	uint32_t div;
	uint32_t common_div;

	for (div = 1; (div <= a) && (div <= b); div++)
		if (!(a % div) && !(b % div))
			common_div = div;

	return common_div;
}

/**
 * Calculate best rational approximation for a given fraction.
 */
void rational_best_approximation(uint32_t given_numerator,
				 uint32_t given_denominator,
				 uint32_t max_numerator,
				 uint32_t max_denominator,
				 uint32_t *best_numerator,
				 uint32_t *best_denominator)
{
	uint32_t gcd;

	gcd = greatest_common_divisor(given_numerator, given_denominator);

	*best_numerator = given_numerator / gcd;
	*best_denominator = given_denominator / gcd;

	if ((*best_numerator > max_numerator) ||
		(*best_denominator > max_denominator)) {
		*best_numerator = 0;
		*best_denominator = 0;
	}
}

