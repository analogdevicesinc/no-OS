/***************************************************************************//**
 *   @file   stm32/stm32_irq.c
 *   @brief  Implementation of external irq driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#include "stm32_irq.h"

struct irq_action {
	void *handle;
	void (*callback)(void *context);
	void *ctx;
};

struct event_list {
	enum no_os_irq_event event;
	uint32_t hal_event;
	struct no_os_list_desc *actions;
};

static bool initialized =  false;

static struct event_list _events[] = {
	[NO_OS_EVT_GPIO] = {.event = NO_OS_EVT_GPIO, .hal_event = HAL_EXTI_COMMON_CB_ID},
	[NO_OS_EVT_UART_TX_COMPLETE] = {.event = NO_OS_EVT_UART_TX_COMPLETE, .hal_event = HAL_UART_TX_COMPLETE_CB_ID},
	[NO_OS_EVT_UART_RX_COMPLETE] = {.event = NO_OS_EVT_UART_RX_COMPLETE, .hal_event = HAL_UART_RX_COMPLETE_CB_ID},
	[NO_OS_EVT_UART_ERROR] = {.event = NO_OS_EVT_UART_ERROR, .hal_event = HAL_UART_ERROR_CB_ID},
#ifdef HAL_TIM_MODULE_ENABLED
	[NO_OS_EVT_TIM_ELAPSED] = {.event = NO_OS_EVT_TIM_ELAPSED, .hal_event = HAL_TIM_PERIOD_ELAPSED_CB_ID},
	[NO_OS_EVT_TIM_PWM_PULSE_FINISHED] = {.event = NO_OS_EVT_TIM_PWM_PULSE_FINISHED, .hal_event = HAL_TIM_PWM_PULSE_FINISHED_CB_ID},
#endif
#ifdef HAL_DMA_MODULE_ENABLED
	[NO_OS_EVT_DMA_RX_COMPLETE] = {.event = NO_OS_EVT_DMA_RX_COMPLETE, .hal_event = HAL_DMA_XFER_CPLT_CB_ID},
	[NO_OS_EVT_DMA_RX_HALF_COMPLETE] = {.event = NO_OS_EVT_DMA_RX_HALF_COMPLETE, .hal_event = HAL_DMA_XFER_HALFCPLT_CB_ID},
	[NO_OS_EVT_DMA_TX_COMPLETE] = {.event = NO_OS_EVT_DMA_TX_COMPLETE, .hal_event = HAL_DMA_XFER_CPLT_CB_ID},
#endif
};

static int32_t irq_action_cmp(void *data1, void *data2)
{
	return (int32_t)((struct irq_action *)data1)->handle -
	       (int32_t)((struct irq_action *)data2)->handle;
}

#ifdef HAL_TIM_MODULE_ENABLED
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
	struct event_list *ee = &_events[NO_OS_EVT_TIM_ELAPSED];
	struct irq_action *a;
	struct irq_action key = {.handle = htim};
	int ret;

	/* Find & call callback */
	ret = no_os_list_read_find(ee->actions, (void **)&a, &key);
	if (ret < 0)
		return;

	if(a->callback)
		a->callback(a->ctx);
}

void HAL_TIM_PWM_PulseFinishedCallback (TIM_HandleTypeDef *htim)
{
	struct event_list *ee = &_events[NO_OS_EVT_TIM_PWM_PULSE_FINISHED];
	struct irq_action *a;
	struct irq_action key = {.handle = htim};
	int ret;

	/* Find & call callback */
	ret = no_os_list_read_find(ee->actions, (void **)&a, &key);
	if (ret < 0)
		return;

	if(a->callback)
		a->callback(a->ctx);
}
#endif

static inline void _common_uart_callback(UART_HandleTypeDef *huart,
		uint32_t no_os_event)
{
	struct event_list *ue = &_events[no_os_event];
	struct irq_action *a;
	struct irq_action key = {.handle = huart};
	int ret;
	ret = no_os_list_read_find(ue->actions, (void **)&a, &key);
	if (ret < 0)
		return;

	if(a->callback)
		a->callback(a->ctx);
}

#if defined (HAL_SAI_MODULE_ENABLED)
static inline void _common_sai_dma_callback(SAI_HandleTypeDef *hsai,
		uint32_t no_os_event)
{
	struct event_list *ue = &_events[no_os_event];
	struct irq_action *a;
	struct irq_action key = {.handle = hsai};
	int ret;
	ret = no_os_list_read_find(ue->actions, (void **)&a, &key);
	if (ret < 0)
		return;

	if(a->callback)
		a->callback(a->ctx);
}
#endif

#if defined (HAL_DMA_MODULE_ENABLED)
static inline void _common_dma_callback(DMA_HandleTypeDef *hdma,
					uint32_t no_os_event)
{
	struct event_list *ue = &_events[no_os_event];
	struct irq_action *a;
	struct irq_action key = {.handle = hdma};
	int ret;
	ret = no_os_list_read_find(ue->actions, (void **)&a, &key);
	if (ret < 0)
		return;

	if(a->callback)
		a->callback(a->ctx);
}
#endif

// equivalent of HAL_UART_TxCpltCallback
void _TxCpltCallback(UART_HandleTypeDef *huart)
{
	_common_uart_callback(huart, NO_OS_EVT_UART_TX_COMPLETE);
}

// equivalent of HAL_UART_RxCpltCallback
void _RxCpltCallback(UART_HandleTypeDef *huart)
{
	_common_uart_callback(huart, NO_OS_EVT_UART_RX_COMPLETE);
}

#if defined (HAL_SAI_MODULE_ENABLED)
void _SAIRxCpltCallback(SAI_HandleTypeDef *hsai)
{
	_common_sai_dma_callback(hsai, NO_OS_EVT_DMA_RX_COMPLETE);
}

void _SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
	_common_sai_dma_callback(hsai, NO_OS_EVT_DMA_RX_HALF_COMPLETE);
}
#endif

#if defined (HAL_DMA_MODULE_ENABLED)
void _DMA_RX_CpltCallback(DMA_HandleTypeDef* hdma)
{
	_common_dma_callback(hdma, NO_OS_EVT_DMA_RX_COMPLETE);
}

void _DMA_TX_CpltCallback(DMA_HandleTypeDef* hdma)
{
	_common_dma_callback(hdma, NO_OS_EVT_DMA_TX_COMPLETE);
}

void _DMA_HalfCpltCallback(DMA_HandleTypeDef *hdma)
{
	_common_dma_callback(hdma, NO_OS_EVT_DMA_RX_HALF_COMPLETE);
}
#endif

// equivalent of HAL_UART_ErrorCallback
void _ErrorCallback(UART_HandleTypeDef *huart)
{
	_common_uart_callback(huart, NO_OS_EVT_UART_ERROR);
}

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialized the controller for the STM32 external interrupts
 * @param desc - Pointer where the configured instance is stored
 * @param param - Configuration information for the instance
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t stm32_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
			    const struct no_os_irq_init_param *param)
{
	static struct no_os_irq_ctrl_desc *descriptor;
	if (!param)
		return -EINVAL;

	if (!initialized) {
		descriptor = no_os_calloc(1, sizeof(*descriptor));
		if (!descriptor)
			return -ENOMEM;
	}

	// unused, there is only 1 interrupt controller and that is NVIC
	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->extra = param->extra;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Free the resources allocated by irq_ctrl_init()
 * @param desc - Interrupt controller descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t stm32_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	initialized = false;

	if (desc)
		no_os_free(desc);

	return 0;
}

/**
 * @brief Unused.
 * @param desc -irq descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param level - the trigger condition.
 * @return -ENOSYS
 */
int32_t stm32_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
				uint32_t irq_id,
				enum no_os_irq_trig_level level)
{
	return -ENOSYS;
}

/**
 * @brief Register a callback.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param cb - Descriptor of the callback.
 * @return 0 if successfull, negative error code otherwise.
 */
int32_t stm32_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
				    uint32_t irq_id,
				    struct no_os_callback_desc *cb)
{
	int ret;
	pUART_CallbackTypeDef pUartCallback;
#ifdef HAL_SAI_MODULE_ENABLED
	pSAI_CallbackTypeDef pSaiDmaCallback;
#endif
#ifdef HAL_TIM_MODULE_ENABLED
	pTIM_CallbackTypeDef pTimCallback;
#endif
	struct irq_action action_key = {.handle = cb->handle};
#ifdef HAL_DMA_MODULE_ENABLED
	DMA_HandleTypeDef pDmaCallback;
#endif
	struct irq_action *li;
	uint32_t hal_event = _events[cb->event].hal_event;

	switch (cb->peripheral) {
	case NO_OS_UART_IRQ:
		switch(hal_event) {
		case HAL_UART_TX_COMPLETE_CB_ID:
			pUartCallback = _TxCpltCallback;
			break;
		case HAL_UART_RX_COMPLETE_CB_ID:
			pUartCallback = _RxCpltCallback;
			break;
		case HAL_UART_ERROR_CB_ID:
			pUartCallback = _ErrorCallback;
			break;
		default:
			return -EINVAL;
		};

		ret = HAL_UART_RegisterCallback(cb->handle, hal_event, pUartCallback);
		if (ret != HAL_OK)
			return -EFAULT;

		break;
#ifdef HAL_TIM_MODULE_ENABLED
	case NO_OS_TIM_IRQ:
		switch(hal_event) {
		case HAL_TIM_PWM_PULSE_FINISHED_CB_ID:
			pTimCallback = HAL_TIM_PWM_PulseFinishedCallback;
			break;
		case HAL_TIM_PERIOD_ELAPSED_CB_ID:
			pTimCallback = HAL_TIM_PeriodElapsedCallback;
			break;
		default:
			return -EINVAL;
		};

		ret = HAL_TIM_RegisterCallback(cb->handle, hal_event, pTimCallback);
		if (ret != HAL_OK)
			return -EFAULT;
		break;
#endif
#if defined(HAL_DMA_MODULE_ENABLED) && defined(HAL_SAI_MODULE_ENABLED)
	case NO_OS_TDM_DMA_IRQ:
		switch(hal_event) {
		case HAL_DMA_XFER_CPLT_CB_ID:
			pSaiDmaCallback = _SAIRxCpltCallback;
			ret = HAL_SAI_RegisterCallback(cb->handle, hal_event, pSaiDmaCallback);
			if (ret != HAL_OK)
				return -EFAULT;

			break;
		case HAL_DMA_XFER_HALFCPLT_CB_ID:
			pSaiDmaCallback = _SAI_RxHalfCpltCallback;
			ret = HAL_SAI_RegisterCallback(cb->handle, hal_event, pSaiDmaCallback);
			if (ret != HAL_OK)
				return -EFAULT;
			break;
		}
		break;
#endif
#if defined (HAL_TIM_MODULE_ENABLED) && defined(HAL_DMA_MODULE_ENABLED)
	case NO_OS_TIM_DMA_IRQ:
		switch(hal_event) {
		case HAL_DMA_XFER_CPLT_CB_ID:
			pDmaCallback.XferCpltCallback = _DMA_RX_CpltCallback;
			ret = HAL_DMA_RegisterCallback(cb->handle, hal_event,
						       pDmaCallback.XferCpltCallback);
			if (ret != HAL_OK)
				return -EFAULT;
			break;
		case HAL_DMA_XFER_HALFCPLT_CB_ID:
			pDmaCallback.XferHalfCpltCallback = _DMA_HalfCpltCallback;
			ret = HAL_DMA_RegisterCallback(cb->handle, hal_event,
						       pDmaCallback.XferHalfCpltCallback);
			if (ret != HAL_OK)
				return -EFAULT;
			break;
		default:
			return -EINVAL;
		};
		break;
#endif
#if defined (HAL_DMA_MODULE_ENABLED)
	case NO_OS_DMA_IRQ:
		switch (hal_event) {
		case HAL_DMA_XFER_CPLT_CB_ID:
			if (cb->event == NO_OS_EVT_DMA_RX_COMPLETE)
				pDmaCallback.XferCpltCallback = _DMA_RX_CpltCallback;

			else
				pDmaCallback.XferCpltCallback = _DMA_TX_CpltCallback;

			ret = HAL_DMA_RegisterCallback((DMA_HandleTypeDef *)cb->handle, hal_event,
						       pDmaCallback.XferCpltCallback);
			if (ret != HAL_OK)
				return -EFAULT;

			break;

		default:
			return -EINVAL;
		};

		break;
#endif

	default:
		return -EINVAL;
	}

	if (_events[cb->event].actions == NULL) {
		ret = no_os_list_init(&_events[cb->event].actions, NO_OS_LIST_PRIORITY_LIST,
				      irq_action_cmp);
		if (ret < 0)
			return ret;
	}
	/*
	 * If an action with the same handle as the function parameter does not exists, insert a new one,
	 * otherwise update
	 */
	ret = no_os_list_read_find(_events[cb->event].actions,
				   (void**)&li,
				   &action_key);
	if (ret) {
		li = no_os_calloc(1, sizeof(struct irq_action));
		if (!li)
			return -ENOMEM;

		li->handle = cb->handle;
		li->callback = cb->callback;
		li->ctx = cb->ctx;
		ret = no_os_list_add_last(_events[cb->event].actions, li);
		if (ret < 0) {
			no_os_free(li);
			return ret;
		}
	} else {
		li->handle = cb->handle;
		li->callback = cb->callback;
		li->ctx = cb->ctx;
	}

	return 0;
}

/**
 * @brief Unregister a callback.
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Id of the interrupt
 * @param cb - Descriptor of the callback.
 * @return 0 if successfull, negative error code otherwise.
 */
int32_t stm32_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
				      uint32_t irq_id, struct no_os_callback_desc *cb)
{
	int ret;
	void *discard  = NULL;
	struct irq_action key;
	uint32_t hal_event = _events[cb->event].hal_event;

	switch (cb->peripheral) {
	case NO_OS_UART_IRQ:
		key.handle = cb->handle;
		ret = no_os_list_get_find(_events[cb->event].actions, &discard, &key);
		if (ret < 0)
			break;
		ret = HAL_UART_UnRegisterCallback(cb->handle, hal_event);
		if (ret != HAL_OK)
			ret = -EFAULT;
		break;
#ifdef HAL_TIM_MODULE_ENABLED
	case NO_OS_TIM_IRQ:
		key.handle = cb->handle;
		ret = no_os_list_get_find(_events[cb->event].actions, &discard, &key);
		if (ret < 0)
			break;
		ret = HAL_TIM_UnRegisterCallback(cb->handle, hal_event);
		if (ret != HAL_OK)
			ret = -EFAULT;
		break;
#endif
#if defined(HAL_DMA_MODULE_ENABLED) && defined(HAL_SAI_MODULE_ENABLED)
	case NO_OS_TDM_DMA_IRQ:
		key.handle = cb->handle;
		ret = no_os_list_get_find(_events[cb->event].actions, &discard, &key);
		if (ret < 0)
			break;
		ret = HAL_SAI_UnRegisterCallback(cb->handle, hal_event);
		if (ret != HAL_OK)
			ret = -EFAULT;
		break;
#endif
#if defined (HAL_TIM_MODULE_ENABLED) && defined(HAL_DMA_MODULE_ENABLED)
	case NO_OS_TIM_DMA_IRQ:
	case NO_OS_DMA_IRQ:
		key.handle = cb->handle;
		ret = no_os_list_get_find(_events[cb->event].actions, &discard, &key);
		if (ret < 0)
			break;
		ret = HAL_DMA_UnRegisterCallback(cb->handle, hal_event);
		if (ret != HAL_OK)
			ret = -EFAULT;
		break;
#endif
	default:
		ret = -EINVAL;
		break;
	}

	if (discard)
		no_os_free(discard);

	return ret;
}

/**
 * @brief Enable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t stm32_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	__enable_irq();

	return 0;
}

/**
 * @brief Disable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t stm32_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	__disable_irq();

	return 0;
}

/**
 * @brief Enable a specific interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Interrupt identifier
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t stm32_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	NVIC_EnableIRQ(irq_id);

	return 0;
}

/**
 * @brief Disable a specific interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Interrupt identifier
 * @return 0 in case of success, -EINVAL otherwise.
 */
int32_t stm32_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	NVIC_DisableIRQ(irq_id);

	return 0;
}

/**
 * @brief Set a priority level for an interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - The interrupt vector entry id of the peripheral.
 * @param priority_level - The interrupt priority level
 * @return 0
 */
static int32_t stm32_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
				      uint32_t irq_id,
				      uint32_t priority_level)
{
	HAL_NVIC_SetPriority(irq_id, priority_level, 0);

	return 0;
}

/**
 * @brief stm32 specific IRQ platform ops structure
 */
const struct no_os_irq_platform_ops stm32_irq_ops = {
	.init = &stm32_irq_ctrl_init,
	.trigger_level_set = &stm32_trigger_level_set,
	.register_callback = &stm32_irq_register_callback,
	.unregister_callback = &stm32_irq_unregister_callback,
	.global_enable = &stm32_irq_global_enable,
	.global_disable = &stm32_irq_global_disable,
	.enable = &stm32_irq_enable,
	.disable = &stm32_irq_disable,
	.set_priority = &stm32_irq_set_priority,
	.remove = &stm32_irq_ctrl_remove
};
