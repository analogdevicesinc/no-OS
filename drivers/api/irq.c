/***************************************************************************//**
 *   @file   irq.c
 *   @brief  Implementation of the IRQ Interface
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <inttypes.h>
#include "no_os_irq.h"
#include <stdlib.h>
#include "no_os_error.h"

/**
 * @brief Initialize the IRQ interrupts.
 * @param desc - The IRQ descriptor.
 * @param param - The structure that contains the IRQ parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_ctrl_init(struct irq_ctrl_desc **desc,
		      const struct irq_init_param *param)
{
	if (!param)
		return FAILURE;

	if ((param->platform_ops->init(desc, param)))
		return FAILURE;

	(*desc)->platform_ops = param->platform_ops;

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by irq_ctrl_init().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_ctrl_remove(struct irq_ctrl_desc *desc)
{
	return desc->platform_ops->remove(desc);
}

/**
 * @brief Register a callback to handle the irq events.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param callback_desc - Callback descriptor
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_register_callback(struct irq_ctrl_desc *desc, uint32_t irq_id,
			      struct callback_desc *callback_desc)
{
	return desc->platform_ops->register_callback(desc, irq_id, callback_desc);
}

/**
 * @brief Unregisters a generic IRQ handling function.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_unregister(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	return desc->platform_ops->unregister(desc, irq_id);
}

/**
 * @brief Enable global interrupts.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_global_enable(struct irq_ctrl_desc *desc)
{
	return desc->platform_ops->global_enable(desc);
}

/**
 * @brief Disable global interrupts.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_global_disable(struct irq_ctrl_desc *desc)
{
	return desc->platform_ops->global_disable(desc);
}

/**
 * @brief Set interrupt trigger level.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param trig - New trigger level for the interrupt.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_trigger_level_set(struct irq_ctrl_desc *desc, uint32_t irq_id,
			      enum irq_trig_level trig)
{
	if(desc->platform_ops->trigger_level_set)
		return desc->platform_ops->trigger_level_set(desc, irq_id, trig);
	else
		return ENOSYS;
}

/**
 * @brief Enable specific interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_enable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	return desc->platform_ops->enable(desc, irq_id);
}

/**
 * @brief Disable specific interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_disable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	return desc->platform_ops->disable(desc, irq_id);
}
