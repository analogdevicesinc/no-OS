/***************************************************************************//**
 *   @file   interface.c
 *   @brief  User interface implementation file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
