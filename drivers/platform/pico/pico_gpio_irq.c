/***************************************************************************//**
 *   @file   pico/pico_gpio_irq.c
 *   @brief  Source file for GPIO IRQ driver.
 *   @author Ramona Bolboaca (ramona.bolboaca@analog.com)
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
#include "no_os_error.h"
#include "no_os_list.h"
#include "no_os_irq.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "pico_gpio_irq.h"
#include "hardware/irq.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Struct used to store a (peripheral, callback) pair
 */
struct irq_action {
	uint32_t irq_id;
	void (*callback)(void *context);
	void *ctx;
};

/******************************************************************************/
/***************************** Static variables *******************************/
/******************************************************************************/
static struct no_os_list_desc *actions;

static int32_t irq_action_cmp(void *data1, void *data2)
{
	return ((struct irq_action *)data1)->irq_id -
	       ((struct irq_action *)data2)->irq_id;
}

static bool initialized = false;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * @brief GPIO interrupt handler callback
 * @param pin pin number on which the interrupt occurred
 * @param events events mask which triggered the interrupt (not used)
 */
void pico_gpio_callback(unsigned int pin, uint32_t events)
{
	int ret;
	struct irq_action *action;
	struct irq_action key;

	key.irq_id = pin;
	ret = no_os_list_read_find(actions, (void **)&action, &key);
	if (ret)
		return;

	if (action->callback)
		action->callback(action->ctx);
}

/**
 * @brief Initialize the GPIO interrupt controller
 * @param desc  - Pointer where the configured instance is stored
 * @param param - Configuration information for the instance
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_gpio_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
				       const struct no_os_irq_init_param *param)
{
	static struct no_os_irq_ctrl_desc *gpio_irq_desc;
	int ret;
	struct pico_gpio_irq_desc *pico_gpio_irq;

	if (!param)
		return -EINVAL;

	if (!initialized) {
		gpio_irq_desc = no_os_calloc(1, sizeof(*gpio_irq_desc));
		if (!gpio_irq_desc)
			return -ENOMEM;

		pico_gpio_irq = (struct pico_gpio_irq_desc*)no_os_calloc(1,
				sizeof(*pico_gpio_irq));
		if (!pico_gpio_irq) {
			ret = -ENOMEM;
			goto error;
		}

		gpio_irq_desc->extra = pico_gpio_irq;
		gpio_irq_desc->irq_ctrl_id = param->irq_ctrl_id;

		ret = no_os_list_init(&actions, NO_OS_LIST_PRIORITY_LIST, irq_action_cmp);
		if (ret)
			goto error;

		initialized = true;
	}

	*desc = gpio_irq_desc;

	return 0;

error:
	no_os_list_remove(actions);
	no_os_free(gpio_irq_desc);
	no_os_free(pico_gpio_irq);
	return ret;
}

/**
 * @brief Free the resources allocated by irq_ctrl_init()
 * @param desc - GPIO interrupt controller descriptor.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_gpio_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	struct no_os_callback_desc *discard;

	if (!desc)
		return -EINVAL;

	while (0 == no_os_list_get_first(actions, (void **)&discard))
		no_os_free(discard);

	no_os_list_remove(actions);

	initialized = false;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Set the trigger condition.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Pin id.
 * @param level  - the trigger condition.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_gpio_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		enum no_os_irq_trig_level level)
{
	struct pico_gpio_irq_desc *pico_gpio_irq;
	enum gpio_irq_level pico_gpio_level;
	if (!desc || !desc->extra || !(irq_id < PICO_GPIO_MAX_PIN_NB))
		return -EINVAL;

	pico_gpio_irq = desc->extra;

	switch (level) {
	case NO_OS_IRQ_EDGE_FALLING:
		pico_gpio_level = GPIO_IRQ_EDGE_FALL;
		break;
	case NO_OS_IRQ_EDGE_RISING:
		pico_gpio_level = GPIO_IRQ_EDGE_RISE;
		break;
	case NO_OS_IRQ_LEVEL_LOW:
		pico_gpio_level = GPIO_IRQ_LEVEL_LOW;
		break;
	case NO_OS_IRQ_LEVEL_HIGH:
		pico_gpio_level = GPIO_IRQ_LEVEL_HIGH;
		break;
	default:
		return -EINVAL;
	}

	pico_gpio_irq->pin_trigger_lvl[irq_id] = pico_gpio_level;

	return 0;
}

/**
 * @brief Register a callback function to be triggered when an
 * interrupt occurs.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Pin id.
 * @param cb     - Descriptor of the callback.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_gpio_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		struct no_os_callback_desc *cb)
{
	int ret;
	struct irq_action *action;
	struct irq_action action_key = {.irq_id = irq_id};
	struct pico_gpio_irq_desc *pico_gpio_irq;

	if (!desc || !desc->extra || !cb  || !(irq_id < PICO_GPIO_MAX_PIN_NB))
		return -EINVAL;

	pico_gpio_irq = desc->extra;

	ret = no_os_list_read_find(actions, (void **)&action, &action_key);
	/* If no action was found, insert a new one, otherwise update it */
	if (ret) {
		action = no_os_calloc(1, sizeof(*action));
		if (!action)
			return -ENOMEM;

		action->irq_id = irq_id;
		action->ctx = cb->ctx;
		action->callback = cb->callback;

		ret = no_os_list_add_last(actions, action);
		if (ret)
			goto free_action;
	} else {
		action->irq_id = irq_id;
		action->ctx = cb->ctx;
		action->callback = cb->callback;
	}

	gpio_set_irq_enabled_with_callback(irq_id,
					   pico_gpio_irq->pin_trigger_lvl[irq_id],
					   false,
					   pico_gpio_callback);
	return 0;

free_action:
	no_os_free(action);
	return ret;
}

/**
 * @brief Unregister a callback.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Pin id.
 * @param cb     - Descriptor of the callback.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_gpio_irq_unregister_callback(
	struct no_os_irq_ctrl_desc *desc,
	uint32_t irq_id,
	struct no_os_callback_desc *cb)
{
	int ret;
	struct irq_action *discard_action = NULL;
	struct irq_action action_key = {.irq_id = desc->irq_ctrl_id};
	struct pico_gpio_irq_desc *pico_gpio_irq;

	if (!desc || !desc->extra || !(irq_id < PICO_GPIO_MAX_PIN_NB))
		return -EINVAL;

	pico_gpio_irq = desc->extra;
	gpio_set_irq_enabled_with_callback(irq_id,
					   pico_gpio_irq->pin_trigger_lvl[irq_id],
					   false,
					   NULL);

	ret = no_os_list_read_find(actions, (void **)&discard_action, &action_key);
	if (ret)
		return -ENODEV;

	no_os_free(discard_action);
	return 0;
}

/**
 * @brief Unused
 * @param desc - GPIO interrupt controller descriptor.
 * @return -ENOSYS
 */
static int32_t pico_gpio_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	return -ENOSYS;
}

/**
 * @brief Unused
 * @param desc - GPIO interrupt controller descriptor.
 * @return -ENOSYS
 */
static int32_t pico_gpio_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	return -ENOSYS;
}

/**
 * @brief Enable a specific gpio interrupt.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Pin id.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_gpio_irq_enable(struct no_os_irq_ctrl_desc *desc,
				    uint32_t irq_id)
{
	struct pico_gpio_irq_desc *pico_gpio_irq;

	if (!desc || !desc->extra || !(irq_id < PICO_GPIO_MAX_PIN_NB))
		return -EINVAL;

	pico_gpio_irq = desc->extra;

	gpio_set_irq_enabled_with_callback(irq_id,
					   pico_gpio_irq->pin_trigger_lvl[irq_id],
					   true,
					   pico_gpio_callback);

	return 0;
}

/**
 * @brief Disable a specific gpio interrupt.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Pin id.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_gpio_irq_disable(struct no_os_irq_ctrl_desc *desc,
				     uint32_t irq_id)
{
	struct pico_gpio_irq_desc *pico_gpio_irq;
	if (!desc || !desc->extra || !(irq_id < PICO_GPIO_MAX_PIN_NB))
		return -EINVAL;

	pico_gpio_irq = desc->extra;

	gpio_set_irq_enabled_with_callback(irq_id,
					   pico_gpio_irq->pin_trigger_lvl[irq_id],
					   false,
					   pico_gpio_callback);

	return 0;
}

/*
 * @brief Set the interrupt priority for the current GPIO pin.
 * @param desc           - GPIO interrupt controller descriptor.
 * @param irq_id         - Pin id.
 * @param priority_level - The interrupt priority level.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_gpio_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		uint32_t priority_level)
{
	if (!desc || !desc->extra || !(irq_id < PICO_GPIO_MAX_PIN_NB)
	    || (priority_level > 3))
		return -EINVAL;

	/* IO_IRQ_BANK0 is ihe interrupt which is used for GPIO */
	/* Only 4 available priorities for pico 0-3 */
	irq_set_priority(IO_IRQ_BANK0, priority_level << 6);

	return 0;
}

/**
 * @brief pico specific IRQ platform ops structure
 */
const struct no_os_irq_platform_ops pico_gpio_irq_ops = {
	.init = &pico_gpio_irq_ctrl_init,
	.trigger_level_set = &pico_gpio_irq_trigger_level_set,
	.register_callback = &pico_gpio_irq_register_callback,
	.unregister_callback = &pico_gpio_irq_unregister_callback,
	.global_enable = &pico_gpio_irq_global_enable,
	.global_disable = &pico_gpio_irq_global_disable,
	.enable = &pico_gpio_irq_enable,
	.disable = &pico_gpio_irq_disable,
	.set_priority = &pico_gpio_irq_set_priority,
	.remove = &pico_gpio_irq_ctrl_remove
};
