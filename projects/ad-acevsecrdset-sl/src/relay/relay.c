/***************************************************************************//**
 *   @file   relay.c
 *   @brief  Relay implementation file.
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
