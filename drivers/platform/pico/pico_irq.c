/***************************************************************************//**
 *   @file   pico/pico_irq.c
 *   @brief  Implementation of external irq driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
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

#include "no_os_irq.h"
#include "no_os_list.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_timer.h"
#include "no_os_alloc.h"
#include "pico_irq.h"
#include "pico_uart.h"
#include "pico_timer.h"
#include "pico/stdlib.h"
#include "hardware/irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define PICO_IRQ_NB 26u

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct irq_action {
	uint32_t irq_id;
	void *handle;
	void (*callback)(void *context);
	void *ctx;
};

struct event_list {
	enum no_os_irq_event event;
	struct no_os_list_desc *actions;
};

/******************************************************************************/
/************************* Variable Declarations ******************************/
/******************************************************************************/

static bool initialized =  false;
static uint32_t irq_enabled_mask = 0;

static struct event_list _events[] = {
	[NO_OS_EVT_UART_RX_COMPLETE] = {.event = NO_OS_EVT_UART_RX_COMPLETE},
	[NO_OS_EVT_TIM_ELAPSED] = {.event = NO_OS_EVT_TIM_ELAPSED},
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief UART interrupt handler.
 * @param uart - UART instance.
 */
static void _uart_common_handler(uart_inst_t *uart)
{
	int ret;
	struct event_list *evt_list = &_events[NO_OS_EVT_UART_RX_COMPLETE];
	struct irq_action *action;

	uint8_t uart_irq_id = (uart == uart0) ? UART0_IRQ : UART1_IRQ;
	struct irq_action key = {.irq_id = uart_irq_id};

	ret = no_os_list_read_find(evt_list->actions, (void **)&action, &key);
	if (ret)
		return;

	if (action->callback)
		action->callback(action->ctx);
}

/**
 * @brief TIMER interrupt handler.
 * @param uart - TIMER instance.
 */
static void _alarm_callback(uint alarm_num)
{
	int ret;
	struct event_list *evt_list = &_events[NO_OS_EVT_TIM_ELAPSED];
	struct irq_action *action;

	struct irq_action key = {.irq_id = alarm_num};

	ret = no_os_list_read_find(evt_list->actions, (void **)&action, &key);
	if (ret)
		return;

	if (action->callback)
		action->callback(action->ctx);

	/* Trigger alarm again */
	no_os_timer_start(pico_alarm_desc[alarm_num]);
}

/**
 * @brief UART0 RX interrupt handler.
 */
void on_uart0_rx(void)
{
	if (uart_is_readable(uart0))
		_uart_common_handler(uart0);
}

/**
 * @brief UART1 RX interrupt handler.
 */
void on_uart1_rx(void)
{
	if (uart_is_readable(uart1))
		_uart_common_handler(uart1);
}

/**
 * @brief Action comparator function.
 * @param data1 - List element.
 * @param data2 - Key.
 * @return 0 if the two are equal, any other integer otherwise.
 */
int32_t irq_action_cmp(void *data1, void *data2)
{
	return ((struct irq_action *)data1)->irq_id -
	       ((struct irq_action *)data2)->irq_id;
}

/**
 * @brief Initialized the controller for pico external interrupts.
 * @param desc  - Pointer where the configured instance is stored.
 * @param param - Configuration information for the instance.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
			   const struct no_os_irq_init_param *param)
{
	static struct no_os_irq_ctrl_desc *descriptor;
	if (!param)
		return -EINVAL;

	if (!initialized) {
		descriptor = no_os_calloc(1, sizeof(*descriptor));
		if (!descriptor)
			return -ENOMEM;
		initialized = true;
	}

	/* There is only 1 interrupt controller and that is NVIC */
	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->extra = param->extra;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Free the resources allocated by irq_ctrl_init().
 * @param desc - Interrupt controller descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	initialized = false;

	if (desc)
		no_os_free(desc);

	return 0;
}

/**
 * @brief Unused.
 * @param desc   - Irq descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param level  - The trigger condition.
 * @return -ENOSYS
 */
int32_t pico_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
			       uint32_t irq_id,
			       enum no_os_irq_trig_level level)
{
	return -ENOSYS;
}

/**
 * @brief Register a callback.
 * @param desc   - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param cb     - Descriptor of the callback.
 * @return 0 if successful, error code otherwise.
 */
int32_t pico_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
				   uint32_t irq_id,
				   struct no_os_callback_desc *cb)
{
	int ret;
	struct irq_action *action;
	struct irq_action action_key = {.irq_id = irq_id};

	switch (cb->peripheral) {
	case NO_OS_UART_IRQ:
		/* Set up the interrupt handler */
		irq_set_exclusive_handler(irq_id,
					  cb->handle == uart0 ? on_uart0_rx : on_uart1_rx);

		if (_events[cb->event].actions == NULL) {
			ret = no_os_list_init(&_events[cb->event].actions,
					      NO_OS_LIST_PRIORITY_LIST,
					      irq_action_cmp);
			if (ret) {
				irq_remove_handler(irq_id,
						   cb->handle == uart0 ? on_uart0_rx : on_uart1_rx);
				return ret;
			}
		}
		break;
	case NO_OS_TIM_IRQ:
		/* Set up the interrupt handler */
		hardware_alarm_set_callback(irq_id, _alarm_callback);
		/* By default, disable alarm irq.
		The user shall enable the interrupt when seen fit. */
		irq_set_enabled(irq_id, false);

		if (_events[cb->event].actions == NULL) {
			ret = no_os_list_init(&_events[cb->event].actions,
					      NO_OS_LIST_PRIORITY_LIST,
					      irq_action_cmp);
			if (ret) {
				hardware_alarm_set_callback(irq_id, NULL);
				return ret;
			}
		}
		break;
	default:
		return -EINVAL;
	}

	ret = no_os_list_read_find(_events[cb->event].actions,
				   (void **)&action,
				   &action_key);
	/*
	 * If an action with the same irq_id as the function parameter does not exists, insert a new one,
	 * otherwise update
	 */
	if (ret) {
		action = no_os_calloc(1, sizeof(*action));
		if (!action)
			return -ENOMEM;

		action->irq_id = irq_id;
		action->handle = cb->handle;
		action->callback = cb->callback;
		action->ctx = cb->ctx;

		ret = no_os_list_add_last(_events[cb->event].actions, action);
		if (ret) {
			no_os_free(action);
			return ret;
		}
	} else {
		action->irq_id = irq_id;
		action->handle = cb->handle;
		action->callback = cb->callback;
		action->ctx = cb->ctx;
	}

	return 0;
}

/**
 * @brief Unregister a callback.
 * @param desc   - Interrupt controller descriptor.
 * @param irq_id - Id of the interrupt.
 * @param cb     - Descriptor of the callback.
 * @return 0 if successful, error code otherwise.
 */
int32_t pico_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
				     uint32_t irq_id, struct no_os_callback_desc *cb)
{
	int ret;
	void *discard;
	struct irq_action key;

	key.handle = cb->handle;
	ret = no_os_list_get_find(_events[cb->event].actions, &discard, &key);
	if (ret)
		return ret;

	irq_remove_handler(irq_id, NULL);
	no_os_free(discard);

	return 0;
}

/**
 * @brief Enable all interrupts.
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t pico_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	/* Enable all interrupts which were previously enabled */
	irq_set_mask_enabled(irq_enabled_mask, true);

	return 0;
}

/**
 * @brief Disable all interrupts.
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t pico_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	for(uint8_t irq_num = 0; irq_num < PICO_IRQ_NB; irq_num++)
		if (irq_is_enabled(irq_num))
			irq_enabled_mask |= 1 << irq_num;

	/* Disable all interrupts which are already enabled */
	irq_set_mask_enabled(irq_enabled_mask, false);

	return 0;
}

/**
 * @brief Enable a specific interrupt.
 * @param desc   - Interrupt controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return 0 if successful, error code otherwise.
 */
int32_t pico_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (!desc || !(irq_id < PICO_IRQ_NB))
		return -EINVAL;

	irq_set_enabled(irq_id, true);

	return 0;
}

/**
 * @brief Disable a specific interrupt.
 * @param desc   - Interrupt controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return 0 if successful, error code otherwise.
 */
int32_t pico_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (!desc || !(irq_id < PICO_IRQ_NB))
		return -EINVAL;

	irq_set_enabled(irq_id, false);

	return 0;
}

/**
 * @brief Set a priority level for an interrupt.
 * @param desc           - Interrupt controller descriptor.
 * @param irq_id         - The interrupt vector entry id of the peripheral.
 * @param priority_level - The interrupt priority level.
 * @return 0 if successful, error code otherwise.
 */
static int32_t pico_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
				     uint32_t irq_id,
				     uint32_t priority_level)
{
	if (!desc || !(irq_id < PICO_IRQ_NB) || (priority_level > 3))
		return -EINVAL;

	/* Only 4 available priorities for pico 0-3 */
	irq_set_priority(irq_id, priority_level << 6);

	return 0;
}

/**
 * @brief pico specific IRQ platform ops structure
 */
const struct no_os_irq_platform_ops pico_irq_ops = {
	.init = &pico_irq_ctrl_init,
	.trigger_level_set = &pico_trigger_level_set,
	.register_callback = &pico_irq_register_callback,
	.unregister_callback = &pico_irq_unregister_callback,
	.global_enable = &pico_irq_global_enable,
	.global_disable = &pico_irq_global_disable,
	.enable = &pico_irq_enable,
	.disable = &pico_irq_disable,
	.set_priority = &pico_irq_set_priority,
	.remove = &pico_irq_ctrl_remove
};
