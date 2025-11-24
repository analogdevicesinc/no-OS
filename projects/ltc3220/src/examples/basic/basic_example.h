/***************************************************************************//**
 *   @file   basic_example.h
 *   @brief  Header file for example program of LTC3220 project.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
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

#ifndef __LTC3220_BASIC_EXAMPLE_TESTS__
#define __LTC3220_BASIC_EXAMPLE_TESTS__

#include "ltc3220.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main();

/***************************************************************************//**
 * @brief individual LED control for all 18 LEDs
 * @param ltc3220 - The device structure.
 * @param mode - the mode to set all the LEDs to.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_test_led_singles(struct ltc3220_dev *ltc3220,
			     enum ltc3220_uled_mode mode);

/***************************************************************************//**
 * @brief individual LED control with alternating modes for all 18 LEDs
 * @param ltc3220 - the device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_test_led_singles_alt_modes(struct ltc3220_dev *ltc3220);

/***************************************************************************//**
 * @brief Parallel and Serial LED control.
 * LED 2 to 9 are set to serial, then LED1 is set to display on all LEDs.
 * After some delay LED 9 to 18 will change mode in serial write.
 * @param ltc3220 - the device structure.
 * @param mode - the mode to set the individual LEDs
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_test_led_quick_write_with_indiv(struct ltc3220_dev *ltc3220,
		enum ltc3220_uled_mode mode);

#endif /* __LTC3220_BASIC_EXAMPLE_TESTS__ */
