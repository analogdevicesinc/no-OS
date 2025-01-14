/***************************************************************************//**
 *   @file   no_os_irq.c
 *   @brief  Implementation of the IRQ Interface
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#include <inttypes.h>
#include <stdlib.h>
#include "no_os_irq.h"
#include "no_os_error.h"

/**
 * @brief Initialize the IRQ interrupts.
 * @param desc - The IRQ descriptor.
 * @param param - The structure that contains the IRQ parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
			const struct no_os_irq_init_param *param)
{
	int32_t ret;

	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->init)
		return -ENOSYS;

	ret = param->platform_ops->init(desc, param);
	if (ret)
		return ret;

	(*desc)->ref++;
	(*desc)->platform_ops = param->platform_ops;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_irq_ctrl_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	if (--desc->ref)
		return 0;

	return desc->platform_ops->remove(desc);
}

/**
 * @brief Register a callback to handle the irq events.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param callback - Callback descriptor - platform specific type.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int no_os_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
				uint32_t irq_id,
				struct no_os_callback_desc *callback)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->register_callback)
		return -ENOSYS;

	return desc->platform_ops->register_callback(desc, irq_id, callback);
}

/**
 * @brief Unregisters a generic IRQ handling function.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param callback - Callback descriptor - platform specific type.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int no_os_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
				  uint32_t irq_id,
				  struct no_os_callback_desc *callback)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->unregister_callback)
		return -ENOSYS;

	return desc->platform_ops->unregister_callback(desc, irq_id, callback);
}

/**
 * @brief Enable global interrupts.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->global_enable)
		return -ENOSYS;

	return desc->platform_ops->global_enable(desc);
}

/**
 * @brief Disable global interrupts.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->global_disable)
		return -ENOSYS;

	return desc->platform_ops->global_disable(desc);
}

/**
 * @brief Set interrupt trigger level.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param trig - New trigger level for the interrupt.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
				uint32_t irq_id,
				enum no_os_irq_trig_level trig)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->trigger_level_set)
		return -ENOSYS;

	return desc->platform_ops->trigger_level_set(desc, irq_id, trig);
}

/**
 * @brief Enable specific interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->enable)
		return -ENOSYS;

	return desc->platform_ops->enable(desc, irq_id);
}

/**
 * @brief Disable specific interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return 0 in case of success, -1 otherwise.
 */
int no_os_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->disable)
		return -ENOSYS;

	return desc->platform_ops->disable(desc, irq_id);
}

/**
 * @brief Set the priority for an interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param priority_level - The priority level of the interrupt.
 * @return 0 in case of success, negative errno error codes.
 */
int no_os_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
			   uint32_t irq_id,
			   uint32_t priority_level)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->set_priority)
		return -ENOSYS;

	return desc->platform_ops->set_priority(desc, irq_id, priority_level);
}

/**
 * @brief Clear the pending interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return 0 in case of success, negative errno error codes.
 */
int no_os_irq_clear_pending(struct no_os_irq_ctrl_desc* desc,
			    uint32_t irq_id)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->clear_pending)
		return -ENOSYS;

	return desc->platform_ops->clear_pending(desc, irq_id);
}
