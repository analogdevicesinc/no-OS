/***************************************************************************//**
 *   @file   maxim_irq.c
 *   @brief  Source file of the NVIC interrupt controller driver for the maxim
 * 	     platform
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include "no_os_error.h"
#include "no_os_mutex.h"
#include "max32655.h"
#include "capi_irq.h"
#include "maxim_irq.h"

struct capi_isr_table isr_table[MXC_IRQ_EXT_COUNT];
static void *nvic_lock;

int capi_irq_init(struct capi_irq_config *config)
{
	no_os_mutex_init(&nvic_lock);

	return 0;
}

int capi_irq_deinit(void)
{
	no_os_mutex_remove(nvic_lock);

	return 0;
}

int capi_irq_global_enable(void)
{
	no_os_mutex_lock(nvic_lock);
	__enable_irq();
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_global_disable(void)
{
	no_os_mutex_lock(nvic_lock);
	__disable_irq();
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_enable(uint32_t irq)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	NVIC_EnableIRQ(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_disable(uint32_t irq)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	NVIC_DisableIRQ(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	uint8_t enabled;

	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	enabled = NVIC_GetEnableIRQ(irq);
	NVIC_DisableIRQ(irq);
	isr_table[irq].isr = isr;
	isr_table[irq].arg = arg;

	if (enabled)
		NVIC_EnableIRQ(irq);

	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_clear_pending(uint32_t irq)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	NVIC_ClearPendingIRQ(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	*pactive  = NVIC_GetActive(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	NVIC_SetPriority(irq, priority);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	if (irq >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	no_os_mutex_lock(nvic_lock);
	*priority = NVIC_GetPriority(irq);
	no_os_mutex_unlock(nvic_lock);

	return 0;
}

int capi_irq_set_level_edge_trigger(uint32_t irq, enum capi_irq_trig_level trigger)
{
	return -ENOSYS;
}
