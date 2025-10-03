/***************************************************************************//**
 *   @file   main.c
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
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
#include <stddef.h>
#include "common_data.h"
#include "parameters.h"

#include "iio_example.h"

#include "no_os_gpio.h"
#include "no_os_irq.h"

struct no_os_gpio_desc *led_ctrl = NULL;
static struct no_os_irq_ctrl_desc *led_irq_ctrl = NULL;
static struct no_os_timer_desc *timer_desc = NULL;

static int led_heartbeat_init(void)
{
	int err;
	err = no_os_gpio_get(&led_ctrl, &led_ctrl_init_param);
	if (err)
		return err;
	no_os_gpio_direction_output(led_ctrl, NO_OS_GPIO_HIGH);

	err = no_os_irq_ctrl_init(&led_irq_ctrl, &timer_irq_ip);
	if (err)
		goto err_irq_ctrl_init;

	err = no_os_irq_register_callback(led_irq_ctrl,
					  TMR0_IRQn,
					  &heartbeat_callback);
	if (err)
		goto err_irq_register_callback;

	err = no_os_timer_init(&timer_desc, &timer_init_param);
	if (err)
		goto err_timer_init;

	err = no_os_timer_start(timer_desc);
	if (err)
		goto err_timer_start;

	err = no_os_irq_enable(led_irq_ctrl, TMR0_IRQn);
	if (err)
		goto err_irq_enable;

	return 0;

err_irq_enable:
	no_os_timer_stop(timer_desc);
err_timer_start:
	no_os_timer_remove(timer_desc);
err_timer_init:
	no_os_irq_unregister_callback(led_irq_ctrl,
				      TMR0_IRQn,
				      &heartbeat_callback);
err_irq_register_callback:
	no_os_irq_ctrl_remove(led_irq_ctrl);
err_irq_ctrl_init:
	no_os_gpio_remove(led_ctrl);
	return err;
}

static void led_heartbeat_fini(void)
{
	no_os_timer_stop(timer_desc);
	no_os_timer_remove(timer_desc);
	no_os_irq_unregister_callback(led_irq_ctrl,
				      TMR0_IRQn,
				      &heartbeat_callback);
	no_os_irq_ctrl_remove(led_irq_ctrl);
	no_os_gpio_remove(led_ctrl);
	return;
}

int main(void)
{
	int err;
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_desc_param = {
		.platform_ops = &max_irq_ops,
	};

	err = no_os_irq_ctrl_init(&nvic_desc, &nvic_desc_param);
	if (err)
		return err;

	err = no_os_irq_enable(nvic_desc, GPIO0_IRQn);
	if (err)
		goto err_irq_enable;

	err = led_heartbeat_init();
	if (err)
		goto err_led_heartbeat;

	iio_example_main();

	led_heartbeat_fini();

err_led_heartbeat:
	no_os_irq_disable(nvic_desc, GPIO0_IRQn);

err_irq_enable:
	no_os_irq_ctrl_remove(nvic_desc);
	return err;
}
