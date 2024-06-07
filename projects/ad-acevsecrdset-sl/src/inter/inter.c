/***************************************************************************//**
 *   @file   inter.c
 *   @brief  Interrupt on port 2 implementation file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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
#include "inter.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/********************************** Variables *********************************/
/******************************************************************************/
uint8_t gpio_flag;

extern struct no_os_irq_ctrl_desc *stout_nvic_desc;

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/

/**
 * @brief callback function
 * @param context - context variable
 * @return none
 */
static void gpio_2_5_cb_fn(void *context)
{
	printf("Test\n");
}

/**
 * @brief Get GPIO flag value
 *
 * @return flag value
 */
int get_gpio_flag_state(void)
{
	return gpio_flag;
}


/**
 * @brief Reset GPIO low flag value
 *
 * @return none
 */
void reset_gpio_low_flag_state(void)
{
	gpio_flag = 0;
}

/**
 * @brief Initialize inter phase
 *
 * @return 0 in case of success, error code otherwise
 */
int inter_init(struct stout *stout)
{
	struct no_os_irq_ctrl_desc *irq_desc1;
	struct no_os_gpio_desc *pin;
	int ret;

	/* Setup GPIO interrupts */
	struct no_os_callback_desc p2_cb = {
		/** Callback to be called when the event occurs. */
		.callback = gpio_2_5_cb_fn,
		/** Parameter to be passed when the callback is called */
		.ctx = NULL,
		/** Event that triggers the calling of the callback. */
		.event = NO_OS_EVT_GPIO,
		/** Interrupt source peripheral specifier. */
		.peripheral = NO_OS_GPIO_IRQ,
		/** Not used in the case of a GPIO IRQ controller */
		.handle = NULL
	};

	/* Port 2 pin */
	ret = no_os_gpio_get(&pin, &gpio_ip);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	ret = no_os_gpio_direction_input(pin);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	/* Initialize GPIO IRQ controller in order to be able to enable GPIO IRQ interrupt */
	struct no_os_irq_init_param irq_gpio_ip = {
		.irq_ctrl_id = INT_PORT,
		.platform_ops = &max_gpio_irq_ops,
	};

	ret = no_os_irq_ctrl_init(&irq_desc1, &irq_gpio_ip);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	/* RCDAC interrupt */
	ret = no_os_irq_register_callback(irq_desc1, INT_PIN,  &p2_cb);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	ret = no_os_irq_trigger_level_set(irq_desc1, INT_PIN,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	ret = no_os_irq_set_priority(irq_desc1, INT_PIN, 1);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	MXC_NVIC_SetVector(GPIO2_IRQn, gpio_2_5_cb_fn);

	ret = no_os_irq_enable(irq_desc1, INT_PIN);
	if (ret) {
		printf("ERROR\n");
		return ret;
	}

	return 0;
}
