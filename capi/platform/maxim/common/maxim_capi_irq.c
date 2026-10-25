/*******************************************************************************
 *   @file   maxim_capi_irq.c
 *   @brief  Implementation of the MAX32650/MAX32690 CAPI IRQ functions
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <string.h>
#include "maxim_capi_irq.h"
#include "mxc_device.h"
#include "nvic_table.h"
#include "gpio.h"

static bool irq_initialized;
static struct max_capi_irq_entry irq_table[MXC_IRQ_EXT_COUNT];
static uint32_t default_priority;

/**
 * @brief Get the GPIO port that drives an IRQ line.
 * @param irq - IRQ number.
 * @return The GPIO port, NULL if the IRQ is not a GPIO port IRQ.
 */
static mxc_gpio_regs_t *max_capi_irq_gpio_port(uint32_t irq)
{
	uint32_t i;

	for (i = 0; i < MXC_CFG_GPIO_INSTANCES; i++)
		if ((uint32_t)MXC_GPIO_GET_IRQ(i) == irq)
			return MXC_GPIO_GET_GPIO(i);

	return NULL;
}

/**
 * @brief Vector installed for every connected IRQ.
 *
 * GPIO port flags latched at entry are cleared after the callback runs, so
 * the callback can still read which pins fired.
 */
static void max_capi_irq_dispatch(void)
{
	uint32_t irq = (__get_IPSR() & 0x1FFU) - 16U;
	struct max_capi_irq_entry *entry = &irq_table[irq];
	mxc_gpio_regs_t *port;
	uint32_t flags = 0;

	port = max_capi_irq_gpio_port(irq);
	if (port)
		flags = MXC_GPIO_GetFlags(port);

	if (entry->callback)
		entry->callback(entry->arg);

	if (port)
		MXC_GPIO_ClearFlags(port, flags);
}

static inline bool max_capi_irq_valid(uint32_t irq)
{
	return irq_initialized && irq < MXC_IRQ_EXT_COUNT;
}

int capi_irq_init(struct capi_irq_config *config)
{
	const struct max_capi_irq_extra_config *extra;

	if (!config)
		return -EINVAL;

	if (irq_initialized)
		return -EBUSY;

	memset(irq_table, 0, sizeof(irq_table));

	extra = config->extra;
	default_priority = extra ? extra->default_priority : 0;
	irq_initialized = true;

	return 0;
}

int capi_irq_deinit(void)
{
	uint32_t i;

	if (!irq_initialized)
		return -EINVAL;

	for (i = 0; i < MXC_IRQ_EXT_COUNT; i++) {
		if (irq_table[i].enabled)
			NVIC_DisableIRQ((IRQn_Type)i);
		if (irq_table[i].connected)
			MXC_NVIC_SetVector((IRQn_Type)i, irq_table[i].prev_vector);
	}

	memset(irq_table, 0, sizeof(irq_table));
	irq_initialized = false;

	return 0;
}

int capi_irq_global_enable(void)
{
	__enable_irq();

	return 0;
}

int capi_irq_global_disable(void)
{
	__disable_irq();

	return 0;
}

int capi_irq_enable(uint32_t irq)
{
	if (!max_capi_irq_valid(irq))
		return -EINVAL;

	if (!irq_table[irq].enabled &&
	    !(NVIC->ISER[irq >> 5] & (1UL << (irq & 0x1FU))))
		NVIC_SetPriority((IRQn_Type)irq, default_priority);

	irq_table[irq].enabled = true;
	NVIC_EnableIRQ((IRQn_Type)irq);

	return 0;
}

int capi_irq_disable(uint32_t irq)
{
	if (!max_capi_irq_valid(irq))
		return -EINVAL;

	NVIC_DisableIRQ((IRQn_Type)irq);
	irq_table[irq].enabled = false;

	return 0;
}

int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	struct max_capi_irq_entry *entry;
	void (**vectors)(void);

	if (!max_capi_irq_valid(irq) || !isr)
		return -EINVAL;

	entry = &irq_table[irq];
	if (!entry->connected) {
		vectors = (void (**)(void))SCB->VTOR;
		entry->prev_vector = vectors[irq + 16];
	}

	entry->callback = isr;
	entry->arg = arg;
	entry->connected = true;
	MXC_NVIC_SetVector((IRQn_Type)irq, max_capi_irq_dispatch);

	return 0;
}

int capi_irq_clear_pending(uint32_t irq)
{
	if (!max_capi_irq_valid(irq))
		return -EINVAL;

	NVIC_ClearPendingIRQ((IRQn_Type)irq);

	return 0;
}

int capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	if (!max_capi_irq_valid(irq) || !pactive)
		return -EINVAL;

	*pactive = NVIC_GetActive((IRQn_Type)irq);

	return 0;
}

int capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	if (!max_capi_irq_valid(irq))
		return -EINVAL;

	NVIC_SetPriority((IRQn_Type)irq, priority);

	return 0;
}

int capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	if (!max_capi_irq_valid(irq) || !priority)
		return -EINVAL;

	*priority = NVIC_GetPriority((IRQn_Type)irq);

	return 0;
}

/**
 * @brief Set the trigger of a GPIO port IRQ.
 *
 * The CAPI IRQ API has no pin argument, so the trigger applies to every pin
 * of the port; only pins with their interrupt enabled raise the IRQ.
 *
 * @param irq - GPIO port IRQ number.
 * @param trigger - Level or edge trigger.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_set_level_edge_trigger(uint32_t irq,
				    enum capi_irq_trig_level trigger)
{
	mxc_gpio_int_pol_t pol;
	mxc_gpio_cfg_t cfg = {
		.mask = 0xFFFFFFFF,
		.func = MXC_GPIO_FUNC_IN,
	};

	if (!max_capi_irq_valid(irq))
		return -EINVAL;

	cfg.port = max_capi_irq_gpio_port(irq);
	if (!cfg.port)
		return -ENOTSUP;

	switch (trigger) {
	case CAPI_IRQ_LEVEL_LOW:
		pol = MXC_GPIO_INT_LOW;
		break;
	case CAPI_IRQ_LEVEL_HIGH:
		pol = MXC_GPIO_INT_HIGH;
		break;
	case CAPI_IRQ_EDGE_FALLING:
		pol = MXC_GPIO_INT_FALLING;
		break;
	case CAPI_IRQ_EDGE_RISING:
		pol = MXC_GPIO_INT_RISING;
		break;
	case CAPI_IRQ_EDGE_BOTH:
		pol = MXC_GPIO_INT_BOTH;
		break;
	default:
		return -EINVAL;
	}

	return MXC_GPIO_IntConfig(&cfg, pol) ? -EIO : 0;
}
