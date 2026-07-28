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
 * PL backend: the dual-channel AXI GPIO loopback core raises ip2intc_irpt on a
 * channel-2 (input) change. The whole channel is the interrupt source; there is
 * no per-pin number. The core's fabric line feeds the GIC (SPI) on a GIC-only
 * build or the AXI INTC input on a cascade build; XPAR_XGPIO_1_INTERRUPT_PARENT
 * low bit is 1 for INTC, so pick the encoding from the SDT descriptor.
 */
#include <xgpio.h>
#include <xinterrupt_wrap.h>

static XGpio gpio_irq_inst;

#define GPIO_IRQ_CH2_MASK	0x2U	/* XGpio channel-2 interrupt bit */

int platform_gpio_irq_arm(uint32_t *irq_line)
{
	XGpio_Config *cfg;

	if (irq_line == NULL)
		return -EINVAL;

	cfg = XGpio_LookupConfig((UINTPTR)GPIO_INPUT_IDENTIFIER);
	if (cfg == NULL)
		return -ENODEV;
	if (XGpio_CfgInitialize(&gpio_irq_inst, cfg, cfg->BaseAddress) !=
	    XST_SUCCESS)
		return -EIO;

	/* Enable channel-2 change interrupt and the core's global gate. */
	XGpio_InterruptClear(&gpio_irq_inst, GPIO_IRQ_CH2_MASK);
	XGpio_InterruptEnable(&gpio_irq_inst, GPIO_IRQ_CH2_MASK);
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
}

bool platform_gpio_irq_ack(void)
{
	uint32_t status = XGpio_InterruptGetStatus(&gpio_irq_inst);

	if (status & GPIO_IRQ_CH2_MASK) {
		XGpio_InterruptClear(&gpio_irq_inst, GPIO_IRQ_CH2_MASK);
		return true;
	}
	return false;
}

void platform_gpio_irq_disarm(void)
{
	XGpio_InterruptDisable(&gpio_irq_inst, GPIO_IRQ_CH2_MASK);
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
