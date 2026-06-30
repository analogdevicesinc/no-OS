/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  exmaple program for Maxim platfom of LTC3220 project.
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

#include "common_data.h"
#include "ltc3220.h"
#include "no_os_delay.h"
#include "basic_example.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct ltc3220_dev *ltc3220;
	int ret;

	ret = ltc3220_init(&ltc3220, ltc3220_user_init);
	if (ret)
		goto error;

	while (true) {
		/* reset operations */
		ret = ltc3220_reset(ltc3220);
		if (ret)
			goto error_ltc3220;

		/* regular light mode */
		ltc3220_test_led_singles(ltc3220, LTC3220_MODE_NORMAL);

		no_os_mdelay(3000);

		ret = ltc3220_reset(ltc3220);
		if (ret)
			goto error_ltc3220;

		/* blinking (fast) */
		ltc3220_set_blink_fast(ltc3220, true);
		ltc3220_set_blink_long(ltc3220, false);
		ltc3220_test_led_singles(ltc3220, LTC3220_MODE_BLINK);

		no_os_mdelay(3000);

		ret = ltc3220_reset(ltc3220);
		if (ret)
			goto error_ltc3220;

		/* blinking (slow) */
		ltc3220_set_blink_fast(ltc3220, false);
		ltc3220_set_blink_long(ltc3220, true);
		ltc3220_test_led_singles(ltc3220, LTC3220_MODE_BLINK);

		no_os_mdelay(3000);

		ret = ltc3220_reset(ltc3220);
		if (ret)
			goto error_ltc3220;

		/* check if voltage output is ~4.6v */
		ltc3220_set_cpo_1p5x(ltc3220, true);


		/* gradation (increasing) */
		ltc3220_set_grad_increasing(ltc3220, true);
		ltc3220_set_grad_speed(ltc3220, LTC3220_GRAD_MAX_SPD);
		ltc3220_test_led_singles(ltc3220, LTC3220_MODE_GRADATION);

		no_os_mdelay(3000);

		ret = ltc3220_reset(ltc3220);
		if (ret)
			goto error_ltc3220;

		/* gradation (decreasing) */
		ltc3220_set_grad_increasing(ltc3220, false);
		ltc3220_set_grad_speed(ltc3220, LTC3220_GRAD_MAX_SPD);
		ltc3220_test_led_singles(ltc3220, LTC3220_MODE_GRADATION);

		no_os_mdelay(3000);

		ret = ltc3220_reset(ltc3220);
		if (ret)
			goto error_ltc3220;

		/* gradation (disabled) = normal mode */
		ltc3220_set_grad_speed(ltc3220, 0);
		ltc3220_test_led_singles(ltc3220, LTC3220_MODE_GRADATION);

		no_os_mdelay(3000);

		ret = ltc3220_reset(ltc3220);
		if (ret)
			goto error_ltc3220;

		/* using different modes on the device */
		ltc3220_set_blink_fast(ltc3220, true);
		ltc3220_set_grad_speed(ltc3220, LTC3220_GRAD_MAX_SPD);
		ltc3220_test_led_singles_alt_modes(ltc3220);

		no_os_mdelay(3000);

		ret = ltc3220_reset(ltc3220);
		if (ret)
			goto error_ltc3220;

		/* check if voltage output is ~5.1v */
		ltc3220_set_cpo_2x(ltc3220, true);

		/* use parallel and single commands on LEDs */
		ltc3220_set_blink_fast(ltc3220, true);
		ltc3220_set_blink_long(ltc3220, true);
		ltc3220_test_led_quick_write_with_indiv(ltc3220, LTC3220_MODE_BLINK);

		no_os_mdelay(3000);

		ret = ltc3220_reset(ltc3220);
		if (ret)
			goto error_ltc3220;

		/* set alternating and shutdown, restore after 5s */
		ltc3220_set_cpo_2x(ltc3220, true);
		ltc3220_set_blink_fast(ltc3220, true);
		ltc3220_set_grad_speed(ltc3220, LTC3220_GRAD_MAX_SPD);
		ltc3220_test_led_singles_alt_modes(ltc3220);
		ltc3220_set_shutdown(ltc3220, true);

		no_os_mdelay(5000);

		ltc3220_set_shutdown(ltc3220, false);

		no_os_mdelay(5000);
	}

error_ltc3220:
	ltc3220_remove(ltc3220);

error:
	return -1;
}

/***************************************************************************//**
 * @brief individual LED control for all 18 LEDs
 * @param ltc3220 - The device structure.
 * @param mode - the mode to set all the LEDs to.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_test_led_singles(struct ltc3220_dev *ltc3220,
			     enum ltc3220_uled_mode mode)
{
	uint8_t power = 1;
	int ret;
	ret = ltc3220_set_quick_write(ltc3220, false);
	if (ret)
		return ret;
	for (int i = LTC3220_REG_START_ULED;
	     i <= LTC3220_REG_END_ULED;
	     i++) {
		ret = ltc3220_set_uled_mode(ltc3220, i, mode);
		if (ret)
			return ret;
		ltc3220_set_uled_current(ltc3220, i, power * i);
		if (ret)
			return ret;
		no_os_mdelay(1500);
	}

	return 0;
}

/***************************************************************************//**
 * @brief individual LED control with alternating modes for all 18 LEDs
 * @param ltc3220 - the device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_test_led_singles_alt_modes(struct ltc3220_dev *ltc3220)
{
	uint8_t power = 1;
	int ret;
	ret = ltc3220_set_quick_write(ltc3220, false);
	if (ret)
		return ret;
	for (int i = LTC3220_REG_START_ULED;
	     i <= LTC3220_REG_END_ULED;
	     i++) {
		/* cycle between the 4 modes of operation*/
		ret = ltc3220_set_uled_mode(ltc3220, i, i % 4);
		if (ret)
			return ret;
		ltc3220_set_uled_current(ltc3220, i, power * i);
		if (ret)
			return ret;
		no_os_mdelay(1500);
	}

	return 0;
}

/***************************************************************************//**
 * @brief Parallel and Serial LED control.
 * LED 2 to 9 are set to serial, then LED1 is set to display on all LEDs.
 * After some delay LED 9 to 18 will change mode in serial write.
 * @param ltc3220 - the device structure.
 * @param mode - the mode to set the individual LEDs
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_test_led_quick_write_with_indiv(struct ltc3220_dev *ltc3220,
		enum ltc3220_uled_mode mode)
{
	uint8_t power = 1;
	int ret;
	ret = ltc3220_set_quick_write(ltc3220, true);
	if (ret)
		return ret;

	for (int i = LTC3220_REG_START_ULED + 1;
	     i <= LTC3220_REG_END_ULED / 2;
	     i++) {
		ret = ltc3220_set_uled_mode(ltc3220, i, mode);
		if (ret)
			return ret;
		ret = ltc3220_set_uled_current(ltc3220, i, power * i);
		if (ret)
			return ret;

		no_os_mdelay(1500);
	}

	ret = ltc3220_set_uled_mode(ltc3220, 1, LTC3220_MODE_NORMAL);
	ret = ltc3220_set_uled_current(ltc3220, 1, 1);
	if (ret)
		return ret;

	no_os_mdelay(3000);

	for (int i = LTC3220_REG_END_ULED / 2;
	     i <= LTC3220_REG_END_ULED;
	     i++) {
		ret = ltc3220_set_uled_mode(ltc3220, i, mode);
		if (ret)
			return ret;
		ret = ltc3220_set_uled_current(ltc3220, i, power * i);
		if (ret)
			return ret;

		no_os_mdelay(1500);
	}

	ret = ltc3220_set_quick_write(ltc3220, false);
	if (ret)
		return ret;

	ret = ltc3220_set_uled_current(ltc3220, LTC3220_REG_START_ULED, 0);
	if (ret)
		return ret;

	no_os_mdelay(3000);

	return 0;
}
