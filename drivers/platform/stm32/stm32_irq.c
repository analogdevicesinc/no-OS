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
#include "no-os/irq.h"
#include "no-os/util.h"
#include "stm32_irq.h"

EXTI_HandleTypeDef *hexti[32];

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_EXTI_IRQHandler(hexti[find_first_set_bit(GPIO_Pin)]);
}

struct ecc {
	uint32_t event;
	void (*callback)(void *context);
	void *context;
	void *handle;
};

static struct ecc uart_callbacks[] = {
		{.event = HAL_UART_TX_HALFCOMPLETE_CB_ID},
		{.event = HAL_UART_TX_COMPLETE_CB_ID},
		{.event = HAL_UART_RX_HALFCOMPLETE_CB_ID},
		{.event = HAL_UART_RX_COMPLETE_CB_ID},
};

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	struct ecc *c = &uart_callbacks[HAL_UART_TX_COMPLETE_CB_ID];
	if(c->handle != huart)
		return;

	if(c->callback)
		c->callback(c->context);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	struct ecc *c = &uart_callbacks[HAL_UART_RX_COMPLETE_CB_ID];
	if(c->handle != huart)
		return;

	if(c->callback)
		c->callback(c->context);
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
int32_t stm32_irq_ctrl_init(struct irq_ctrl_desc **desc,
			    const struct irq_init_param *param)
{
	struct irq_ctrl_desc *descriptor;

	if (!param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	// unused, there is only 1 interrupt controller and that is NVIC
	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->platform_ops = param->platform_ops;
	descriptor->extra = param->extra;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Free the resources allocated by irq_ctrl_init()
 * @param desc - Interrupt controller descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t stm32_irq_ctrl_remove(struct irq_ctrl_desc *desc)
{
	if (desc)
		free(desc);

	return 0;
}

/**
 * @brief Set the EXTI line trigger level.
 * @param desc - gpio irq descriptor.
 * @param irq_id - the pin number
 * @param level - the trigger condition.
 * @return -ENOSYS
 */
int32_t stm32_trigger_level_set(struct irq_ctrl_desc *desc,
				uint32_t irq_id,
				enum irq_trig_level level)
{
	int ret;
	EXTI_ConfigTypeDef config;
	ret = HAL_EXTI_GetConfigLine(desc->extra, &config);
	if (ret != HAL_OK)
		return -EFAULT;

	switch (level) {
	case IRQ_EDGE_FALLING:
		config.Trigger = EXTI_TRIGGER_FALLING;
		break;
	case IRQ_EDGE_RISING:
		config.Trigger = EXTI_TRIGGER_RISING;
		break;
	case IRQ_EDGE_BOTH:
		config.Trigger = EXTI_TRIGGER_RISING_FALLING;
		break;
	default:
		return -EINVAL;
	}

	ret = HAL_EXTI_SetConfigLine(desc->extra, &config);
	if (ret != HAL_OK)
		return -EFAULT;

	return 0;
}

/**
 * @brief Register a callback.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param callback - Descriptor of the callback.
 * @return -ENOSYS
 */
int32_t stm32_irq_register_callback(struct irq_ctrl_desc *desc, uint32_t irq_id,
				    struct irq_callback *cb)
{
	int ret;

	switch (cb->peripheral) {
	case NO_OS_GPIO_IRQ:
		ret = HAL_EXTI_RegisterCallback(desc->extra, cb->event, cb->callback);
		if (ret != HAL_OK) {
			ret = -EFAULT;
			break;
		}
		hexti[((EXTI_HandleTypeDef *)desc->extra)->Line & 0xff] = desc->extra;
		break;
	case NO_OS_UART_IRQ:
		if (cb->event > HAL_UART_WAKEUP_CB_ID) {
			ret = -EINVAL;
			break;
		}

		uart_callbacks[cb->event].callback = cb->callback;
		uart_callbacks[cb->event].context = cb->context;
		uart_callbacks[cb->event].handle = desc->extra;
		break;


	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

/**
 * @brief Unregister a callback.
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Id of the interrupt
 * @param callback - Descriptor of the callback.
 * @return -ENOSYS
 */
int32_t stm32_irq_unregister_callback(struct irq_ctrl_desc *desc,
				      uint32_t irq_id, struct irq_callback *cb)
{
	int ret;

	switch (cb->peripheral) {
	case NO_OS_GPIO_IRQ:
		hexti[((EXTI_HandleTypeDef *)desc->extra)->Line & 0xff] = NULL;
		break;
	case NO_OS_UART_IRQ:
		ret = HAL_UART_UnRegisterCallback(desc->extra, cb->event);
		if (ret != HAL_OK)
			ret = -EFAULT;
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

/**
 * @brief Enable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t stm32_irq_global_enable(struct irq_ctrl_desc *desc)
{
	__enable_irq();

	return 0;
}

/**
 * @brief Disable all interrupts
 * @param desc - Interrupt controller descriptor.
 * @return 0
 */
int32_t stm32_irq_global_disable(struct irq_ctrl_desc *desc)
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
int32_t stm32_irq_enable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	// TODO: add a proper priority setting function in irq.h instead of this default
	HAL_NVIC_SetPriority(irq_id, 0, 0);

	NVIC_EnableIRQ(irq_id);

	return 0;
}

/**
 * @brief Disable a specific interrupt
 * @param desc - Interrupt controller descriptor.
 * @param irq_id - Interrupt identifier
 * @return 0 in case of success, -EINVAL otherwise.
 */
int32_t stm32_irq_disable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	NVIC_DisableIRQ(irq_id);

	return 0;
}

/**
 * @brief stm32 specific IRQ platform ops structure
 */
const struct irq_platform_ops stm32_irq_ops = {
	.init = &stm32_irq_ctrl_init,
	.trigger_level_set = &stm32_trigger_level_set,
	.register_callback = &stm32_irq_register_callback,
	.unregister_callback = &stm32_irq_unregister_callback,
	.global_enable = &stm32_irq_global_enable,
	.global_disable = &stm32_irq_global_disable,
	.enable = &stm32_irq_enable,
	.disable = &stm32_irq_disable,
	.remove = &stm32_irq_ctrl_remove
};
