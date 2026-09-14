/***************************************************************************//**
 *   @file   main.c
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#include "example.h"
#include "no_os_irq.h"
#include "no_os_timer.h"
#include "maxim_irq.h"
#include "common_data.h"
#if (TARGET_NUM == 32655)
#include "tmr.h"
#endif

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

#if (TARGET_NUM == 32655)
	/* max32655 timers require explicit interrupt enable (IE_A bit in ctrl1);
	   max32665 has no such gate and no MXC_TMR_EnableInt API. */
	MXC_TMR_EnableInt(MXC_TMR0);
#endif

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
