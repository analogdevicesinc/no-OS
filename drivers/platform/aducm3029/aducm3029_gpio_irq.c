/***************************************************************************//**
 *   @file   aducm3029_gpio_irq.c
 *   @brief  Source file for GPIO IRQ driver.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
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

#include <errno.h>
#include <drivers/gpio/adi_gpio.h>
#include <stdlib.h>
#include <stdint.h>

#include "aducm3029_gpio.h"
#include "aducm3029_gpio_irq.h"
#include "aducm3029_irq.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_list.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Call the user defined callback when a read/write operation completed.
 * @param ctx:		ADuCM3029 specific descriptor for the GPIO device
 * @param event:	Not used here. Present to keep function signature.
 * @param pins:		Pointer to the active pins memory location
 */
static void aducm_gpio_callback(void *ctx, uint32_t event, void *pins)
{
	int ret;
	struct irq_action *action;
	struct irq_action key;
	struct aducm_gpio_irq_ctrl_desc *extra = ctx;
	uint16_t *pinints = pins;

	while (*pinints) {
		key.irq_id = no_os_find_first_set_bit((uint32_t)*pinints);
		if (key.irq_id == 32)
			break;
		*pinints &= ~NO_OS_BIT(key.irq_id);
		ret = no_os_list_read_find(extra->actions, (void **)&action, &key);
		if (ret)
			continue;

		if (action)
			action->callback(action->ctx);
	}
}

/**
 * @brief Call the user defined callback when a read/write operation completed.
 * @param ctx:		ADuCM3029 specific descriptor for the IRQ device
 * @param event:	Event ID from ADI_UART_EVENT
 * @param buff:		Not used here. Present to keep function signature.
 */
static void aducm_xint_callback(void *ctx, uint32_t event, void *buff)
{
	int ret;
	struct irq_action *action;
	struct irq_action key;
	struct aducm_gpio_irq_ctrl_desc *extra = ctx;

	key.irq_id = event;
	ret = no_os_list_read_find(extra->actions, (void **)&action, &key);
	if (ret)
		return;

	if (action)
		action->callback(action->ctx);
}

/**
 * @brief Initialize the GPIO interrupt controller
 * @param desc - Pointer where the configured instance is stored
 * @param param - Configuration information for the instance
 * @return 0 in case of success, errno error codes otherwise.
 */
static int aducm_gpio_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
				    const struct no_os_irq_init_param *param)
{
	int ret;
	struct no_os_irq_ctrl_desc *descriptor;
	struct aducm_gpio_irq_ctrl_desc *extra;

	if (!param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	extra = no_os_calloc(1, sizeof(*extra));
	if (!extra)
		goto error_dev;

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->extra = extra;

	ret = no_os_list_init(&extra->actions, NO_OS_LIST_PRIORITY_LIST,
			      irq_action_cmp);
	if (ret)
		goto error_extra;

	switch (descriptor->irq_ctrl_id) {
	case ADUCM_XINT_SOFT_CTRL:
		adi_xint_Init(extra->irq_memory, ADI_XINT_MEMORY_SIZE);

		adi_xint_RegisterCallback(ADI_XINT_EVENT_INT0, aducm_xint_callback,
					  descriptor->extra);
		adi_xint_RegisterCallback(ADI_XINT_EVENT_INT1, aducm_xint_callback,
					  descriptor->extra);
		adi_xint_RegisterCallback(ADI_XINT_EVENT_INT2, aducm_xint_callback,
					  descriptor->extra);
		adi_xint_RegisterCallback(ADI_XINT_EVENT_INT3, aducm_xint_callback,
					  descriptor->extra);
		break;
	case ADUCM_GPIO_A_GROUP_SOFT_CTRL:
		adi_gpio_RegisterCallback(ADI_GPIO_INTA_IRQ, aducm_gpio_callback,
					  descriptor->extra);
		break;
	case ADUCM_GPIO_B_GROUP_SOFT_CTRL:
		adi_gpio_RegisterCallback(ADI_GPIO_INTB_IRQ, aducm_gpio_callback,
					  descriptor->extra);
		break;
	default:
		goto error_extra;
	}

	*desc = descriptor;

	return 0;

error_extra:
	no_os_free(extra);
error_dev:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_irq_ctrl_init()
 * @param desc - Interrupt GPIO controller descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
static int aducm_gpio_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	struct irq_action *discard;
	struct aducm_gpio_irq_ctrl_desc *extra = desc->extra;

	if (!desc)
		return -EINVAL;

	switch (desc->irq_ctrl_id) {
	case ADUCM_XINT_SOFT_CTRL:
		for (int i = ADI_XINT_EVENT_INT0; i < ADI_XINT_EVENT_RESERVED; i++)
			adi_xint_DisableIRQ(i);
		adi_xint_UnInit();
		break;
	case ADUCM_GPIO_A_GROUP_SOFT_CTRL:
		adi_gpio_RegisterCallback(ADI_GPIO_INTA_IRQ, NULL, NULL);
		for (int i = ADI_GPIO_PORT0; i < ADI_GPIO_NUM_PORTS; i++)
			adi_gpio_SetGroupInterruptPins(i, ADI_GPIO_INTA_IRQ, 0);
		break;
	case ADUCM_GPIO_B_GROUP_SOFT_CTRL:
		for (int i = ADI_GPIO_PORT0; i < ADI_GPIO_NUM_PORTS; i++)
			adi_gpio_SetGroupInterruptPins(i, ADI_GPIO_INTB_IRQ, 0);
		break;
	default:
		return -EINVAL;
	}

	while (0 == no_os_list_get_last(extra->actions, (void **)&discard))
		no_os_free(discard);

	no_os_list_remove(extra->actions);
	no_os_free(desc->extra);
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
static int aducm_gpio_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		struct no_os_callback_desc *callback_desc)
{
	int ret, stat;
	struct irq_action *action;
	struct irq_action action_key = {.irq_id = irq_id};
	struct aducm_gpio_irq_ctrl_desc *extra = desc->extra;
	uint16_t gpio_pin;
	uint8_t gpio_port = PORT(irq_id);
	int8_t id = (desc->irq_ctrl_id == ADUCM_GPIO_A_GROUP_SOFT_CTRL) ?
		    ADI_GPIO_INTA_IRQ :
		    ADI_GPIO_INTB_IRQ;

	if (!desc || !callback_desc)
		return -EINVAL;

	ret = no_os_list_read_find(extra->actions, (void **)&action, &action_key);
	/*
	* If no action was found, insert a new one, otherwise update it
	*/
	if (ret) {
		action = no_os_calloc(1, sizeof(*action));
		if (!action)
			return -ENOMEM;

		action->irq_id = irq_id;
		action->handle = callback_desc->handle;
		action->ctx = callback_desc->ctx;
		action->callback = callback_desc->callback;

		stat = no_os_list_add_last(extra->actions, action);
		if (stat)
			goto free_action;

		if (desc->irq_ctrl_id != ADUCM_XINT_SOFT_CTRL) {
			stat = adi_gpio_GetGroupInterruptPins(gpio_port, id, &gpio_pin);
			if (stat)
				goto free_action;
			gpio_pin |= PIN(irq_id);
			stat = adi_gpio_SetGroupInterruptPins(gpio_port, id, gpio_pin);
			if (stat)
				goto free_action;
		}
	} else {
		action->irq_id = irq_id;
		action->handle = callback_desc->handle;
		action->ctx = callback_desc->ctx;
		action->callback = callback_desc->callback;
	}

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
static int aducm_gpio_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		struct no_os_callback_desc *callback_desc)
{
	int ret;
	struct irq_action *discard_action = NULL;
	struct irq_action action_key = {.irq_id = irq_id};
	struct aducm_gpio_irq_ctrl_desc *extra = desc->extra;
	uint16_t gpio_pin;
	uint8_t gpio_port = PORT(irq_id);
	int8_t id = (desc->irq_ctrl_id == ADUCM_GPIO_A_GROUP_SOFT_CTRL) ?
		    ADI_GPIO_INTA_IRQ :
		    ADI_GPIO_INTB_IRQ;

	if (!desc || !callback_desc)
		return -EINVAL;

	ret = no_os_list_get_find(extra->actions, (void **)&discard_action,
				  &action_key);
	if (ret)
		return -ENODEV;

	if (desc->irq_ctrl_id != ADUCM_XINT_SOFT_CTRL) {
		ret = adi_gpio_GetGroupInterruptPins(gpio_port, id, &gpio_pin);
		if (ret)
			return ret;
		gpio_pin |= PIN(irq_id);
		ret = adi_gpio_SetGroupInterruptPins(gpio_port, id, gpio_pin);
		if (ret)
			return ret;
	}

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
static int aducm_gpio_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		enum no_os_irq_trig_level trig)
{
	int ret;
	struct irq_action *action;
	struct irq_action action_key = {.irq_id = irq_id};
	struct aducm_gpio_irq_ctrl_desc *extra = desc->extra;
	uint16_t gpio_pin;
	uint8_t gpio_port = PORT(irq_id);

	if (!desc)
		return -EINVAL;

	ret = no_os_list_read_find(extra->actions, (void **)&action, &action_key);
	if (ret)
		return -ENODEV;

	if (desc->irq_ctrl_id != ADUCM_XINT_SOFT_CTRL) {
		if (trig < NO_OS_IRQ_EDGE_FALLING || trig > NO_OS_IRQ_EDGE_RISING)
			return -EINVAL;

		ret = adi_gpio_GetGroupInterruptPolarity(gpio_port, &gpio_pin);
		if (ret)
			return ret;
		gpio_pin &= ~PIN(irq_id);
		gpio_pin |= (PIN(irq_id) * (trig - NO_OS_IRQ_EDGE_FALLING));
		ret = adi_gpio_SetGroupInterruptPolarity(gpio_port, gpio_pin);
		if (ret)
			return ret;
	}

	action->trig_lv = trig;

	return 0;
}

/**
 * @brief Enable a specific interrupt
 * @param desc - the GPIO irq descriptor.
 * @param irq_id - In case of XINT, values ranging from ADI_XINT_EVENT_INT0 to ADI_XINT_EVENT_INT3,
 *                 otherwise, not used.
 * @return 0 in case of success, errno error codes otherwise.
 */
static int aducm_gpio_irq_enable(struct no_os_irq_ctrl_desc *desc,
				 uint32_t irq_id)
{
	int ret;
	struct irq_action *action;
	struct irq_action action_key = {.irq_id = irq_id};
	struct aducm_gpio_irq_ctrl_desc *extra = desc->extra;
	int8_t id = (desc->irq_ctrl_id == ADUCM_GPIO_A_GROUP_SOFT_CTRL) ?
		    ADI_GPIO_INTA_IRQ :
		    ADI_GPIO_INTB_IRQ;

	if (!desc)
		return -EINVAL;

	if (desc->irq_ctrl_id == ADUCM_XINT_SOFT_CTRL) {
		if(irq_id > ADI_XINT_EVENT_INT3)
			return -EINVAL;

		ret = no_os_list_read_find(extra->actions, (void **)&action,
					   &action_key);
		if (ret)
			return -ENODEV;

		switch(action->trig_lv) {
		case NO_OS_IRQ_LEVEL_LOW:
			return adi_xint_EnableIRQ(irq_id, ADI_XINT_IRQ_LOW_LEVEL);
		case NO_OS_IRQ_LEVEL_HIGH:
			return adi_xint_EnableIRQ(irq_id, ADI_XINT_IRQ_HIGH_LEVEL);
		case NO_OS_IRQ_EDGE_FALLING:
			return adi_xint_EnableIRQ(irq_id, ADI_XINT_IRQ_FALLING_EDGE);
		case NO_OS_IRQ_EDGE_RISING:
			return adi_xint_EnableIRQ(irq_id, ADI_XINT_IRQ_RISING_EDGE);
		case NO_OS_IRQ_EDGE_BOTH:
			return adi_xint_EnableIRQ(irq_id, ADI_XINT_IRQ_EITHER_EDGE);
		default:
			return -EINVAL;
		}
	}
	NVIC_EnableIRQ(id);


	return 0;
}

/**
 * @brief Disable a specific interrupt
 * @param desc - the GPIO irq descriptor.
 * @param irq_id - In case of XINT, values ranging from ADI_XINT_EVENT_INT0 to ADI_XINT_EVENT_INT3,
 *                 otherwise, not used.
 * @return 0 in case of success, errno error codes otherwise.
 */
static int aducm_gpio_irq_disable(struct no_os_irq_ctrl_desc *desc,
				  uint32_t irq_id)
{
	int8_t id = (desc->irq_ctrl_id == ADUCM_GPIO_A_GROUP_SOFT_CTRL) ?
		    ADI_GPIO_INTA_IRQ :
		    ADI_GPIO_INTB_IRQ;

	if (!desc)
		return -EINVAL;

	if (desc->irq_ctrl_id == ADUCM_XINT_SOFT_CTRL) {
		if(irq_id > ADI_XINT_EVENT_INT3)
			return -EINVAL;
		return adi_xint_DisableIRQ(irq_id);
	}

	NVIC_DisableIRQ(id);

	return 0;
}

/**
 * @brief Enable all interrupts
 * @param desc - GPIO interrupt controller descriptor.
 * @return 0
 */
static int aducm_gpio_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	int i, ret;

	if (desc->irq_ctrl_id != ADUCM_XINT_SOFT_CTRL)
		return aducm_gpio_irq_enable(desc, 0);

	for (i = ADI_XINT_EVENT_INT0; i < ADI_XINT_EVENT_RESERVED; i++) {
		ret = aducm_gpio_irq_enable(desc, i);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Disable all interrupts
 * @param desc - GPIO interrupt controller descriptor.
 * @return 0
 */
static int aducm_gpio_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	int i, ret;

	if (desc->irq_ctrl_id != ADUCM_XINT_SOFT_CTRL)
		aducm_gpio_irq_disable(desc, 0);

	for (i = ADI_XINT_EVENT_INT0; i < ADI_XINT_EVENT_RESERVED; i++) {
		ret = aducm_gpio_irq_disable(desc, i);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Set the interrupt priority for the current GPIO port.
 * @param desc - GPIO interrupt controller descriptor.
 * @param irq_id - In case of XINT, values ranging from ADI_XINT_EVENT_INT0 to ADI_XINT_EVENT_INT3,
 *                 otherwise, not used.
 * @param priority_level - The interrupt priority level.
 * @return 0
 */
static int aducm_gpio_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
				       uint32_t irq_id,
				       uint32_t priority_level)
{
	switch (desc->irq_ctrl_id) {
	case ADUCM_XINT_SOFT_CTRL:
		/* available values for irq-id are 0-3 equivalent to ADI_XINT_EVENT_INT0-ADI_XINT_EVENT_INT3 */
		switch(irq_id) {
		case ADI_XINT_EVENT_INT0:
			NVIC_SetPriority(XINT_EVT0_IRQn, priority_level);
			break;
		case ADI_XINT_EVENT_INT1:
			NVIC_SetPriority(XINT_EVT1_IRQn, priority_level);
			break;
		case ADI_XINT_EVENT_INT2:
			NVIC_SetPriority(XINT_EVT2_IRQn, priority_level);
			break;
		case ADI_XINT_EVENT_INT3:
			NVIC_SetPriority(XINT_EVT3_IRQn, priority_level);
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADUCM_GPIO_A_GROUP_SOFT_CTRL:
		NVIC_SetPriority(ADI_GPIO_INTA_IRQ, priority_level);
		break;
	case ADUCM_GPIO_B_GROUP_SOFT_CTRL:
		NVIC_SetPriority(ADI_GPIO_INTB_IRQ, priority_level);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief maxim specific GPIO IRQ platform ops structure
 */
const struct no_os_irq_platform_ops aducm_gpio_irq_ops = {
	.init = (int32_t (*)())aducm_gpio_irq_ctrl_init,
	.register_callback = (int32_t (*)())aducm_gpio_irq_register_callback,
	.unregister_callback = (int32_t (*)())aducm_gpio_irq_unregister_callback,
	.enable = (int32_t (*)())aducm_gpio_irq_enable,
	.disable = (int32_t (*)())aducm_gpio_irq_disable,
	.trigger_level_set = (int32_t (*)())aducm_gpio_irq_trigger_level_set,
	.global_enable = (int32_t (*)())aducm_gpio_irq_global_enable,
	.global_disable = (int32_t (*)())aducm_gpio_irq_global_disable,
	.set_priority = (int32_t (*)())(aducm_gpio_irq_set_priority),
	.remove = (int32_t (*)())aducm_gpio_irq_ctrl_remove
};
