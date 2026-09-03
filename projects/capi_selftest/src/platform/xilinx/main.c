/***************************************************************************//**
 * @file main.c
 * @brief Main file for Xilinx platform of capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <xil_cache.h>
#include "parameters.h"
#include "common_data.h"

extern int example_main(void);

/*
 * ---------------------------------------------------------------------------
 * GPIO external-interrupt platform hooks for test_irq (see common_data.h).
 *
 * capi_irq wraps the interrupt controller; capi_gpio exposes no interrupt
 * surface. Which pin/channel fired and on what edge lives in the GPIO core's
 * own registers, so per-source config and the in-ISR acknowledge are bare
 * vendor-driver code and live here. The test stays platform-agnostic and
 * drives the loopback edge through the CAPI GPIO output port.
 *
 * Four combinations are handled from the compile-time selectors:
 *   GPIO backend   - GPIO_SEL_PS (XGpioPs, EMIO) vs GPIO_SEL_PL (XGpio, AXI).
 *   IRQ controller - the fabric/PS line is encoded as a GIC id or an AXI INTC
 *                    input depending on the controller the source feeds.
 *
 * arm() owns its own GPIO instance for the sampled (input) source only. The
 * test drives the output via capi_gpio; the two never cross owners, so capi's
 * private instance and this one don't fight over pin/channel state. Cfg-init
 * masks all interrupts, so arm() runs its enable after that mask.
 * ---------------------------------------------------------------------------
 */

#if defined(GPIO_SEL_PL)
/*
 * PL backend: the AXI GPIO input core raises ip2intc_irpt on any change to its
 * input register. The whole channel is the interrupt source; there is no per-pin
 * number. The core's fabric line feeds the GIC (SPI) on a GIC-only build or the
 * AXI INTC input on a cascade build; XPAR_XGPIO_1_INTERRUPT_PARENT low bit is 1
 * for INTC, so pick the encoding from the SDT descriptor.
 *
 * The input is channel 1 of a single-channel core (GPIO_1, IS_DUAL=0) wired to
 * the output core GPIO_0 -- so arm the channel-1 interrupt bit. (The older
 * dual-channel loopback IP put the input on channel 2 of one core; that is no
 * longer the topology.)
 */
#include <xgpio.h>
#include <xinterrupt_wrap.h>

static XGpio gpio_irq_inst;

#define GPIO_IRQ_CH1_MASK	0x1U	/* XGpio channel-1 interrupt bit */

int platform_gpio_irq_arm(uint32_t *irq_line)
{
	if (irq_line == NULL)
		return -EINVAL;

#if !defined(XPAR_XGPIO_1_INTERRUPTS)
	/*
	 * An XSA built without fabric interrupts emits no XPAR_XGPIO_1_INTERRUPTS
	 * (nor the _INTERRUPT_PARENT / XPAR_FABRIC_* macros the encoding below
	 * needs), so the loopback core has no IRQ to arm. Report it the way the
	 * contract expects and let the IRQ suite skip.
	 */
	return -ENOTSUP;
#else
	XGpio_Config *cfg = XGpio_LookupConfig((UINTPTR)GPIO_INPUT_IDENTIFIER);

	if (cfg == NULL)
		return -ENODEV;
	if (XGpio_CfgInitialize(&gpio_irq_inst, cfg, cfg->BaseAddress) !=
	    XST_SUCCESS)
		return -EIO;

	/* Enable channel-1 change interrupt and the core's global gate. */
	XGpio_InterruptClear(&gpio_irq_inst, GPIO_IRQ_CH1_MASK);
	XGpio_InterruptEnable(&gpio_irq_inst, GPIO_IRQ_CH1_MASK);
	XGpio_InterruptGlobalEnable(&gpio_irq_inst);

#if (XPAR_XGPIO_1_INTERRUPT_PARENT & 0x1U)
	/* Fabric line is an AXI INTC input (raw local number). */
	*irq_line = CAPI_IRQ_XILINX_INTC(XPAR_FABRIC_XGPIO_1_INTR);
#else
	/* Fabric line is a GIC SPI: resolve the SDT-encoded id to a GIC id. */
	*irq_line = CAPI_IRQ_XILINX_GIC(XGet_IntrId(XPAR_XGPIO_1_INTERRUPTS) +
					XGet_IntrOffset(XPAR_XGPIO_1_INTERRUPTS));
#endif
	return 0;
#endif /* XPAR_XGPIO_1_INTERRUPTS */
}

bool platform_gpio_irq_ack(void)
{
	uint32_t status = XGpio_InterruptGetStatus(&gpio_irq_inst);

	if (status & GPIO_IRQ_CH1_MASK) {
		XGpio_InterruptClear(&gpio_irq_inst, GPIO_IRQ_CH1_MASK);
		return true;
	}
	return false;
}

void platform_gpio_irq_disarm(void)
{
	XGpio_InterruptDisable(&gpio_irq_inst, GPIO_IRQ_CH1_MASK);
}

#else /* GPIO_SEL_PS */
/*
 * PS backend: a whole XGpioPs bank shares one GIC line (XPS_GPIO_INT_ID); the
 * per-pin config/status live in the GPIO regs. The PS GPIO interrupt is a fixed
 * PS peripheral SPI, so it is always a GIC id (never an AXI INTC input).
 */
#include <xgpiops.h>

static XGpioPs gpio_irq_inst;

int platform_gpio_irq_arm(uint32_t *irq_line)
{
	XGpioPs_Config *cfg;

	if (irq_line == NULL)
		return -EINVAL;

	cfg = XGpioPs_LookupConfig((UINTPTR)GPIO_INPUT_IDENTIFIER);
	if (cfg == NULL)
		return -ENODEV;
	if (XGpioPs_CfgInitialize(&gpio_irq_inst, cfg, cfg->BaseAddr) !=
	    XST_SUCCESS)
		return -EIO;

	/* Sample the input pin, interrupt on the rising edge the test drives. */
	XGpioPs_SetDirectionPin(&gpio_irq_inst, GPIO_INPUT_PIN, 0U);
	XGpioPs_SetIntrTypePin(&gpio_irq_inst, GPIO_INPUT_PIN,
			       XGPIOPS_IRQ_TYPE_EDGE_RISING);
	XGpioPs_IntrClearPin(&gpio_irq_inst, GPIO_INPUT_PIN);
	XGpioPs_IntrEnablePin(&gpio_irq_inst, GPIO_INPUT_PIN);

	*irq_line = CAPI_IRQ_XILINX_GIC(XPS_GPIO_INT_ID);
	return 0;
}

bool platform_gpio_irq_ack(void)
{
	uint32_t status = XGpioPs_IntrGetStatusPin(&gpio_irq_inst,
			  GPIO_INPUT_PIN);

	if (status)
		XGpioPs_IntrClearPin(&gpio_irq_inst, GPIO_INPUT_PIN);

	return status != 0U;
}

void platform_gpio_irq_disarm(void)
{
	XGpioPs_IntrDisablePin(&gpio_irq_inst, GPIO_INPUT_PIN);
}

#endif /* GPIO_SEL_PL / GPIO_SEL_PS */

#ifdef UART_ASYNC_OPS
/*
 * ---------------------------------------------------------------------------
 * Loopback UART sanity checks for test_uart (see common_data.h).
 *
 * The UART tests need no runtime platform hook -- the mapped core is reached
 * entirely through CAPI, and its interrupt is connected by the driver off the
 * irq_id in UART_ASYNC_EXTRA_INIT, on top of the one capi_irq_init() below. What
 * they DO need is that parameters.h mapped a sane instance, and both ways of
 * getting that wrong are silent at runtime, so they are caught here instead.
 * ---------------------------------------------------------------------------
 */

/*
 * The loopback UART must not be the console. test_uart reconfigures the line
 * rate mid-run and strap-loops TX into RX; pointed at the report transport that
 * kills the log, and the failure looks like a hang rather than a bad mapping.
 */
#if UART_ASYNC_IDENTIFIER == UART_IDENTIFIER
#error "UART_ASYNC_IDENTIFIER must not be the console UART (UART_IDENTIFIER)"
#endif

/*
 * The async cases are gated on UART_ASYNC_HAS_IRQ, which only skips them
 * correctly if it agrees with the use_irq the driver was configured with. Both
 * come out of parameters.h from the same XPAR_*_INTERRUPTS macro; if an edit
 * ever separates them, the async ops return -ENOTSUP and the cases FAIL where
 * they should SKIP. capi_irq_init() is what makes that IRQ deliverable, so a
 * build claiming one without an interrupt controller is the same mistake.
 */
#if UART_ASYNC_HAS_IRQ && !defined(IRQ_CTRL_IDENTIFIER)
#error "UART_ASYNC_HAS_IRQ set but no IRQ controller mapped (IRQ_CTRL_IDENTIFIER)"
#endif
#endif /* UART_ASYNC_OPS */

/**
 * @brief Main function execution for Xilinx platform.
 * @return Result of the enabled example execution.
 */
int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

#ifdef IRQ_CTRL_IDENTIFIER
	if (capi_irq_init(&irq_config) == 0)
		(void)capi_irq_global_enable();
#endif

	return example_main();
}
