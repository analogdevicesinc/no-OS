/***************************************************************************//**
 *   @file   maxim_capi_irq.h
 *   @brief  Header file for IRQ functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_IRQ_H_
#define MAXIM_CAPI_IRQ_H_

#include "uart.h"
#include "capi_gpio.h"
#include "capi_irq.h"

struct max_capi_irq_entry {
	/** Callback function */
	capi_isr_callback_t callback;
	/** Callback argument */
	void *arg;
	/* IRQ enabled flag */
	bool enabled;
};

struct max_capi_irq_extra_config {
	uint32_t default_priority;
};

extern const struct capi_irq_ops max_capi_irq_ops;

void max_capi_dma_callback(int ch, int reason);

int max_capi_gpio_irq_connect(struct capi_gpio_pin *pin,
			      capi_isr_callback_t isr, void *arg);
int max_capi_gpio_irq_disconnect(const struct capi_gpio_pin *pin);
int max_capi_gpio_irq_enable(struct capi_gpio_pin *pin);
int max_capi_gpio_irq_disable(struct capi_gpio_pin *pin);
int max_capi_gpio_irq_set_level_edge_trigger(struct capi_gpio_pin *pin,
		enum capi_irq_trig_level trigger);
int max_capi_gpio_irq_global_enable(void);
int max_capi_gpio_irq_global_disable(void);

#endif /* MAXIM_CAPI_IRQ_H_ */
