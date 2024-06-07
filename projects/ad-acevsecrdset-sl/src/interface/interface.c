/***************************************************************************//**
 *   @file   interface.c
 *   @brief  User interface implementation file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
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
#include "no_os_print_log.h"
#include "state_machine.h"
#include "no_os_delay.h"
#include "common_data.h"
#include "interface.h"

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/

/**
 * @brief Interface initialization routines
 * @param gpio_led_desc - poionter to first LED descriptor
 * @return 0 in case of success, error code otherwise
 */
int interface_init(struct no_os_gpio_desc **gpio_led_desc)
{
	int ret = 0;
	struct no_os_gpio_init_param params[LED_NO] = {
		ade9113_gpio_led1_ip,
		ade9113_gpio_led2_ip,
		ade9113_gpio_led3_ip,
		ade9113_gpio_led4_ip
	};

	for (unsigned int i = 0; i < LED_NO; i++) {
		ret = no_os_gpio_get_optional(&gpio_led_desc[i], &params[i]);
		if (ret)
			return ret;

		if (gpio_led_desc[0]) {
			ret = no_os_gpio_direction_output(gpio_led_desc[i], NO_OS_GPIO_LOW);
			if (ret)
				return ret;
		}
	}

	return ret;
}

/**
 * @brief Turn LED on
 * @param gpio_led_desc - LED descriptor
 * @return 0 in case of success, error code otherwise
 */
int interface_led_on(struct no_os_gpio_desc *gpio_led_desc)
{
	return no_os_gpio_set_value(gpio_led_desc, 1);
}

/**
 * @brief Turn LED off
 * @param gpio_led_desc - LED descriptor
 * @return 0 in case of success, error code otherwise
 */
int interface_led_off(struct no_os_gpio_desc *gpio_led_desc)
{
	return no_os_gpio_set_value(gpio_led_desc, 0);
}

/**
 * @brief Toggle LED
 * @param gpio_led_desc - LED descriptor
 * @return 0 in case of success, error code otherwise
 */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc)
{
	uint8_t val;
	int ret;

	ret = no_os_gpio_get_value(gpio_led_desc, &val);
	if (ret)
		return ret;

	if (val == NO_OS_GPIO_LOW)
		val = NO_OS_GPIO_HIGH;
	else
		val = NO_OS_GPIO_LOW;

	return no_os_gpio_set_value(gpio_led_desc, val);
}

/**
 * @brief Display error code
 * @param stout - state machine descriptor
 * @return 0 in case of success, error code otherwise
 */
int interface_disp(struct stout *stout)
{
	if (stout->current_state == STATE_FAULT) {
		if ((stout->err_status == INTF_INPUT_V_ERR_U)
		    || (stout->err_status == INTF_INPUT_V_ERR_O)) {
			interface_led_on(stout->gpio_led[0]);
			interface_led_off(stout->gpio_led[1]);
			interface_led_off(stout->gpio_led[2]);
			interface_toggle_led(stout->gpio_led[3]);
		} else if (stout->err_status == INTF_INIT_RCD_ERROR) {
			interface_toggle_led(stout->gpio_led[0]);
			interface_toggle_led(stout->gpio_led[1]);
			interface_toggle_led(stout->gpio_led[2]);
			interface_toggle_led(stout->gpio_led[3]);
		} else if (stout->err_status == INTF_RCD_ERROR) {
			interface_led_off(stout->gpio_led[0]);
			interface_toggle_led(stout->gpio_led[1]);
			interface_toggle_led(stout->gpio_led[2]);
			interface_led_off(stout->gpio_led[3]);
		} else if (stout->err_status == INTF_PILOT_ERROR) {
			interface_led_off(stout->gpio_led[0]);
			interface_toggle_led(stout->gpio_led[1]);
			interface_toggle_led(stout->gpio_led[2]);
			interface_toggle_led(stout->gpio_led[3]);
		} else if (stout->err_status == INTF_DIODE_ERR) {
			interface_toggle_led(stout->gpio_led[0]);
			interface_led_off(stout->gpio_led[1]);
			interface_led_off(stout->gpio_led[2]);
			interface_led_off(stout->gpio_led[3]);
		} else if (stout->err_status == INTF_RELAY_ERR) {
			interface_toggle_led(stout->gpio_led[0]);
			interface_led_off(stout->gpio_led[1]);
			interface_led_on(stout->gpio_led[2]);
			interface_led_off(stout->gpio_led[3]);
		} else if (stout->err_status == INTF_TEMPERATURE_ERR) {
			interface_toggle_led(stout->gpio_led[0]);
			interface_toggle_led(stout->gpio_led[1]);
			interface_led_off(stout->gpio_led[2]);
			interface_led_off(stout->gpio_led[3]);
#if defined(REV_D)
		} else if (stout->err_status == INTF_PE_UPSTREAM_ERR) {
			interface_led_off(stout->gpio_led[0]);
			interface_toggle_led(stout->gpio_led[1]);
			interface_led_off(stout->gpio_led[2]);
			interface_led_on(stout->gpio_led[3]);
#endif
		} else {
			interface_led_on(stout->gpio_led[0]);
			interface_led_off(stout->gpio_led[1]);
			interface_led_off(stout->gpio_led[2]);
			interface_led_on(stout->gpio_led[3]);
		}
	} else if ((stout->current_state == STATE_C)
		   || (stout->current_state == STATE_D)) {
		interface_led_on(stout->gpio_led[0]);
		interface_led_off(stout->gpio_led[1]);
		interface_toggle_led(stout->gpio_led[2]);
		interface_led_off(stout->gpio_led[3]);
	} else if (stout-> current_state == STATE_B) {
		interface_led_on(stout->gpio_led[0]);
		interface_led_on(stout->gpio_led[1]);
		interface_led_off(stout->gpio_led[2]);
		interface_led_off(stout->gpio_led[3]);
	} else {
		interface_led_on(stout->gpio_led[0]);
		interface_led_off(stout->gpio_led[1]);
		interface_led_off(stout->gpio_led[2]);
		interface_led_off(stout->gpio_led[3]);
	}

	return 0;
}

/**
 * @brief Get interface blink time (multimple of 20ms)
 * @param stout - state machine descriptor
 * @return 0 in case of success, error code otherwise
 */
int interface_blink_time(struct stout *stout)
{
	if (stout->current_state == STATE_C)
		return 5;
	else
		return 2;
}
