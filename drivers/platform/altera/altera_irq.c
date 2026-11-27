/***************************************************************************//**
 *   @file   altera/altera_irq.c
 *   @brief  Altera / Nios V (CLIC/CLINT) IRQ controller driver.
 *   @author Mihaela-Georgeta Petrea (Mihaela-georgeta.Petrea@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

/*
 * The whole translation unit is a no-op unless building for the Altera / Nios V
 * platform, so the file can sit in a shared driver directory without affecting
 * MicroBlaze/Xilinx or any other target.
 */
#if defined(CONFIG_ALTERA_PLATFORM_NIOSV)

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdbool.h>
#include "altera_irq.h"
#include "no_os_irq.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/* BSP HAL: core-local interrupt controller (CLIC/CLINT) primitives. */
#include "sys/alt_irq.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief One-shot bring-up of the Nios V core-local interrupt controller.
 *
 * alt_irq_init() -> intel_niosv_irq_init() clears MIE and re-initializes the
 * CLIC on every call, which would wipe an already-enabled peripheral interrupt
 * line. Guard it so the controller HW is initialized exactly once, no matter
 * whether the UART driver or the IRQ controller driver reaches it first.
 */
void altera_irq_hw_init_once(void)
{
	static bool done;

	if (done)
		return;

	/*
	 * Bring up the HAL interrupt-controller driver(s). The generated BSP
	 * normally calls this from alt_main(); a bare no-OS application replaces
	 * main(), so do it explicitly here.
	 */
	alt_irq_init(NULL);
	done = true;
}

/**
 * @brief Initialize the Altera / Nios V interrupt controller.
 * @param desc  - Pointer where the descriptor is stored.
 * @param param - Configuration information (irq_ctrl_id is used as the HAL
 *                interrupt controller id).
 * @return 0 in case of success, negative error code otherwise.
 */
static int altera_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
				const struct no_os_irq_init_param *param)
{
	struct no_os_irq_ctrl_desc *ldesc;
	struct altera_irq_desc *aldesc;

	if (!desc || !param)
		return -EINVAL;

	ldesc = no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	aldesc = no_os_calloc(1, sizeof(*aldesc));
	if (!aldesc) {
		no_os_free(ldesc);
		return -ENOMEM;
	}

	aldesc->ic_id = param->irq_ctrl_id;

	ldesc->irq_ctrl_id = param->irq_ctrl_id;
	ldesc->platform_ops = param->platform_ops;
	ldesc->extra = aldesc;

	/* Bring up the controller HW exactly once (order independent). */
	altera_irq_hw_init_once();

	*desc = ldesc;

	return 0;
}

/**
 * @brief Register a callback (ISR) for a given interrupt line.
 * @param desc          - The IRQ controller descriptor.
 * @param irq_id        - Interrupt line number (Qsys IRQ, e.g. SYS_*_IRQ).
 * @param callback_desc - Descriptor of the callback to register.
 * @return 0 in case of success, negative error code otherwise.
 */
static int altera_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
					uint32_t irq_id,
					struct no_os_callback_desc *callback_desc)
{
	struct altera_irq_desc *aldesc;

	if (!desc || !desc->extra || !callback_desc || !callback_desc->callback)
		return -EINVAL;

	aldesc = desc->extra;

	/*
	 * alt_ic_isr_register() stores the handler in the vector table and, on
	 * success, enables the line at the controller. The no-OS callback type
	 * matches the HAL alt_isr_func type (void (*)(void *)).
	 */
	return alt_ic_isr_register(aldesc->ic_id, irq_id,
				   (alt_isr_func)callback_desc->callback,
				   callback_desc->ctx, NULL);
}

/**
 * @brief Unregister a callback for a given interrupt line.
 * @param desc          - The IRQ controller descriptor.
 * @param irq_id        - Interrupt line number.
 * @param callback_desc - Descriptor of the callback (unused).
 * @return 0 in case of success, negative error code otherwise.
 */
static int altera_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, struct no_os_callback_desc *callback_desc)
{
	struct altera_irq_desc *aldesc;

	(void)callback_desc;

	if (!desc || !desc->extra)
		return -EINVAL;

	aldesc = desc->extra;

	alt_ic_irq_disable(aldesc->ic_id, irq_id);
	/* Point the vector back at a null handler. */
	return alt_ic_isr_register(aldesc->ic_id, irq_id, NULL, NULL, NULL);
}

/**
 * @brief Enable a specific interrupt line.
 * @param desc   - The IRQ controller descriptor.
 * @param irq_id - Interrupt line number.
 * @return 0 in case of success, negative error code otherwise.
 */
static int altera_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	struct altera_irq_desc *aldesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	aldesc = desc->extra;
	alt_ic_irq_enable(aldesc->ic_id, irq_id);

	return 0;
}

/**
 * @brief Disable a specific interrupt line.
 * @param desc   - The IRQ controller descriptor.
 * @param irq_id - Interrupt line number.
 * @return 0 in case of success, negative error code otherwise.
 */
static int altera_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	struct altera_irq_desc *aldesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	aldesc = desc->extra;
	alt_ic_irq_disable(aldesc->ic_id, irq_id);

	return 0;
}

/**
 * @brief Globally enable interrupts (set the core MIE).
 * @param desc - The IRQ controller descriptor (unused).
 * @return 0 in case of success, negative error code otherwise.
 */
static int altera_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	(void)desc;
	alt_irq_cpu_enable_interrupts();

	return 0;
}

/**
 * @brief Globally disable interrupts (clear the core MIE).
 * @param desc - The IRQ controller descriptor (unused).
 * @return 0 in case of success, negative error code otherwise.
 */
static int altera_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	(void)desc;
	/* Discard the returned context: this is a permanent global mask. */
	(void)alt_irq_disable_all();

	return 0;
}

/**
 * @brief Free the resources allocated by altera_irq_ctrl_init().
 * @param desc - The IRQ controller descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int altera_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Altera / Nios V specific IRQ platform ops structure.
 *
 * set_priority/get_priority, clear_pending/set_pending and trigger_level_set
 * are intentionally omitted (the core-local controller drives them from the HDL
 * / HAL), mirroring the Xilinx driver.
 */
const struct no_os_irq_platform_ops altera_irq_ops = {
	.init = &altera_irq_ctrl_init,
	.register_callback = &altera_irq_register_callback,
	.unregister_callback = &altera_irq_unregister_callback,
	.global_enable = &altera_irq_global_enable,
	.global_disable = &altera_irq_global_disable,
	.enable = &altera_irq_enable,
	.disable = &altera_irq_disable,
	.remove = &altera_irq_ctrl_remove,
};

#endif /* CONFIG_ALTERA_PLATFORM_NIOSV */
