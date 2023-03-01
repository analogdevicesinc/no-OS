/***************************************************************************//**
 *   @file   aducm3029/aducm3029_irq.c
 *   @brief  Implementation of External IRQ driver for ADuCM302x.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#include "aducm3029_irq.h"
#include "no_os_error.h"
#include <stdlib.h>
#include "no_os_uart.h"
#include "aducm3029_uart.h"
#include "no_os_rtc.h"
#include "aducm3029_rtc.h"
#include "no_os_gpio.h"
#include <drivers/gpio/adi_gpio.h>
#include <drivers/tmr/adi_tmr.h>
#include "no_os_timer.h"
#include "aducm3029_timer.h"
#include "no_os_util.h"
#include "no_os_list.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** The number of the first external interrupt, used by NVIC */
#define BASE_XINT_NB			(XINT_EVT0_IRQn)

/** Number of available external interrupts */
#define NB_EXT_INTERRUPTS		4u

/** Number of interrupts controllers available */
#define NB_INTERRUPT_CONTROLLERS	1u

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
 * @brief Struct that stores all the actions for a specific event
 */
struct event_list {
	enum no_os_irq_event event;
	struct no_os_list_desc *actions;
	uint32_t hal_event;
};

static struct event_list _events[] = {
	[NO_OS_EVT_GPIO] = {.event = NO_OS_EVT_GPIO},
	[NO_OS_EVT_UART_TX_COMPLETE] = {.event = NO_OS_EVT_UART_TX_COMPLETE, .hal_event = UART_EVT_IRQn},
	[NO_OS_EVT_UART_RX_COMPLETE] = {.event = NO_OS_EVT_UART_RX_COMPLETE, .hal_event = UART_EVT_IRQn},
	[NO_OS_EVT_UART_ERROR] = {.event = NO_OS_EVT_UART_ERROR, .hal_event = UART_EVT_IRQn},
	[NO_OS_EVT_RTC] {.event = NO_OS_EVT_RTC},
	[NO_OS_EVT_TIM_ELAPSED] = {.event = NO_OS_EVT_TIM_ELAPSED, .hal_event = TMR1_EVT_IRQn},
};

/**
 * @brief Call the user defined callback when a read/write operation completed.
 * @param ctx:		ADuCM3029 specific descriptor for the UART device
 * @param event:	Event ID from ADI_UART_EVENT
 * @param buff:		Pointer to the handled buffer or to an error code
 */
static void aducm_uart_callback(void *ctx, uint32_t event, void *buff)
{
	struct no_os_aducm_uart_desc	*extra = ctx;
	uint32_t		len;
	struct irq_action *action;

	switch(event) {
	/* Read done */
	case ADI_UART_EVENT_RX_BUFFER_PROCESSED:
		if (extra->read_desc.pending) {
			len = no_os_min(extra->read_desc.pending, NO_OS_UART_MAX_BYTES);
			extra->read_desc.pending -= len;
			adi_uart_SubmitRxBuffer(
				(ADI_UART_HANDLE const)extra->uart_handler,
				(void *const)extra->read_desc.buff,
				(uint32_t const)len,
				len > 4 ? true : false);
			extra->read_desc.buff += len;
		} else {
			extra->read_desc.is_nonblocking = false;
			no_os_list_read_last(_events[NO_OS_EVT_UART_RX_COMPLETE].actions,
					     (void **)&action);
			if (action)
				action->callback(action->ctx);
		}
		break;
	/* Write done */
	case ADI_UART_EVENT_TX_BUFFER_PROCESSED:
		if (extra->write_desc.pending) {
			len = no_os_min(extra->write_desc.pending, NO_OS_UART_MAX_BYTES);
			extra->write_desc.pending -= len;
			adi_uart_SubmitTxBuffer(
				(ADI_UART_HANDLE const)extra->uart_handler,
				(void *const)extra->write_desc.buff,
				(uint32_t const)len,
				len > 4 ? true : false);
			extra->write_desc.buff += len;
		} else {
			extra->write_desc.is_nonblocking = false;
			no_os_list_read_last(_events[NO_OS_EVT_UART_TX_COMPLETE].actions,
					     (void **)&action);
			if (action)
				action->callback(action->ctx);
		}
		break;
	default:
		extra->errors |= (uint32_t)buff;
		extra->read_desc.is_nonblocking = false;
		extra->write_desc.is_nonblocking = false;
		no_os_list_read_last(_events[NO_OS_EVT_UART_ERROR].actions,
				     (void **)&action);
		if (action)
			action->callback(action->ctx);
		break;
	}
}

/**
 * @brief Call the user defined callback when a read/write operation completed.
 * @param ctx:		Not used here. Present to keep function signature.
 * @param event:	Not used here. Present to keep function signature.
 * @param buff:		Not used here. Present to keep function signature.
 */
static void aducm_rtc_callback(void *ctx, uint32_t event, void *buff)
{
	struct irq_action *action;

	no_os_list_read_last(_events[NO_OS_EVT_RTC].actions,
			     (void **)&action);
	if (action)
		action->callback(action->ctx);
}

/**
 * @brief Call the user defined callback when the timer has elapsed.
 * @param ctx:		Not used here. Present to keep function signature.
 * @param event:	Not used here. Present to keep function signature.
 * @param buff:		Not used here. Present to keep function signature.
 */
static void aducm_timer_callback(void *ctx, uint32_t event, void *buff)
{
	struct irq_action *action;
	if (event == ADI_TMR_EVENT_TIMEOUT) {
		no_os_list_read_last(_events[NO_OS_EVT_TIM_ELAPSED].actions,
				     (void **)&action);
		if (action)
			action->callback(action->ctx);
	}
}

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialized the controller for the ADuCM3029 external interrupts
 *
 * @param desc - Pointer where the configured instance is stored
 * @param param - Configuration information for the instance
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
				const struct no_os_irq_init_param *param)
{
	struct aducm_irq_ctrl_desc *aducm_desc;

	if (!desc || !param)
		return -1;

	*desc = (struct no_os_irq_ctrl_desc *)no_os_calloc(1, sizeof(**desc));
	if (!*desc)
		return -1;
	aducm_desc = (struct aducm_irq_ctrl_desc *)
		     no_os_calloc(1, sizeof(*aducm_desc));
	if (!aducm_desc) {
		no_os_free(*desc);
		*desc = NULL;
		return -1;
	}

	(*desc)->extra = aducm_desc;
	(*desc)->irq_ctrl_id = param->irq_ctrl_id;

	return 0;
}

/**
 * @brief Free the resources allocated by \ref no_os_irq_ctrl_init()
 * @param desc - Interrupt controller descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	if (!desc || !desc->extra)
		return -1;

	no_os_irq_unregister_callback(desc, ADUCM_UART_INT_ID, NULL);
	no_os_irq_unregister_callback(desc, ADUCM_RTC_INT_ID, NULL);
	no_os_irq_unregister_callback(desc, ADUCM_TIMER1_INT_ID, NULL);
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Registers a IRQ callback function to irq controller.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param callback_desc - Descriptor of the callback. If it is NULL, the
 * callback will be unregistered
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
					uint32_t irq_id,
					struct no_os_callback_desc *callback_desc)
{
	struct no_os_aducm_uart_desc	*aducm_uart;
	struct no_os_rtc_desc			*rtc_desc;
	struct aducm_rtc_desc		*rtc_extra;
	struct no_os_timer_desc			*timer_desc;
	struct aducm_timer_desc		*timer_extra;
	int32_t				ret;
	struct irq_action	*action;
	int32_t i;

	if (!desc || !desc->extra ||  irq_id >= NB_INTERRUPTS)
		return -1;

	if (!callback_desc)
		return -EINVAL;

	switch (irq_id) {
	case ADUCM_UART_INT_ID:
		aducm_uart = callback_desc->handle;
		for (i = NO_OS_EVT_UART_TX_COMPLETE; i <= NO_OS_EVT_UART_ERROR; i++)
			if (!no_os_list_get_last(_events[callback_desc->event].actions,
						 (void **)&action))
				break;
		if (i > NO_OS_EVT_UART_ERROR)
			adi_uart_RegisterCallback(aducm_uart->uart_handler,
						  aducm_uart_callback, callback_desc->handle);

		if (_events[callback_desc->event].actions == NULL) {
			ret = no_os_list_init(&_events[callback_desc->event].actions,
					      NO_OS_LIST_PRIORITY_LIST,
					      irq_action_cmp);
			if (ret)
				return ret;
		}

		break;
	case ADUCM_RTC_INT_ID:
		rtc_desc = callback_desc->handle;
		rtc_extra = rtc_desc->extra;
		if (_events[callback_desc->event].actions == NULL) {
			ret = no_os_list_init(&_events[callback_desc->event].actions,
					      NO_OS_LIST_PRIORITY_LIST,
					      irq_action_cmp);
			if (ret)
				return ret;
			adi_rtc_RegisterCallback(rtc_extra->instance, aducm_rtc_callback,
						 callback_desc->handle);
		}

		break;
	case ADUCM_TIMER1_INT_ID:
		timer_desc = callback_desc->handle;
		timer_extra = timer_desc->extra;
		if (_events[callback_desc->event].actions == NULL) {
			ret = no_os_list_init(&_events[callback_desc->event].actions,
					      NO_OS_LIST_PRIORITY_LIST,
					      irq_action_cmp);
			if (ret)
				return ret;
			/* Init function is called again to register the needed callback.
			   This implementation can be changed in the future if adi_tmr_RegisterCallback will be available. */
			adi_tmr_Init(timer_desc->id, aducm_timer_callback, callback_desc->handle,
				     false);

			while (ADI_TMR_DEVICE_BUSY == adi_tmr_ConfigTimer(timer_desc->id,
					&timer_extra->tmr_conf));

		}

		break;
	default:
		return -1;
	}

	ret = no_os_list_read_last(_events[callback_desc->event].actions,
				   (void **)&action);
	if (ret) {
		action = no_os_calloc(1, sizeof(*action));
		if (!action)
			return -ENOMEM;

		action->irq_id = callback_desc->event;
		action->handle = callback_desc->handle;
		action->callback = callback_desc->callback;
		action->ctx = callback_desc->ctx;

		ret = no_os_list_add_last(_events[callback_desc->event].actions,
					  action);
		if (ret)
			goto free_action;
	} else {
		action->irq_id = callback_desc->event;
		action->handle = callback_desc->handle;
		action->callback = callback_desc->callback;
		action->ctx = callback_desc->ctx;
	}

	return 0;
free_action:
	no_os_free(action);

	return ret;
}

/**
 * @brief Unregister IRQ handling function for the specified <em>irq_id</em>.
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Id of the interrupt
 * @param cb - Callback descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, struct no_os_callback_desc *cb)
{
	struct irq_action			*action;
	uint32_t					ret;
	int i;
	struct no_os_aducm_uart_desc	*aducm_uart;

	if (!desc || !desc->extra || irq_id >= NB_INTERRUPTS)
		return -1;

	aducm_uart = cb->handle;

	ret = no_os_list_get_last(_events[cb->event].actions, (void **)&action);
	if (ret)
		return ret;

	ret = no_os_list_remove(_events[cb->event].actions);
	if (ret)
		return ret;

	_events[cb->event].actions = NULL;

	for (i = NO_OS_EVT_UART_TX_COMPLETE; i <= NO_OS_EVT_UART_ERROR; i++)
		if (!no_os_list_get_last(_events[cb->event].actions, (void **)&action))
			break;
	if (i > NO_OS_EVT_UART_ERROR)
		adi_uart_RegisterCallback(aducm_uart->uart_handler, NULL, NULL);

	return 0;
}

/**
 * @brief Enable all previously enabled interrupts by \ref no_os_irq_enable().
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t aducm3029_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	if (!desc || !desc->extra)
		return -1;

	no_os_irq_enable(desc, ADUCM_UART_INT_ID);
	no_os_irq_enable(desc, ADUCM_RTC_INT_ID);
	no_os_irq_enable(desc, ADUCM_TIMER1_INT_ID);

	return 0;
}

/**
 * @brief Disable all external interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t aducm3029_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	if (!desc || !desc->extra)
		return -1;

	no_os_irq_disable(desc, ADUCM_UART_INT_ID);
	no_os_irq_disable(desc, ADUCM_RTC_INT_ID);
	no_os_irq_disable(desc, ADUCM_TIMER1_INT_ID);

	return 0;
}

/**
 * @brief Enable the interrupt
 *
 * In order for the <em>irq_id</em> interrupt to be triggered the GPIO
 * associated pin must be set as input.\n
 * The associated values are:\n
 * - ID 0 -> GPIO15
 * - ID 1 -> GPIO16
 * - ID 2 -> GPIO13
 * - ID 3 -> GPIO33
 *
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Id of the interrupt
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_irq_enable(struct no_os_irq_ctrl_desc *desc,
			     uint32_t irq_id)
{
	struct no_os_rtc_desc		*rtc_desc;
	struct aducm_rtc_desc		*aducm_rtc;
	struct irq_action			*action;
	int32_t						ret;

	if (!desc || !desc->extra || irq_id >= NB_INTERRUPTS)
		return -1;

	switch (irq_id) {
	case ADUCM_UART_INT_ID:
		NVIC_EnableIRQ(UART_EVT_IRQn);
		break;
	case ADUCM_RTC_INT_ID:
		ret = no_os_list_read_last(_events[NO_OS_EVT_RTC].actions,
					   (void **)&action);
		if (ret)
			return ret;

		rtc_desc = action->handle;
		aducm_rtc = rtc_desc->extra;
		adi_rtc_EnableInterrupts(aducm_rtc->instance, RTC_COUNT_ROLLOVER_INT, true);
		break;

	case ADUCM_TIMER1_INT_ID:
		NVIC_EnableIRQ(TMR1_EVT_IRQn);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Disable the interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Id of the interrupt
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_irq_disable(struct no_os_irq_ctrl_desc *desc,
			      uint32_t irq_id)
{
	struct no_os_rtc_desc		*rtc_desc;
	struct aducm_rtc_desc		*aducm_rtc;
	struct irq_action			*action;
	int32_t						ret;

	if (!desc || !desc->extra || irq_id >= NB_INTERRUPTS)
		return -1;

	switch (irq_id) {
	case ADUCM_UART_INT_ID:
		NVIC_DisableIRQ(UART_EVT_IRQn);
		break;
	case ADUCM_RTC_INT_ID:
		ret = no_os_list_read_last(_events[NO_OS_EVT_RTC].actions,
					   (void **)&action);
		if (ret)
			return ret;

		rtc_desc = action->handle;
		aducm_rtc = rtc_desc->extra;
		adi_rtc_EnableInterrupts(aducm_rtc->instance, RTC_COUNT_INT, false);
		break;
	case ADUCM_TIMER1_INT_ID:
		NVIC_DisableIRQ(TMR1_EVT_IRQn);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set a priority level for an interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param priority_level - The interrupt priority level
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int32_t aducm3029_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id,
		uint32_t priority_level)
{
	if (irq_id >= NVIC_INTS)
		return -EINVAL;

	NVIC_SetPriority(irq_id, priority_level);

	return 0;
}

/**
 * @brief Aducm3029 platform specific IRQ platform ops structure
 */
const struct no_os_irq_platform_ops aducm_irq_ops = {
	.init = &aducm3029_irq_ctrl_init,
	.register_callback = &aducm3029_irq_register_callback,
	.unregister_callback = &aducm3029_irq_unregister_callback,
	.global_enable = &aducm3029_irq_global_enable,
	.global_disable = &aducm3029_irq_global_disable,
	.enable = &aducm3029_irq_enable,
	.disable = &aducm3029_irq_disable,
	.set_priority = &aducm3029_irq_set_priority,
	.remove = &aducm3029_irq_ctrl_remove
};
