/***************************************************************************//**
 *   @file   mbed_gpio_irq.h
 *   @brief  Header containing extra types required for GPIO IRQ drivers
********************************************************************************
 * Copyright (c) 2022 Analog Devices, Inc.
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

#ifndef MBED_GPIO_IRQ_H
#define MBED_GPIO_IRQ_H

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

/**
 * @enum gpio_irq_id
 * @brief GPIO IRQ IDs supported by the mbed gpio irq driver
 * @note Every interrupt instance should have unique interrupt ID. For instance,
 * there are 5 interrupt IDs for external/gpio interrupts. The IDs can be mapped
 * randomly to any of the instances as Mbed layer abstracts most of the low level
 * MCU details such as IRQn_type, IRQn_number, etc
 * Example: mbed_gpio_irq_enable(irq_desc, GPIO_IRQ_ID1);
 */
enum gpio_irq_id {
	/* External/GPIO interrupt IDs (Max possible instances of external IRQs are
	 * possible/implemented in the mbed irq module) */
	/** GPIO/External interrupt ID1 */
	GPIO_IRQ_ID1,
	/** GPIO/External interrupt ID2 */
	GPIO_IRQ_ID2,
	/** GPIO/External interrupt ID3 */
	GPIO_IRQ_ID3,
	/** GPIO/External interrupt ID4 */
	GPIO_IRQ_ID4,
	/** GPIO/External interrupt ID5 */
	GPIO_IRQ_ID5,

	/* Number of available gpio interrupts */
	NB_GPIO_IRQS
};

/**
 * @struct mbed_gpio_irq_init_param
 * @brief Structure holding the extra parameters for GPIO Interrupt Request
 * Example:
 *		struct mbed_irq_init_param my_ext_mbed_irq_init_param = {
 *			.ext_int_pin = ARDUINO_D2,
 *		}
 */
struct mbed_gpio_irq_init_param {
	uint32_t gpio_irq_pin;	// External Interrupt/GPIO pin
};

/**
 * @struct mbed_irq_desc
 * @brief Structure holding the platform descriptor for Interrupt Request
 */
struct mbed_gpio_irq_desc {
	uint32_t gpio_irq_pin;	// External Interrupt/GPIO pin
	void *int_obj;			// GPIO/Ext IRQ instance
};

/**
 * @brief Mbed specific GPIO IRQ platform ops structure
 */
extern const struct no_os_irq_platform_ops mbed_gpio_irq_ops;

#ifdef __cplusplus // Closing extern c
}
#endif

#endif // MBED_GPIO_IRQ_H
