/***************************************************************************//**
 *   @file   stm32/stm32_gpio_irq.c
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
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "no_os_list.h"
#include "no_os_irq.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "stm32_gpio_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define STM32_IRQ_CTRL_NB 16

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

static bool initialized[STM32_IRQ_CTRL_NB] =  {false};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * @brief Generic Interrupt handler callback
 * @param pin pin number on which the interrupt occurred (GPIO_PIN_pin)
 */
static inline void stm32_handle_generic_callback(uint16_t pin)
{
	int ret;
	struct irq_action *action;
	struct irq_action key;

	key.irq_id = no_os_find_first_set_bit(pin);

	ret = no_os_list_read_find(actions, (void **)&action, &key);
	if (ret)
		return;

	if (action->callback)
		action->callback(action->ctx);
}

/**
 * @brief EXTI GPIO Interrupt handler callback
 * @param pin pin number on which the interrupt occurred (GPIO_PIN_pin)
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
	stm32_handle_generic_callback(pin);
}

/**
 * @brief EXTI GPIO Interrupt handler callback for rising edge detection
 * @param pin pin number on which the interrupt occurred (GPIO_PIN_pin)
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t pin)
{
	stm32_handle_generic_callback(pin);
}

/**
 * @brief EXTI GPIO Interrupt handler callback for falling edge detection
 * @param pin pin number on which the interrupt occurred (GPIO_PIN_pin)
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t pin)
{
	stm32_handle_generic_callback(pin);
}

/**
 * @brief Initialize the GPIO interrupt controller
 * @param desc  - Pointer where the configured instance is stored
 * @param param - Configuration information for the instance
 * @return 0 in case of success, errno error codes otherwise.
 */
static int32_t stm32_gpio_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
					const struct no_os_irq_init_param *param)
{
	static struct no_os_irq_ctrl_desc *gpio_irq_desc_arr[STM32_IRQ_CTRL_NB];

	int ret;
	struct no_os_irq_ctrl_desc *gpio_irq_desc;
	struct stm32_gpio_irq_desc *sdesc;
	struct stm32_gpio_irq_init_param *gpio_irq_ip;

	if (!param || !param->extra)
		return -EINVAL;

	gpio_irq_ip =  param->extra;

	/* Check port number */
	if (!IS_EXTI_GPIO_PORT(gpio_irq_ip->port_nb))
		return -EINVAL;

	/* Check pin number */
	if (!IS_EXTI_GPIO_PIN(param->irq_ctrl_id))
		return -EINVAL;

	if (!initialized[param->irq_ctrl_id]) {

		gpio_irq_desc = no_os_calloc(1, sizeof(*gpio_irq_desc));
		if (!gpio_irq_desc)
			return -ENOMEM;

		sdesc = (struct stm32_gpio_irq_desc*)no_os_calloc(1, sizeof(*sdesc));
		if (!sdesc) {
			ret = -ENOMEM;
			goto error;
		}
		/* Add port number */
		sdesc->port_nb = gpio_irq_ip->port_nb;

		gpio_irq_desc->extra = sdesc;
		gpio_irq_desc->irq_ctrl_id = param->irq_ctrl_id;

		ret = no_os_list_init(&actions, NO_OS_LIST_PRIORITY_LIST, irq_action_cmp);
		if (ret)
			goto error;

		gpio_irq_desc_arr[param->irq_ctrl_id] = gpio_irq_desc;
		initialized[param->irq_ctrl_id] = true;
	}

	*desc = gpio_irq_desc_arr[param->irq_ctrl_id];

	return 0;
error:
	no_os_list_remove(actions);
	no_os_free(gpio_irq_desc);
	no_os_free(sdesc);

	return ret;
}

/**
 * @brief Free the resources allocated by irq_ctrl_init()
 * @param desc - GPIO interrupt controller descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
static int32_t stm32_gpio_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	struct no_os_callback_desc *discard;

	if (!desc)
		return -EINVAL;

	while (0 == no_os_list_get_first(actions, (void **)&discard))
		no_os_free(discard);

	no_os_list_remove(actions);

	initialized[desc->irq_ctrl_id] = false;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Set the trigger condition.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Not used, pin id is already present in desc.
 * @param level  - the trigger condition.
 * @return 0 in case of success, errno error codes otherwise
 */
static int32_t stm32_gpio_irq_trigger_level_set(struct no_os_irq_ctrl_desc
		*desc,
		uint32_t irq_id,
		enum no_os_irq_trig_level level)
{
	int ret;
	EXTI_ConfigTypeDef config;
	struct stm32_gpio_irq_desc *sdesc;

	if (!desc || !desc->extra || !IS_EXTI_GPIO_PIN(desc->irq_ctrl_id))
		return -EINVAL;

	sdesc = desc->extra;

	ret = HAL_EXTI_GetConfigLine(&sdesc->hexti, &config);
	if (ret != HAL_OK)
		return -EFAULT;

	switch (level) {
	case NO_OS_IRQ_EDGE_FALLING:
		config.Trigger = EXTI_TRIGGER_FALLING;
		break;
	case NO_OS_IRQ_EDGE_RISING:
		config.Trigger = EXTI_TRIGGER_RISING;
		break;
	case NO_OS_IRQ_EDGE_BOTH:
		config.Trigger = EXTI_TRIGGER_RISING_FALLING;
		break;
	default:
		return -EINVAL;
	}

	config.GPIOSel = sdesc->port_nb;
	ret = HAL_EXTI_SetConfigLine(&sdesc->hexti, &config);
	if (ret != HAL_OK)
		return -EFAULT;

	return 0;
}

/**
 * @brief Register a callback function to be triggered when an
 * interrupt occurs.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Not used, pin id is already present in desc.
 * @param cb     - Descriptor of the callback.
 * @return 0 if successful, negative error code otherwise.
 */
static int32_t stm32_gpio_irq_register_callback(struct no_os_irq_ctrl_desc
		*desc,
		uint32_t irq_id,
		struct no_os_callback_desc *cb)
{
	int ret;
	struct irq_action *action;
	struct irq_action action_key = {.irq_id = desc->irq_ctrl_id};
	struct stm32_gpio_irq_desc *sdesc;

	if (!desc || !desc->extra || !cb || !IS_EXTI_GPIO_PIN(desc->irq_ctrl_id))
		return -EINVAL;

	sdesc = desc->extra;

	ret = no_os_list_read_find(actions, (void **)&action, &action_key);
	/*
	* If no action was found, insert a new one, otherwise update it
	*/
	if (ret) {
		action = no_os_calloc(1, sizeof(*action));
		if (!action)
			return -ENOMEM;

		action->irq_id = desc->irq_ctrl_id;
		action->ctx = cb->ctx;
		action->callback = cb->callback;

		ret = no_os_list_add_last(actions, action);
		if (ret)
			goto free_action;
	} else {
		action->irq_id = desc->irq_ctrl_id;
		action->ctx = cb->ctx;
		action->callback = cb->callback;
	}

	EXTI_ConfigTypeDef config;
	config.Mode = EXTI_MODE_INTERRUPT;
	config.GPIOSel = sdesc->port_nb;
	config.Line = EXTI_GPIO | desc->irq_ctrl_id;
	ret = HAL_EXTI_SetConfigLine(&sdesc->hexti, &config);
	if (ret) {
		ret = -EFAULT;
		goto free_action;
	}

	return 0;

free_action:
	no_os_free(action);
	return ret;
}

/**
 * @brief Unregister a callback.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Not used, pin id is already present in desc.
 * @param cb     - Descriptor of the callback.
 * @return 0 if successful, negative error code otherwise.
 */
static int32_t stm32_gpio_irq_unregister_callback(struct no_os_irq_ctrl_desc
		*desc,
		uint32_t irq_id, struct no_os_callback_desc *cb)
{
	int ret;
	struct irq_action *discard_action = NULL;
	struct irq_action action_key = {.irq_id = desc->irq_ctrl_id};

	if (!desc || !cb || !IS_EXTI_GPIO_PIN(desc->irq_ctrl_id))
		return -EINVAL;

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
static int32_t stm32_gpio_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	return -ENOSYS;
}

/**
 * @brief Unused
 * @param desc - GPIO interrupt controller descriptor.
 * @return -ENOSYS
 */
static int32_t stm32_gpio_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	return -ENOSYS;
}

/**
 * @brief Enable a specific gpio interrupt.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Not used, pin id is already present in desc.
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int32_t stm32_gpio_irq_enable(struct no_os_irq_ctrl_desc *desc,
				     uint32_t irq_id)
{
	IRQn_Type nvic_irq_id;
	int ret;

	if (!desc || !desc->extra || !IS_EXTI_GPIO_PIN(desc->irq_ctrl_id))
		return -EINVAL;

	ret = stm32_get_exti_irq_id_from_pin(desc->irq_ctrl_id, &nvic_irq_id);
	if (ret)
		return ret;

	HAL_NVIC_EnableIRQ(nvic_irq_id);

	return 0;
}

/**
 * @brief Disable a specific gpio interrupt.
 * @param desc   - GPIO interrupt controller descriptor.
 * @param irq_id - Not used, pin id is already present in desc.
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int32_t stm32_gpio_irq_disable(struct no_os_irq_ctrl_desc *desc,
				      uint32_t irq_id)
{
	IRQn_Type nvic_irq_id;
	int ret;

	if (!desc || !desc->extra || !IS_EXTI_GPIO_PIN(desc->irq_ctrl_id))
		return -EINVAL;

	ret = stm32_get_exti_irq_id_from_pin(desc->irq_ctrl_id, &nvic_irq_id);
	if (ret)
		return ret;

	HAL_NVIC_DisableIRQ(nvic_irq_id);

	return 0;
}

/*
 * @brief Set the interrupt priority for the current GPIO pin.
 * @param desc           - GPIO interrupt controller descriptor.
 * @param irq_id         - Not used, pin id is already present in desc.
 * @param priority_level - The interrupt priority level.
 * @return 0
 */
static int32_t stm32_gpio_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		uint32_t priority_level)
{
	IRQn_Type nvic_irq_id;
	int ret;

	if (!desc || !desc->extra || !IS_EXTI_GPIO_PIN(desc->irq_ctrl_id))
		return -EINVAL;

	ret = stm32_get_exti_irq_id_from_pin(desc->irq_ctrl_id, &nvic_irq_id);
	if (ret)
		return ret;

	NVIC_SetPriority(nvic_irq_id, priority_level);

	return 0;
}

/*
 * @brief Clear the pending interrupt for the current GPIO pin.
 * @param desc           - GPIO interrupt controller descriptor.
 * @param irq_id         - Not used, pin id is already present in desc.
 * @return 0
 */
static int32_t stm32_irq_clear_pending(struct no_os_irq_ctrl_desc* desc,
				       uint32_t irq_id)
{
	if (!desc || !desc->extra || !IS_EXTI_GPIO_PIN(desc->irq_ctrl_id))
		return -EINVAL;

	if (__HAL_GPIO_EXTI_GET_IT(1 << (desc->irq_ctrl_id)))
		__HAL_GPIO_EXTI_CLEAR_IT(1 << (desc->irq_ctrl_id));

	return 0;
}

/**
 * @brief stm32 specific IRQ platform ops structure
 */
const struct no_os_irq_platform_ops stm32_gpio_irq_ops = {
	.init = &stm32_gpio_irq_ctrl_init,
	.trigger_level_set = &stm32_gpio_irq_trigger_level_set,
	.register_callback = &stm32_gpio_irq_register_callback,
	.unregister_callback = &stm32_gpio_irq_unregister_callback,
	.global_enable = &stm32_gpio_irq_global_enable,
	.global_disable = &stm32_gpio_irq_global_disable,
	.enable = &stm32_gpio_irq_enable,
	.disable = &stm32_gpio_irq_disable,
	.set_priority = &stm32_gpio_irq_set_priority,
	.remove = &stm32_gpio_irq_ctrl_remove,
	.clear_pending = &stm32_irq_clear_pending
};
