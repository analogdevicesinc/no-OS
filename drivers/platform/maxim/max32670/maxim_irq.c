/***************************************************************************//**
 *   @file   maxim_irq.c
 *   @brief  Implementation of external irq driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "rtc.h"
#include "uart.h"
#include "tmr.h"
#include "maxim_irq.h"
#include "max32670.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

static struct event_list _events[] = {
	[NO_OS_EVT_GPIO] = {.event = NO_OS_EVT_GPIO},
	[NO_OS_EVT_UART_TX_COMPLETE] = {.event = NO_OS_EVT_UART_TX_COMPLETE},
	[NO_OS_EVT_UART_RX_COMPLETE] = {.event = NO_OS_EVT_UART_RX_COMPLETE},
	[NO_OS_EVT_UART_ERROR] = {.event = NO_OS_EVT_UART_ERROR},
	[NO_OS_EVT_RTC] = {.event = NO_OS_EVT_RTC},
	[NO_OS_EVT_TIM_ELAPSED] = {.event = NO_OS_EVT_TIM_ELAPSED},
};

static struct no_os_irq_ctrl_desc *nvic;
extern mxc_uart_req_t uart_irq_state[MXC_UART_INSTANCES];
extern bool is_callback;

/**
 * @brief Action comparator function
 * @param data1 - List element
 * @param data2 - Key
 * @return 0 if the two are equal, any other integer otherwise
 */
int32_t irq_action_cmp(void *data1, void *data2)
{
	return ((struct irq_action *)data1)->irq_id -
	       ((struct irq_action *)data2)->irq_id;
}

/**
 * @brief Check if the irq_id is for a GPIO interrupt
 * @param irq_id - The interrupt vector entry id.
 * @return true/false
 */
static bool is_gpio_irq_id(uint32_t irq_id)
{
	uint32_t i;

	for (i = 0; i < MXC_CFG_GPIO_INSTANCES; i++) {
		if (irq_id == MXC_GPIO_GET_IRQ(i))
			return true;
	}

	return false;
}

/**
 * @brief Timer interrupt routine that further calls a registed callback
 * function
 * @param tmr - timer registers struct (HAL defined)
 */
static void _timer_common_callback(mxc_tmr_regs_t *tmr)
{
	int ret;
	struct irq_action key = {.irq_id = MXC_TMR_GET_IRQ(MXC_TMR_GET_IDX(tmr))};
	struct irq_action *action;
	struct event_list *evt_list = &_events[NO_OS_EVT_TIM_ELAPSED];

	ret = no_os_list_read_find(evt_list->actions, (void **)&action, &key);
	if (ret)
		return;

	if (action->callback)
		action->callback(action->ctx);

	MXC_TMR_ClearFlags(tmr);
}

/**
 * @brief UART interrupt handler
 * @param uart - uart registers struct (HAL defined)
 */
static void _uart_common_handler(mxc_uart_regs_t *uart)
{
	mxc_uart_req_t *req = &uart_irq_state[MXC_UART_GET_IDX(uart)];
	MXC_UART_AsyncHandler(uart);
	/**
	* A new nonblocking transaction was registered in a callback.
	* This step is necessary because the AsyncHandler function
	* unregisters all the async transactions (including those that
	* were made in the callback function).
	*/
	if (req->uart && ((req->rxCnt == 0 && req->rxLen != 0) || (req->txLen != 0)))
		MXC_UART_TransactionAsync(req);
}

void UART0_IRQHandler()
{
	_uart_common_handler(MXC_UART0);
}

#ifdef MXC_UART1
void UART1_IRQHandler()
{
	_uart_common_handler(MXC_UART1);
}
#endif

#ifdef MXC_UART2
void UART2_IRQHandler()
{
	_uart_common_handler(MXC_UART2);
}
#endif

#ifdef MXC_UART3
void UART3_IRQHandler()
{
	_uart_common_handler(MXC_UART3);
}
#endif

#ifdef MXC_TMR0
void TMR0_IRQHandler()
{
	_timer_common_callback(MXC_TMR0);
}
#endif

#ifdef MXC_TMR1
void TMR1_IRQHandler()
{
	_timer_common_callback(MXC_TMR1);
}
#endif

#ifdef MXC_TMR2
void TMR2_IRQHandler()
{
	_timer_common_callback(MXC_TMR2);
}
#endif

void RTC_IRQHandler()
{
	int ret;
	uint32_t flags = MXC_RTC_GetFlags();
	struct event_list *evt_list = &_events[NO_OS_EVT_RTC];
	struct irq_action *action;

	if (flags & MXC_RTC_INT_FL_LONG) {
		MXC_RTC_ClearFlags(MXC_RTC_INT_FL_LONG);
		ret = no_os_list_read_first(evt_list->actions, (void **)&action);
		if (ret < 0)
			return;

		if (action->callback)
			action->callback(action->ctx);
	}
}

/**
 * @brief UART callback function that sets the event and further calls
 * the user registered callback
 * @param req - UART request struct
 * @param result - status of the request (error code)
 */
void max_uart_callback(mxc_uart_req_t *req, int result)
{
	struct event_list *ee;
	struct irq_action *a;
	uint32_t uart_id = MXC_UART_GET_IDX(req->uart);
	struct irq_action key = {.irq_id = MXC_UART_GET_IRQ(uart_id)};
	int ret;

	if (result)
		ee = &_events[NO_OS_EVT_UART_ERROR];
	else if (req->txLen == req->txCnt && req->txLen != 0)
		ee = &_events[NO_OS_EVT_UART_TX_COMPLETE];
	else if (req->rxLen == req->rxCnt && req->rxLen != 0)
		ee = &_events[NO_OS_EVT_UART_RX_COMPLETE];
	else
		return;

	ret = no_os_list_read_find(ee->actions, (void **)&a, &key);
	if (ret)
		return;

	uart_irq_state[uart_id].uart = NULL;
	if (a->callback) {
		is_callback = true;
		a->callback(a->ctx);
		is_callback = false;
	}
}

/**
 * @brief Initialize the interrupt controller
 * @param desc - Pointer where the configured instance is stored
 * @param param - Configuration information for the instance
 * @return 0 in case of success, errno error codes otherwise.
 */
int max_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
		      const struct no_os_irq_init_param *param)
{
	struct no_os_irq_ctrl_desc *descriptor;

	if (!param)
		return -EINVAL;

	if (nvic) {
		*desc = nvic;
		return 0;
	}

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->extra = param->extra;

	*desc = descriptor;
	nvic = descriptor;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_irq_ctrl_init()
 * @param desc - Interrupt controller descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int max_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	uint32_t i;
	void *discard;
	if (!desc)
		return -EINVAL;

	for (i = 0; i < NO_OS_ARRAY_SIZE(_events); i++) {
		while (0 == no_os_list_get_first(_events[i].actions, &discard))
			no_os_free(discard);
		no_os_list_remove(_events[i].actions);
		_events[i].actions = NULL;
	}
	no_os_free(desc);
	nvic = NULL;

	return 0;
}

/**
 * @brief Register a callback function to be triggered when an
 * interrupt occurs.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param callback_desc - Descriptor of the callback.
 * @return 0 in case of success, errno error codes otherwise
 */
int max_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
			      uint32_t irq_id,
			      struct no_os_callback_desc *callback_desc)
{
	int ret;
	struct irq_action *action;
	struct irq_action action_key = {.irq_id = irq_id};

	if (is_gpio_irq_id(irq_id))
		return -ENOSYS;

	if (!desc || !callback_desc)
		return -EINVAL;

	switch (callback_desc->peripheral) {
	case NO_OS_UART_IRQ:
		if (_events[callback_desc->event].actions == NULL) {
			ret = no_os_list_init(&_events[callback_desc->event].actions,
					      NO_OS_LIST_PRIORITY_LIST,
					      irq_action_cmp);
			if (ret)
				return ret;
		}

		ret = no_os_list_read_find(_events[callback_desc->event].actions,
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
			action->handle = callback_desc->handle;
			action->callback = callback_desc->callback;
			action->ctx = callback_desc->ctx;

			ret = no_os_list_add_last(_events[callback_desc->event].actions, action);
			if (ret)
				goto free_action;
		} else {
			action->irq_id = irq_id;
			action->handle = callback_desc->handle;
			action->callback = callback_desc->callback;
			action->ctx = callback_desc->ctx;
		}
		break;
	case NO_OS_RTC_IRQ:
		if (_events[NO_OS_EVT_RTC].actions == NULL) {
			ret = no_os_list_init(&_events[NO_OS_EVT_RTC].actions, NO_OS_LIST_PRIORITY_LIST,
					      irq_action_cmp);
			if (ret)
				return ret;
		}

		/*
		 * This is a special case for RTC on Maxim platform. Since there is only 1 RTC peripheral, there should
		 * be only 1 registered callback at a time.
		 */
		ret = no_os_list_read_first(_events[NO_OS_EVT_RTC].actions, (void **)&action);
		if (ret) {
			action = no_os_calloc(1, sizeof(*action));
			if (!action)
				return -ENOMEM;

			action->irq_id = irq_id;
			action->handle = callback_desc->handle;
			action->callback = callback_desc->callback;
			action->ctx = callback_desc->ctx;

			ret = no_os_list_add_first(_events[NO_OS_EVT_RTC].actions, action);
			if (ret)
				goto free_action;

		} else {
			action->irq_id = irq_id;
			action->handle = callback_desc->handle;
			action->callback = callback_desc->callback;
			action->ctx = callback_desc->ctx;
		}

		ret = MXC_RTC_EnableInt(MXC_RTC_INT_EN_LONG);
		if (ret)
			return -EBUSY;
		break;
	case NO_OS_TIM_IRQ:
		if (_events[NO_OS_EVT_TIM_ELAPSED].actions == NULL) {
			ret = no_os_list_init(&_events[NO_OS_EVT_TIM_ELAPSED].actions,
					      NO_OS_LIST_PRIORITY_LIST,
					      irq_action_cmp);
			if (ret)
				return ret;
		}

		ret = no_os_list_read_find(_events[callback_desc->event].actions,
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
			action->handle = callback_desc->handle;
			action->callback = callback_desc->callback;
			action->ctx = callback_desc->ctx;

			ret = no_os_list_add_last(_events[callback_desc->event].actions, action);
			if (ret)
				goto free_action;

		} else {
			action->irq_id = irq_id;
			action->handle = callback_desc->handle;
			action->callback = callback_desc->callback;
			action->ctx = callback_desc->ctx;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;

free_action:
	no_os_free(action);
	return ret;
}

/**
 * @brief Unregister a callback function
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param cb - Callback descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int max_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
				uint32_t irq_id, struct no_os_callback_desc *cb)
{
	int ret;
	void *discard_action = NULL;
	struct irq_action action_key = {.irq_id = irq_id};

	if (is_gpio_irq_id(irq_id))
		return -ENOSYS;

	if (!desc || !cb)
		return -EINVAL;

	switch (cb->peripheral) {
	case NO_OS_UART_IRQ:
		action_key.handle = cb->handle;
		break;
	case NO_OS_RTC_IRQ:
		action_key.handle = MXC_RTC;
		MXC_RTC_DisableInt(MXC_RTC_INT_EN_LONG);
		break;
	default:
		break;
	}

	ret = no_os_list_get_find(_events[cb->event].actions, &discard_action,
				  &action_key);
	if (ret)
		return -ENODEV;

	no_os_free(discard_action);

	return ret;
}

/**
 * @brief Unused.
 * @param desc -irq descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param trig_l - the trigger condition.
 * @return -ENOSYS
 */
int max_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
			      uint32_t irq_id,
			      enum no_os_irq_trig_level trig_l)
{
	return -ENOSYS;
}

/**
 * @brief Enable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int max_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	__enable_irq();

	return 0;
}

/**
 * @brief Disable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int max_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	__disable_irq();

	return 0;
}

/**
 * @brief Enable a specific interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @return 0 in case of success, errno error codes otherwise.
 */
int max_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	NVIC_EnableIRQ(irq_id);

	return 0;
}

/**
 * @brief Disable a specific interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_irq_disable(struct no_os_irq_ctrl_desc *desc,
		    uint32_t irq_id)
{
	if (irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	NVIC_DisableIRQ(irq_id);

	return 0;
}

/**
 * @brief Set a priority level for an interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param priority_level - The interrupt priority level
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int max_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
				uint32_t irq_id,
				uint32_t priority_level)
{
	if (irq_id >= MXC_IRQ_EXT_COUNT)
		return -EINVAL;

	NVIC_SetPriority(irq_id, priority_level);

	return 0;
}

/**
 * @brief maxim specific IRQ platform ops structure
 */
const struct no_os_irq_platform_ops max_irq_ops = {
	.init = &max_irq_ctrl_init,
	.register_callback = &max_irq_register_callback,
	.unregister_callback = &max_irq_unregister_callback,
	.trigger_level_set = &max_irq_trigger_level_set,
	.global_enable = &max_irq_global_enable,
	.global_disable = &max_irq_global_disable,
	.enable = &max_irq_enable,
	.disable = &max_irq_disable,
	.set_priority = &max_irq_set_priority,
	.remove = &max_irq_ctrl_remove
};
