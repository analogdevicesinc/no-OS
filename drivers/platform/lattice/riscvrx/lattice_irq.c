/*******************************************************************************
 *   @file   lattice_irq.c
 *   @brief  Implementation of IRQ Generic Driver for Lattice RISC-V RX.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_irq.h"
#include "no_os_alloc.h"
#include "lattice_irq.h"

/******************************************************************************/
/************************* Variable Declarations ******************************/
/******************************************************************************/

static bool initialized =  false;
extern struct interrupt_entry plic_int_table[];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the IRQ interrupts.
 * @param desc - The IRQ controller descriptor.
 * @param param - The structure that contains the IRQ parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t latt_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
			  const struct no_os_irq_init_param *param)
{
	static struct no_os_irq_ctrl_desc *descriptor;
	struct latt_irq_desc *latt_desc;
	struct latt_irq_init_param *latt_ip;

	latt_desc = (struct latt_irq_desc *) no_os_malloc(sizeof(*latt_desc));
	if (!latt_desc)
		return -ENOMEM;

	if (!param)
		return -EINVAL;

	if (!initialized) {
		descriptor = no_os_calloc(1, sizeof(*descriptor));
		if (!descriptor)
			return -ENOMEM;
		initialized = true;
	}

	if (!param->extra)
		return -EINVAL;

	latt_ip = param->extra;

	latt_desc->mode = latt_ip->mode;

	/* There is only 1 interrupt controller and that is PLIC */
	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->extra = latt_desc;

	*desc = descriptor;

	plic_init(); //initialises the interrupt controller

	return 0;
}

/**
 * @brief Unused.
 * @param desc   - Irq descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param level  - The trigger condition.
 * @return -ENOSYS
 */
int32_t latt_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
			       uint32_t irq_id,
			       enum no_os_irq_trig_level level)
{
	return -ENOSYS;
}

/**
 * @brief Set a priority level for an interrupt.
 * @param desc           - Interrupt controller descriptor.
 * @param irq_id         - The interrupt vector entry id of the peripheral.
 * @param priority_level - The interrupt priority level.
 * @return 0 if successful, error code otherwise.
 */
int32_t latt_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
				     uint32_t irq_id,
				     uint32_t priority_level)
{
	if (!desc || (irq_id > PLIC_INT_SRC_MAX) || (priority_level > 7)) {
		return -EINVAL;
	}

	PLIC_PRIORITY(irq_id) = priority_level;

	return 0;
}

/**
 * @brief Enable global interrupts.
 */
int32_t latt_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	/* enable machine-mode external interrupts. */
	w_mie(r_mie() | MIE_MEIE);

	/* enable supervisor-mode external interrupts. */
	w_mie(r_mie() | SIE_SEIE);

	return 0;
}

/**
 * @brief Disable global interrupts.
 */
int32_t latt_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	/* enable machine-mode external interrupts. */
	w_mie(r_mie() & (~MIE_MEIE));

	/* enable supervisor-mode external interrupts. */
	w_mie(r_mie() & (~SIE_SEIE));

	return 0;
}

/**
 * @brief Enable specific interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t latt_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (!desc || (irq_id > PLIC_INT_SRC_MAX)) {
		return -EINVAL;
	}

	struct latt_irq_desc *latt_dev = desc->extra;
	int hart = r_tp();

	switch(latt_dev->mode) {
	case ePRIV_M:
		PLIC_ENABLE_M(hart) |= 1 << irq_id;
		break;
	case ePRIV_S:
		PLIC_ENABLE_S(hart) |= 1 << irq_id;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Disable specific interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t latt_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (!desc || (irq_id > PLIC_INT_SRC_MAX)) {
		return -EINVAL;
	}

	struct latt_irq_desc *latt_dev = desc->extra;
	int hart = r_tp();

	switch(latt_dev->mode) {
	case ePRIV_M:
		PLIC_ENABLE_M(hart) &= (~(1 << irq_id));
		break;
	case ePRIV_S:
		PLIC_ENABLE_S(hart) &= (~(1 << irq_id));
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Register a callback to handle the irq events.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param callback_desc - Callback descriptor
 * @return 0 in case of success, -1 otherwise.
 */
int32_t latt_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
				  uint32_t irq_id,
				  struct no_os_callback_desc *callback_desc)
{
	if (!desc || (irq_id > PLIC_INT_SRC_MAX)) {
		return -EINVAL;
	}

	plic_int_table[irq_id].isr = callback_desc->callback;
	plic_int_table[irq_id].context = callback_desc->ctx;

	return 0;
}

/**
 * @brief Unregisters a generic IRQ handling function.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param cb - Callback descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t latt_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
				    uint32_t irq_id, struct no_os_callback_desc *cb)
{
	if (!desc || (irq_id > PLIC_INT_SRC_MAX)) {
		return -EINVAL;
	}

	struct latt_irq_desc *latt_dev = desc->extra;
	int hart = r_tp();

	switch(latt_dev->mode) {
	case ePRIV_M:
		PLIC_ENABLE_M(hart) &= (~(1 << irq_id));
		break;
	case ePRIV_S:
		PLIC_ENABLE_S(hart) &= (~(1 << irq_id));
		break;
	default:
		return -1;
	}

	plic_int_table[irq_id].isr = 0;
	plic_int_table[irq_id].context = 0;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_irq_ctrl_init().
 * @param desc - The IRQ control descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t latt_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	initialized = false;

	if (desc->extra)
		no_os_free(desc->extra);
	if (desc)
		no_os_free(desc);

	return 0;
}

/**
 * @brief Lattice platform specific IRQ platform ops structure
 */
const struct no_os_irq_platform_ops latt_irq_ops = {
	.set_priority = &latt_irq_set_priority,
	.init = &latt_irq_ctrl_init,
	.register_callback = &latt_irq_register_callback,
	.unregister_callback = &latt_irq_unregister_callback,
	.global_enable = &latt_irq_global_enable,
	.global_disable = &latt_irq_global_disable,
	.trigger_level_set = &latt_trigger_level_set,
	.enable = &latt_irq_enable,
	.disable = &latt_irq_disable,
	.remove = &latt_irq_ctrl_remove};
