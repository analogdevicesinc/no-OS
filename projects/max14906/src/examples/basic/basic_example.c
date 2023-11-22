/***************************************************************************//**
 *   @file   max14906/src/examples/basic/basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "basic_example.h"
#include "common_data.h"
#include "max149x6-base.h"
#include "max14906.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"

/** Defining specific mask for this example. */
#define MAX14906_SLED_CH(x)	NO_OS_BIT(x + MAX14906_CHANNELS)

static const char *climit_name[] = {
	"600mA", "130mA", "300mA", "1.2A",
};

void gpio_callback_fn(void *ctx)
{
	struct max149x6_desc *desc = ctx;
	int ret;
	uint32_t val;

	ret = max149x6_reg_read(desc, MAX14906_INT_REG, &val);
	if (val)
		pr_info("Fault detected!\n");
}

int basic_example_main()
{
	int ret, i;
	uint32_t val;
	struct max149x6_desc *max14906_desc;

	enum max14906_climit climit = MAX14906_CL_300;
	enum max14906_climit climit2 = MAX14906_CL_600;

	struct no_os_callback_desc gpio_cb = {
		.callback = gpio_callback_fn,
		/** Parameter to be passed when the callback is called. */
		.ctx = max14906_desc,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = NULL
	};

	/** GPIO Pin Interrupt Controller */
	struct no_os_irq_ctrl_desc *gpio_irq_desc;
	struct no_os_irq_init_param gpio_irq_desc_param = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA
	};

	/** MAX14906 Initialization */
	ret = max14906_init(&max14906_desc, &max14906_ip);
	if (ret)
		goto exit;

	ret = no_os_irq_ctrl_init(&gpio_irq_desc, &gpio_irq_desc_param);
	if (ret)
		goto max14906_remove;

	ret = no_os_irq_register_callback(gpio_irq_desc, GPIO_FAULT_PIN_NUM,
					  &gpio_cb);
	if (ret)
		goto remove_gpio_irq_ctrl;

	ret = no_os_irq_trigger_level_set(gpio_irq_desc, GPIO_FAULT_PIN_NUM,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto gpio_irq_unregister_callback;

	ret = no_os_irq_set_priority(gpio_irq_desc, GPIO_FAULT_PIN_NUM, 1);
	if (ret)
		goto gpio_irq_unregister_callback;

	ret = no_os_irq_enable(gpio_irq_desc, GPIO_FAULT_PIN_NUM);
	if (ret)
		goto gpio_irq_unregister_callback;

	/** Setting SLED set bit 1 in the config register. */
	ret = max149x6_reg_update(max14906_desc, MAX14906_CONFIG1_REG,
				  MAX14906_SLED_MASK, no_os_field_prep(MAX14906_SLED_MASK, 1));
	if (ret)
		goto gpio_irq_unregister_callback;

	/** Turning the Status LEDs on, then off. */
	for (i = 0; i <  MAX14906_CHANNELS; i++) {
		ret = max149x6_reg_update(max14906_desc, MAX14906_SETLED_REG,
					  MAX14906_SLED_CH(i), no_os_field_prep(MAX14906_SLED_CH(i), 1));
		if (ret)
			goto gpio_irq_unregister_callback;

		no_os_mdelay(500);

		ret = max149x6_reg_update(max14906_desc, MAX14906_SETLED_REG,
					  MAX14906_SLED_CH(i), no_os_field_prep(MAX14906_SLED_CH(i), 0));
		if (ret)
			goto gpio_irq_unregister_callback;

		no_os_mdelay(500);
	}

	/** Setting a current limit for channel 0. */
	ret = max14906_climit_set(max14906_desc, 0, climit);
	if (ret)
		goto gpio_irq_unregister_callback;

	/** Read current limit for all channels. */
	for (i = 0; i < MAX14906_CHANNELS; i++) {
		ret = max14906_climit_get(max14906_desc, i, &climit2);
		if (ret)
			goto gpio_irq_unregister_callback;
		pr_info("Current limit for channel %d is %s\n", i,
			climit_name[climit2]);
	}

	/** Setting the on state for channel 0, and then verifying it. */
	if (max14906_desc->en_gpio) {
		ret = max14906_ch_set(max14906_desc, 0, 1);
		if (ret)
			goto gpio_irq_unregister_callback;

		ret = max14906_ch_get(max14906_desc, 0, &val);
		if (ret)
			goto gpio_irq_unregister_callback;
	}

	while (1);

gpio_irq_unregister_callback:
	no_os_irq_unregister_callback(gpio_irq_desc, GPIO_FAULT_PIN_NUM, &gpio_cb);
remove_gpio_irq_ctrl:
	no_os_irq_ctrl_remove(gpio_irq_desc);
max14906_remove:
	max14906_remove(max14906_desc);
exit:
	if (ret)
		pr_info("Error!\n");
	return ret;
}
