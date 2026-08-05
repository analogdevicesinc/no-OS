/***************************************************************************//**
 *   @file   basic_example.h
 *   @brief  Header file for example program of LTC3220 project.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
