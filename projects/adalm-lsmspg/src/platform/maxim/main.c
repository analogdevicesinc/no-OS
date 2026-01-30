/***************************************************************************//**
 *   @file   main.c
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ******************************************************************************/
#include "example.h"
#include "no_os_irq.h"
#include "no_os_timer.h"
#include "maxim_irq.h"
#include "common_data.h"

static struct no_os_irq_ctrl_desc *timer_irq_desc;
static struct no_os_callback_desc heartbeat_callback;
static struct no_os_timer_desc *timer_desc;

int heartbeat_init(void (*cb)(void *context))
{
	int err;

	err = no_os_irq_ctrl_init(&timer_irq_desc, &timer_irq_ip);
	if (err)
		return err;

	heartbeat_callback.callback = cb;
	heartbeat_callback.event = NO_OS_EVT_TIM_ELAPSED;
	heartbeat_callback.peripheral = NO_OS_TIM_IRQ;
	err = no_os_irq_register_callback(timer_irq_desc, TMR0_IRQn,
					  &heartbeat_callback);
	if (err)
		goto err_irq_ctrl_exit;

	err = no_os_timer_init(&timer_desc, &timer_ip);
	if (err)
		goto err_unregister_callback;

	err = no_os_timer_start(timer_desc);
	if (err)
		goto err_timer_exit;

	err = no_os_irq_enable(timer_irq_desc, TMR0_IRQn);
	if (err)
		goto err_timer_stop;

	return 0;

err_timer_stop:
	no_os_timer_stop(timer_desc);
err_timer_exit:
	no_os_timer_remove(timer_desc);
err_unregister_callback:
	no_os_irq_unregister_callback(timer_irq_desc, TMR0_IRQn, &heartbeat_callback);
err_irq_ctrl_exit:
	no_os_irq_ctrl_remove(timer_irq_desc);

	return err;
}

void heartbeat_exit(void)
{
	no_os_irq_disable(timer_irq_desc, TMR0_IRQn);
	no_os_timer_stop(timer_desc);
	no_os_timer_remove(timer_desc);
	no_os_irq_unregister_callback(timer_irq_desc, TMR0_IRQn, &heartbeat_callback);
	no_os_irq_ctrl_remove(timer_irq_desc);
	return;
}

int main(void)
{
	int err;
	struct no_os_irq_ctrl_desc *nvic_desc;
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	err = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (err)
		return err;

	err = no_os_irq_enable(nvic_desc, GPIO0_IRQn);
	if (err)
		goto err_irq_ctrl_exit;

	err = start_example();

	no_os_irq_disable(nvic_desc, GPIO0_IRQn);
err_irq_ctrl_exit:
	no_os_irq_ctrl_remove(nvic_desc);
	return err;
}
