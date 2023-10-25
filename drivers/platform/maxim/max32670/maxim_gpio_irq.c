/***************************************************************************//**
 *   @file   maxim_gpio_irq.c
 *   @brief  Source file for GPIO IRQ driver.
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
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_list.h"
#include "no_os_irq.h"
#include "no_os_gpio.h"

#include "maxim_gpio_irq.h"
#include "maxim_irq.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

static struct no_os_list_desc *actions[MXC_CFG_GPIO_INSTANCES];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief GPIO callback function that sets the event and further calls
 * the user registered callback
 * @param cbdata - Used to pass an action reference
 */
static void gpio_irq_callback(void *cbdata)
{
	int ret;
	struct irq_action *action;
	struct irq_action *key = cbdata;

	/* key->handle is the address of the GPIO port */
	uint32_t id = MXC_GPIO_GET_IDX(key->handle);

	ret = no_os_list_read_find(actions[id], (void **)&action, key);
	if (ret)
		return;

	if (action->callback)
		action->callback(action->ctx);
}

void GPIO0_IRQHandler()
{
	MXC_GPIO_Handler(0);
}

#ifdef MXC_GPIO1
void GPIO1_IRQHandler()
{
	MXC_GPIO_Handler(1);
}
#endif

#ifdef MXC_GPIO2
void GPIO2_IRQHandler()
{
	MXC_GPIO_Handler(2);
}
#endif

/**
 * @brief Initialize the GPIO interrupt controller
 * @param desc - Pointer where the configured instance is stored
 * @param param - Configuration information for the instance
 * @return 0 in case of success, errno error codes otherwise.
 */
static int max_gpio_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
				  const struct no_os_irq_init_param *param)
{
	int ret;
	struct no_os_irq_ctrl_desc *descriptor;

	if (!param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->extra = param->extra;

	if (!actions[param->irq_ctrl_id]) {
		ret = no_os_list_init(&actions[param->irq_ctrl_id],
				      NO_OS_LIST_PRIORITY_LIST, irq_action_cmp);
		if (ret)
			goto error;
	}

	*desc = descriptor;

	return 0;

error:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_irq_ctrl_init()
 * @param desc - Interrupt GPIO controller descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
static int max_gpio_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	struct no_os_callback_desc *discard;

	if (!desc)
		return -EINVAL;

	while (0 == no_os_list_get_first(actions[desc->irq_ctrl_id], (void **)&discard))
		no_os_free(discard);

	no_os_list_remove(actions[desc->irq_ctrl_id]);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Register a callback function to be triggered when an
 * interrupt occurs.
 * @param desc - The GPIO IRQ controller descriptor.
 * @param irq_id - the pin on which the interrupt signal will be.
 * @param callback_desc - Descriptor of the callback.
 * @return 0 in case of success, errno error codes otherwise
 */
static int max_gpio_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		struct no_os_callback_desc *callback_desc)
{
	int ret;
	struct irq_action *action;
	struct irq_action action_key = {.irq_id = irq_id};
	mxc_gpio_cfg_t cfg;

	if (!desc || !callback_desc)
		return -EINVAL;

	ret = no_os_list_read_find(actions[desc->irq_ctrl_id], (void **)&action,
				   &action_key);
	/*
	* If no action was found, insert a new one, otherwise update it
	*/
	if (ret) {
		action = no_os_calloc(1, sizeof(*action));
		if (!action)
			return -ENOMEM;

		action->irq_id = irq_id;
		action->handle = MXC_GPIO_GET_GPIO(desc->irq_ctrl_id);
		action->ctx = callback_desc->ctx;
		action->callback = callback_desc->callback;

		ret = no_os_list_add_last(actions[desc->irq_ctrl_id], action);
		if (ret)
			goto free_action;
	} else {
		action->irq_id = irq_id;
		action->handle = MXC_GPIO_GET_GPIO(desc->irq_ctrl_id);
		action->ctx = callback_desc->ctx;
		action->callback = callback_desc->callback;
	}

	cfg = (mxc_gpio_cfg_t) {
		.mask = NO_OS_BIT(irq_id),
		.port = MXC_GPIO_GET_GPIO(desc->irq_ctrl_id)
	};
	MXC_GPIO_RegisterCallback(&cfg, gpio_irq_callback, action);

	return 0;

free_action:
	no_os_free(action);
	return ret;
}

/**
 * @brief Unregister a callback function
 * @param desc - GPIO interrupt controller descriptor.
 * @param irq_id - the pin on which the interrupt signal will be.
 * @param cb - Callback descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
static int max_gpio_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		struct no_os_callback_desc *callback_desc)
{
	int ret;
	struct irq_action *discard_action = NULL;
	struct irq_action action_key = {.irq_id = irq_id};
	mxc_gpio_cfg_t cfg;

	if (!desc || !callback_desc || irq_id >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	ret = no_os_list_read_find(actions[desc->irq_ctrl_id], (void **)&discard_action,
				   &action_key);
	if (ret)
		return -ENODEV;

	cfg = (mxc_gpio_cfg_t) {
		.port = MXC_GPIO_GET_GPIO(desc->irq_ctrl_id),
		.mask = NO_OS_BIT(irq_id)
	};
	MXC_GPIO_RegisterCallback(&cfg, NULL, NULL);
	no_os_free(discard_action);

	return 0;
}

/**
 * @brief Set the trigger condition.
 * @param desc - the GPIO irq descriptor.
 * @param irq_id - the pin on which the interrupt signal will be.
 * @param trig_l - the trigger condition.
 * @return 0 in case of success, errno error codes otherwise
 */
static int max_gpio_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		enum no_os_irq_trig_level trig)
{
	mxc_gpio_cfg_t cfg;

	const int32_t trig_level[5] = {
		[NO_OS_IRQ_LEVEL_LOW] = MXC_GPIO_INT_LOW,
		[NO_OS_IRQ_LEVEL_HIGH] = MXC_GPIO_INT_HIGH,
		[NO_OS_IRQ_EDGE_RISING] = MXC_GPIO_INT_RISING,
		[NO_OS_IRQ_EDGE_FALLING] = MXC_GPIO_INT_FALLING,
		[NO_OS_IRQ_EDGE_BOTH] = MXC_GPIO_INT_BOTH
	};

	if (!desc || irq_id >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	cfg = (mxc_gpio_cfg_t) {
		.port = MXC_GPIO_GET_GPIO(desc->irq_ctrl_id),
		.mask = NO_OS_BIT(irq_id)
	};
	MXC_GPIO_IntConfig(&cfg, trig_level[trig]);

	return 0;
}

/**
 * @brief Enable a specific interrupt
 * @param desc - the GPIO irq descriptor.
 * @param irq_id - the pin on which the interrupt signal will be.
 * @return 0 in case of success, errno error codes otherwise.
 */
static int max_gpio_irq_enable(struct no_os_irq_ctrl_desc *desc,
			       uint32_t irq_id)
{
	if (irq_id >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	MXC_GPIO_EnableInt(MXC_GPIO_GET_GPIO(desc->irq_ctrl_id), NO_OS_BIT(irq_id));

	return 0;
}

/**
 * @brief Disable a specific interrupt
 * @param desc - the GPIO irq descriptor.
 * @param irq_id - the pin on which the interrupt signal will be.
 * @return 0 in case of success, errno error codes otherwise.
 */
static int max_gpio_irq_disable(struct no_os_irq_ctrl_desc *desc,
				uint32_t irq_id)
{
	if (irq_id >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	MXC_GPIO_DisableInt(MXC_GPIO_GET_GPIO(desc->irq_ctrl_id), NO_OS_BIT(irq_id));

	return 0;
}

/**
 * @brief Enable all interrupts
 * @param desc - GPIO interrupt controller descriptor.
 * @return 0
 */
static int max_gpio_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	MXC_GPIO_EnableInt(MXC_GPIO_GET_GPIO(desc->irq_ctrl_id),
			   NO_OS_GENMASK(31, 0));

	return 0;
}

/**
 * @brief Disable all interrupts
 * @param desc - GPIO interrupt controller descriptor.
 * @return 0
 */
static int max_gpio_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	MXC_GPIO_DisableInt(MXC_GPIO_GET_GPIO(desc->irq_ctrl_id),
			    NO_OS_GENMASK(31, 0));

	return 0;
}

/**
 * @brief Set the interrupt priority for the current GPIO port.
 * @param desc - GPIO interrupt controller descriptor.
 * @param irq_id - Unused
 * @param priority_level - The interrupt priority level.
 * @return 0
 */
static int max_gpio_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
				     uint32_t irq_id,
				     uint32_t priority_level)
{
	uint32_t id = MXC_GPIO_GET_IRQ(desc->irq_ctrl_id);
	NVIC_SetPriority(id, priority_level);

	return 0;
}

/**
 * @brief maxim specific GPIO IRQ platform ops structure
 */
const struct no_os_irq_platform_ops max_gpio_irq_ops = {
	.init = (int32_t (*)())max_gpio_irq_ctrl_init,
	.register_callback = (int32_t (*)())max_gpio_irq_register_callback,
	.unregister_callback = (int32_t (*)())max_gpio_irq_unregister_callback,
	.enable = (int32_t (*)())max_gpio_irq_enable,
	.disable = (int32_t (*)())max_gpio_irq_disable,
	.trigger_level_set = (int32_t (*)())max_gpio_irq_trigger_level_set,
	.global_enable = (int32_t (*)())max_gpio_irq_global_enable,
	.global_disable = (int32_t (*)())max_gpio_irq_global_disable,
	.set_priority = (int32_t (*)())(max_gpio_irq_set_priority),
	.remove = (int32_t (*)())max_gpio_irq_ctrl_remove
};
