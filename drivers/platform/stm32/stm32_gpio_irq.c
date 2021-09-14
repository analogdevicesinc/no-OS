/***************************************************************************//**
 *   @file   stm32/stm32_gpio_irq.c
 *   @brief  Implementation of stm32 GPIO IRQ functionality.
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
#include <stdlib.h>
#include <errno.h>
#include "no-os/util.h"
#include "no-os/irq.h"
#include "stm32_gpio_irq.h"

/**
* @struct stm32_gpio_irq_callback_desc
* @brief Structure holding the callback functions for STM32 gpio irqs
* @note  The callback functions are mapped with 'gpio_irq_id' structure
*/
typedef struct stm32_gpio_irq_callback_desc {
	uint16_t GPIO_Pin;
	struct callback_desc desc;
} stm32_gpio_irq_callback_desc[NB_GPIO_INTERRUPTS];

stm32_gpio_irq_callback_desc stm32_gpio_irq_callback;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief STM32 GPIO external interrupt callback function
 * @param GPIO_Pin[in] - GPIO pin
 * @return none
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint8_t id;

	/* Invoke the callback function attached to corresponding GPIO pin */
	for (id = GPIO_IRQ_ID1; id <= GPIO_IRQ_ID5; id++) {
		if (stm32_gpio_irq_callback[id].GPIO_Pin == GPIO_Pin) {
			stm32_gpio_irq_callback[id].desc.callback(stm32_gpio_irq_callback[id].desc.ctx,
					id, NULL);
			return;
		}
	}
}

/**
 * @brief Initialize the STM32 GPIO IRQ Controller
 * @param desc[in,out] - GPIO IRQ descriptor
 * @param param[in] - GPIO IRQ initialization parameters
 * @return SUCCESS in case of success, negative error code otherwise
 */
int32_t stm32_gpio_irq_ctrl_init(struct irq_ctrl_desc **desc,
				 const struct irq_init_param *param)
{
	struct irq_ctrl_desc *descriptor;
	struct stm32_gpio_irq_desc *stm32_desc;
	int32_t ret;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct irq_ctrl_desc *)calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	stm32_desc = (struct stm32_gpio_irq_desc *)calloc(1, sizeof(*stm32_desc));
	if (!stm32_desc) {
		ret = -ENOMEM;
		goto error_desc;
	}

	switch (param->irq_ctrl_id) {
	case GPIO_IRQ_ID1:
	case GPIO_IRQ_ID2:
	case GPIO_IRQ_ID3:
	case GPIO_IRQ_ID4:
	case GPIO_IRQ_ID5:
		stm32_desc->gpio_num = ((struct stm32_gpio_irq_init_param *)(
						param->extra))->gpio_num;
		break;

	default:
		ret = -EINVAL;
		goto error_extra;
	}

	stm32_desc->irq_type = ((struct stm32_gpio_irq_init_param *)(
					param->extra))->irq_type;

	descriptor->extra = stm32_desc;
	*desc = descriptor;

	return SUCCESS;

error_extra:
	free(stm32_desc);
error_desc:
	free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by stm32_gpio_irq_ctrl_init()
 * @param desc[in] - GPIO IRQ descriptor
 * @return SUCCESS in case of success, negative error code otherwise
 */
int32_t stm32_gpio_irq_ctrl_remove(struct irq_ctrl_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (desc->extra)
		free(desc->extra);

	free(desc);
	return SUCCESS;
}

/**
 * @brief Register a callback to handle the GPIO irq events
 * @param desc[in] - IRQ descriptor
 * @param irq_id[in] - IRQ ID
 * @param callback_desc[in] - IRQ callback descriptor
 * @return SUCCESS in case of success, negative error code otherwise
 */
int32_t stm32_gpio_irq_register_callback(struct irq_ctrl_desc *desc,
		uint32_t irq_id,
		struct callback_desc *callback_desc)
{
	if (!desc || !desc->extra || !callback_desc)
		return -EINVAL;

	switch (irq_id) {
	case GPIO_IRQ_ID1:
	case GPIO_IRQ_ID2:
	case GPIO_IRQ_ID3:
	case GPIO_IRQ_ID4:
	case GPIO_IRQ_ID5:
		stm32_gpio_irq_callback[irq_id].GPIO_Pin = ((struct stm32_gpio_irq_desc *)(
					desc->extra))->gpio_num;
		stm32_gpio_irq_callback[irq_id].desc.callback = callback_desc->callback;
		stm32_gpio_irq_callback[irq_id].desc.config = callback_desc->config;
		stm32_gpio_irq_callback[irq_id].desc.ctx = callback_desc->ctx;
		break;

	default:
		return -EINVAL;
	}

	return SUCCESS;
}

/**
 * @brief Unregister a callback to handle the GPIO irq events
 * @param desc[in] - GPIO IRQ descriptor
 * @param irq_id[in] - GPIO IRQ ID
 * @return SUCCESS in case of success, negative error otherwise
 */
int32_t stm32_gpio_irq_unregister(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	if (!desc || !desc->extra || (irq_id >= NB_INTERRUPTS))
		return -EINVAL;

	stm32_gpio_irq_callback[irq_id].desc.callback = NULL;
	return SUCCESS;
}

/**
 * @brief Specific GPIO interrupt enable
 * @param desc[in] - GPIO IRQ descriptor
 * @param irq_id[in] - GPIO IRQ ID
 * @return SUCCESS in case of success, negative error otherwise
 */
int32_t stm32_gpio_irq_enable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	IRQn_Type irq_type;

	if (!desc || !desc->extra || (irq_id >= NB_GPIO_INTERRUPTS))
		return -EINVAL;

	irq_type = ((struct stm32_gpio_irq_desc *)(desc->extra))->irq_type;
	HAL_NVIC_EnableIRQ(irq_type);

	return SUCCESS;
}

/**
 * @brief Specific interrupt disable
 * @param desc[in] - GPIO IRQ descriptor
 * @param irq_id[in] - GPIO IRQ ID
 * @return SUCCESS in case of success, negative error otherwise
 */
int32_t stm32_gpio_irq_disable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	IRQn_Type irq_type;

	if (!desc || !desc->extra || (irq_id >= NB_GPIO_INTERRUPTS))
		return -EINVAL;

	irq_type = ((struct stm32_gpio_irq_desc *)(desc->extra))->irq_type;
	HAL_NVIC_DisableIRQ(irq_type);

	return SUCCESS;
}

/**
 * @brief STM32 platform specific GPIO IRQ platform ops structure
 */
const struct irq_platform_ops stm32_gpio_irq_ops = {
	.init = &stm32_gpio_irq_ctrl_init,
	.enable = &stm32_gpio_irq_enable,
	.disable = &stm32_gpio_irq_disable,
	.register_callback = &stm32_gpio_irq_register_callback,
	.unregister = &stm32_gpio_irq_unregister,
	.remove = &stm32_gpio_irq_ctrl_remove
};
