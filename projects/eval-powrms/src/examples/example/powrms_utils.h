/***************************************************************************//**
 *   @file   powrms_utils.h
 *   @brief  Utility functions header for input handling and data conversion in eval-powrms project
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

#ifndef POWRMS_UTILS_H
#define POWRMS_UTILS_H

#include <stdint.h>
#include "common_data.h"

/**
 * @brief Read and decode user input from UART
 *
 * @return uint8_t Input command code:
 *         - 0: No input received
 *         - 1: 'q' - Next/Navigate forward
 *         - 2: 'w' - Back/Increment (context dependent)
 *         - 3: 'e' - Enter/Confirm
 */
uint8_t read_input();

/**
 * @brief Convert float value to string representation
 *
 * @param var Float value to convert
 * @param text_buf Output buffer for string representation
 * @param int_digits Number of integer digits
 * @param float_digits Number of decimal digits
 */
void powrms_float_to_str(float var, char* text_buf, uint8_t int_digits,
			 uint8_t float_digits);

/**
 * @brief Convert integer value to string representation
 *
 * @param var Integer value to convert
 * @param text_buf Output buffer for string representation
 */
void powrms_int_to_str(int var, char* text_buf);

#endif /* POWRMS_UTILS_H */