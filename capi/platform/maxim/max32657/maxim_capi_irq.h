/***************************************************************************//**
 *   @file   maxim_capi_irq.h
 *   @brief  Header file for IRQ functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
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
