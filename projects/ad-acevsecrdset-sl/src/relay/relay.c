/***************************************************************************//**
 *   @file   relay.c
 *   @brief  Relay implementation file.
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
#include "common_data.h"
#include "relay.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/********************************** Variables *********************************/
/******************************************************************************/
// Flag indicating relay status (updated depending on state machine actions)
static uint8_t relay_status;

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/
/**
 * @brief Initialize relay component
 * @param relay - relay GPIO pin description
 * @return 0 in case of success, error code otherwise
 */
int relay_init(struct no_os_gpio_desc **relay)
{
	int ret;

	ret = no_os_gpio_get_optional(relay, &ade9113_gpio_relay_ip);
	if (ret)
		return ret;

	if (relay) {
		ret = no_os_gpio_direction_output(*relay, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Open relay command
 * @param relay - relay GPIO pin description
 * @return 0 in case of success, error code otherwise
 */
int relay_open(struct no_os_gpio_desc *relay)
{
	return  no_os_gpio_direction_output(relay, NO_OS_GPIO_LOW);
}

/**
 * @brief Close relay command
 * @param relay - relay GPIO pin description
 * @return 0 in case of success, error code otherwise
 */
int relay_close(struct no_os_gpio_desc *relay)
{
	return  no_os_gpio_direction_output(relay, NO_OS_GPIO_HIGH);
}

/**
 * @brief Relay command line state
 * @param relay - relay GPIO pin description
 * @param state - relay state (0 - OPEN, 1 - CLOSED)
 * @return 0 in case of success, error code otherwise
 */
int relay_state(struct no_os_gpio_desc *relay, uint8_t *state)
{
	return  no_os_gpio_get_value(relay, state);
}

/**
 * @brief Get relay status
 *
 * @return 1 - RELAY CLOSED, 0 - RELAY OPENED
 */
uint8_t get_relay_status(void)
{
	return relay_status;
}

/**
 * @brief Set relay status to CLOSED - 1
 *
 * @return none
 */
void set_relay_status(void)
{
	relay_status = 1;
}

/**
 * @brief Set relay status to OPENED - 0
 *
 * @return none
 */
void reset_relay_status(void)
{
	relay_status = 0;
}
