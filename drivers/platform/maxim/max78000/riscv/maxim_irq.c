/* SPDX-License-Identifier: BSD-3-Clause */

#include <errno.h>
#include "maxim_irq_priv.h"

int max_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	__asm__ volatile("csrsi mstatus, 8" ::: "memory");
	return 0;
}

int max_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	__asm__ volatile("csrci mstatus, 8" ::: "memory");
	return 0;
}

int max_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	uint32_t state;

	if (irq_id < PF_IRQn || irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	state = maxim_irq_save();
	NVIC_EnableIRQ(irq_id);
	maxim_irq_restore(state);
	return 0;
}

int max_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	uint32_t state;

	if (irq_id < PF_IRQn || irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	state = maxim_irq_save();
	NVIC_DisableIRQ(irq_id);
	maxim_irq_restore(state);
	return 0;
}

int max_irq_set_priority(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id,
			 uint32_t priority_level)
{
	if (irq_id < PF_IRQn || irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	return -ENOTSUP;
}
