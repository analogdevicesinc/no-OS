/***************************************************************************//**
 *   @file   rcd.c
 *   @brief  RCD implementation file.
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
#include "rcd.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/********************************** Variables *********************************/
/******************************************************************************/
// Flag indicating RCD fired
static volatile int flag_rcd;

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/
/**
 * @brief RCDADC callback function
 * @param context - context variable
 * @return none
 */
static void rcdac_fn(void *context)
{
	flag_rcd++;
}

/**
 * @brief Get RCD flag value
 *
 * @return flag value
 */
int get_rcd_flag_state(void)
{
	return flag_rcd;
}

/**
 * @brief Reset RCD flag value
 *
 * @return none
 */
void reset_rcd_flag_state(void)
{
	flag_rcd = 0;
}

/**
 * @brief Initialize pilot phase
 * @param irq_desc - irq ctrl descriptor
 * @param rcm_test - RCM test pin descriptor
 * @param rcddc - RCDDC pin descriptor
 * @param rcdac - RCDAC pin descriptor
 * @return 0 in case of success, error code otherwise
 */
int rcd_init(struct no_os_irq_ctrl_desc *irq_desc,
	     struct no_os_gpio_desc **rcm_test, struct no_os_gpio_desc **rcddc,
	     struct no_os_gpio_desc **rcdac)
{
	int ret;

	struct no_os_callback_desc rcdac_cb = {
		/** Callback to be called when the event occurs. */
		.callback = rcdac_fn,
		/** Parameter to be passed when the callback is called */
		.ctx = NULL,
		/** Event that triggers the calling of the callback. */
		.event = NO_OS_EVT_GPIO,
		/** Interrupt source peripheral specifier. */
		.peripheral = NO_OS_GPIO_IRQ,
		/** Not used in the case of a GPIO IRQ controller */
		.handle = NULL
	};

	/* RCM test PIN */
	ret = no_os_gpio_get_optional(rcm_test, &ade9113_gpio_rcm_test_ip);
	if (ret)
		return ret;

	if (rcm_test) {
		ret = no_os_gpio_direction_output(*rcm_test, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	/* RCDAC pin */
	ret = no_os_gpio_get_optional(rcdac, &ade9113_gpio_rcdac_ip);
	if (ret)
		return ret;

	if (rcdac) {
		ret = no_os_gpio_direction_input(*rcdac);
		if (ret)
			return ret;
	}

	/* RCDAC interrupt */
	ret = no_os_irq_register_callback(irq_desc, GPIO_RCDAC_PIN,  &rcdac_cb);
	if (ret)
		return ret;

	ret = no_os_irq_trigger_level_set(irq_desc, GPIO_RCDAC_PIN,
					  NO_OS_IRQ_EDGE_RISING);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(irq_desc, GPIO_RCDAC_PIN, 1);
	if (ret)
		return ret;

	ret = no_os_irq_enable(irq_desc, GPIO_RCDAC_PIN);
	if (ret)
		return ret;

	/* RCDDC pin */
	ret = no_os_gpio_get_optional(rcddc, &ade9113_gpio_rcddc_ip);
	if (ret)
		return ret;

	if (rcddc) {
		ret = no_os_gpio_direction_input(*rcddc);
		if (ret)
			return ret;
	}

	/* RCDDC interrupt */
	ret = no_os_irq_register_callback(irq_desc, GPIO_RCDDC_PIN,  &rcdac_cb);
	if (ret)
		return ret;

	ret = no_os_irq_trigger_level_set(irq_desc, GPIO_RCDDC_PIN,
					  NO_OS_IRQ_EDGE_RISING);
	if (ret)
		return ret;

	ret = no_os_irq_set_priority(irq_desc, GPIO_RCDDC_PIN, 1);
	if (ret)
		return ret;

	ret = no_os_irq_enable(irq_desc, GPIO_RCDDC_PIN);
	if (ret)
		return ret;

	return 0;
}
