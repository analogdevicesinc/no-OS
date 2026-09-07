/***************************************************************************//**
 * @file main.c
 * @brief Main file for Maxim (MSDK) platform of capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <errno.h>
#include "parameters.h"
#include "common_data.h"
#include "capi_irq.h"

extern int example_main(void);

/*
 * ===== GPIO loopback IRQ hooks (test_irq) =====
 *
 * Only the loopback example wires these, and only when the board maps both a
 * GPIO output pair and an interrupt controller. test_irq.c gates its whole IRQ
 * suite on exactly this pair, so match it here — the basic variant never pulls
 * these in.
 *
 * The CAPI GPIOn_IRQHandler (capi/platform/maxim/maxim_capi_irq.c) runs MSDK's
 * MXC_GPIO_Common_Handler first, which reads-and-CLEARS the port's hardware
 * interrupt flags, and only then dispatches the CAPI callback (the test's
 * irq_callback). So by the time irq_callback calls platform_gpio_irq_ack(), the
 * raw pin status is already gone. To still answer "was my pin the source?", arm
 * registers a per-pin MSDK callback that latches the hit during that handler
 * pass; ack() reports and clears the latch. On the non-ISR ack paths (clearing
 * a latched edge before enabling), it falls back to draining the raw flags.
 */
#if defined(IRQ_CTRL_IDENTIFIER) && defined(GPIO_OUTPUT_OPS)
#include <stdbool.h>
#include "gpio.h"

static mxc_gpio_cfg_t gpio_irq_cfg;
static volatile bool  gpio_irq_fired;

static void gpio_irq_latch(void *arg)
{
	(void)arg;
	gpio_irq_fired = true;
}

int platform_gpio_irq_arm(uint32_t *irq_line)
{
	if (irq_line == NULL)
		return -EINVAL;

	gpio_irq_cfg.port   = MXC_GPIO_GET_GPIO(GPIO_INPUT_IDENTIFIER);
	gpio_irq_cfg.mask   = (1U << GPIO_INPUT_PIN);
	gpio_irq_cfg.func   = MXC_GPIO_FUNC_IN;
	gpio_irq_cfg.pad    = MXC_GPIO_PAD_NONE;
	gpio_irq_cfg.vssel  = MXC_GPIO_VSSEL_VDDIO;
	gpio_irq_cfg.drvstr = MXC_GPIO_DRVSTR_0;

	if (MXC_GPIO_Config(&gpio_irq_cfg) != E_NO_ERROR)
		return -EIO;

	gpio_irq_fired = false;
	MXC_GPIO_RegisterCallback(&gpio_irq_cfg, gpio_irq_latch, NULL);
	MXC_GPIO_IntConfig(&gpio_irq_cfg, MXC_GPIO_INT_RISING);
	MXC_GPIO_ClearFlags(gpio_irq_cfg.port, gpio_irq_cfg.mask);
	MXC_GPIO_EnableInt(gpio_irq_cfg.port, gpio_irq_cfg.mask);

	/* CAPI Maxim IRQ line == the port's raw NVIC IRQn. */
	*irq_line = (uint32_t)MXC_GPIO_GET_IRQ(GPIO_INPUT_IDENTIFIER);
	return 0;
}

bool platform_gpio_irq_ack(void)
{
	/* ISR path: the common handler already cleared HW; report the latch. */
	if (gpio_irq_fired) {
		gpio_irq_fired = false;
		return true;
	}

	/* Non-ISR path: drain a raw latched edge if one is pending. */
	if (MXC_GPIO_GetFlags(gpio_irq_cfg.port) & gpio_irq_cfg.mask) {
		MXC_GPIO_ClearFlags(gpio_irq_cfg.port, gpio_irq_cfg.mask);
		return true;
	}

	return false;
}

void platform_gpio_irq_disarm(void)
{
	MXC_GPIO_DisableInt(gpio_irq_cfg.port, gpio_irq_cfg.mask);
	MXC_GPIO_ClearFlags(gpio_irq_cfg.port, gpio_irq_cfg.mask);
	gpio_irq_fired = false;
}
#endif /* IRQ_CTRL_IDENTIFIER && GPIO_OUTPUT_OPS */

/**
 * @brief Main function execution for the Maxim platform.
 * @return Result of the enabled example execution.
 *
 * MSDK's Reset_Handler/SystemInit set up clocks, the vector table and the C
 * runtime before main(); there is no no-OS platform system-init on Maxim. The
 * one cross-cutting bring-up owned here is the interrupt controller, and it
 * runs UNCONDITIONALLY before example_main(): the Maxim UART backend connects
 * and enables its interrupt line inside capi_uart_init(), and
 * capi_irq_connect()/_enable() return -EINVAL until the NVIC is initialized.
 * So IRQ init cannot be gated behind an async-capability macro the way the
 * gate-on-capability platforms do it.
 */
int main(void)
{
	if (capi_irq_init(&irq_config) == 0)
		(void)capi_irq_global_enable();

	return example_main();
}
