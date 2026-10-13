/* SPDX-License-Identifier: BSD-3-Clause */

#include <errno.h>
#include "maxim_irq_priv.h"

/**
 * @brief Enable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int max_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	__enable_irq();

	return 0;
}

/**
 * @brief Disable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int max_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	__disable_irq();

	return 0;
}

/**
 * @brief Enable a specific interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @return 0 in case of success, errno error codes otherwise.
 */
int max_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	NVIC_EnableIRQ(irq_id);

	return 0;
}

/**
 * @brief Disable a specific interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_irq_disable(struct no_os_irq_ctrl_desc *desc,
		    uint32_t irq_id)
{
	if (irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	NVIC_DisableIRQ(irq_id);

	return 0;
}

/**
 * @brief Set a priority level for an interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param priority_level - The interrupt priority level
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
			 uint32_t irq_id,
			 uint32_t priority_level)
{
	if (irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	NVIC_SetPriority(irq_id, priority_level);

	return 0;
}
