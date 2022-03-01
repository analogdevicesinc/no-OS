/***************************************************************************//**
 *   @file   maxim/maxim_irq.c
 *   @brief  Implementation of external irq driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "no-os/irq.h"
#include "irq_extra.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialized the controller for the MAX32660 external interrupts
 * @param desc - Pointer where the configured instance is stored
 * @param param - Configuration information for the instance
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_irq_ctrl_init(struct irq_ctrl_desc **desc,
			  const struct irq_init_param *param)
{
	struct irq_ctrl_desc *descriptor;

	if (!param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->platform_ops = param->platform_ops;
	descriptor->extra = param->extra;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Free the resources allocated by irq_ctrl_init()
 * @param desc - Interrupt controller descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_irq_ctrl_remove(struct irq_ctrl_desc *desc)
{
	if (!desc)
		return -EINVAL;

	for(uint32_t i = 0; i < MXC_IRQ_COUNT; i++) {
		NVIC_DisableIRQ(i);
	}

	free(desc);

	return 0;
}

/**
 * @brief Not implemented.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param callback_desc - Descriptor of the callback.
 * @return -ENOSYS
 */
int32_t max_irq_register_callback(struct irq_ctrl_desc *desc, uint32_t irq_id,
				  void *callback_desc)
{
	return -ENOSYS;
}

/**
 * @brief Not implemented
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Id of the interrupt
 * @return -ENOSYS
 */
int32_t max_irq_unregister_callback(struct irq_ctrl_desc *desc, uint32_t irq_id, void *callback)
{
	return -ENOSYS;
}

/**
 * @brief Enable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t max_irq_global_enable(struct irq_ctrl_desc *desc)
{
	for(uint32_t i = 0; i < MXC_IRQ_COUNT; i++) {
		NVIC_EnableIRQ(i);
	}

	return 0;
}

/**
 * @brief Disable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t max_irq_global_disable(struct irq_ctrl_desc *desc)
{
	for(uint32_t i = 0; i < MXC_IRQ_COUNT; i++) {
		NVIC_ClearPendingIRQ(i);
		NVIC_DisableIRQ(i);
	}

	return 0;
}

/**
 * @brief Enable a specific interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Interrupt identifier
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max_irq_enable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	NVIC_EnableIRQ(irq_id);

	return 0;
}

/**
 * @brief Disable a specific interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Interrupt identifier
 * @return 0 in case of success, -EINVAL otherwise.
 */
int32_t max_irq_disable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	NVIC_DisableIRQ(irq_id);

	return 0;
}

/**
 * @brief maxim specific IRQ platform ops structure
 */
const struct irq_platform_ops irq_ops = {
	.init = &max_irq_ctrl_init,
	.register_callback = &max_irq_register_callback,
	.unregister_callback = &max_irq_unregister_callback,
	.global_enable = &max_irq_global_enable,
	.global_disable = &max_irq_global_disable,
	.enable = &max_irq_enable,
	.disable = &max_irq_disable,
	.remove = &max_irq_ctrl_remove
};
