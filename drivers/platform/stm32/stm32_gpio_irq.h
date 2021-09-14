/***************************************************************************//**
 *   @file   stm32/stm32_gpio_irq.h
 *   @brief  Header file for stm32 GPIO IRQ specifics.
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
#ifndef GPIO_IRQ_EXTRA_H_
#define GPIO_IRQ_EXTRA_H_

#include <stdint.h>
#include <stdbool.h>
#include "no-os/irq.h"
#include "stm32_hal.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum gpio_irq_id
 * @brief Interrupt IDs supported by the STM32 GPIO irq driver
 */
enum gpio_irq_id {
	/** GPIO Interrupt ID1 */
	GPIO_IRQ_ID1,
	/** GPIO Interrupt ID2 */
	GPIO_IRQ_ID2,
	/** GPIO Interrupt ID3 */
	GPIO_IRQ_ID3,
	/** GPIO Interrupt ID4 */
	GPIO_IRQ_ID4,
	/** GPIO Interrupt ID5 */
	GPIO_IRQ_ID5,
	/* Number of available gpio interrupts */
	NB_GPIO_INTERRUPTS
};

/**
 * @struct stm32_gpio_irq_init_param
 * @brief Structure holding the STM32 GPIO Interrupt Event parameters.
 */
struct stm32_gpio_irq_init_param {
	/** GPIO Pin number */
	uint16_t gpio_num;
	/** STM32 IRQ type */
	IRQn_Type irq_type;
};

/**
 * @struct stm32_gpio_irq_desc
 * @brief Structure holding the STM32 GPIO Interrupt Event descriptor.
 */
struct stm32_gpio_irq_desc {
	/** GPIO Pin number */
	uint16_t gpio_num;
	/** STM32 IRQ type */
	IRQn_Type irq_type;
};

/**
 * @brief STM32 specific GPIO IRQ platform ops structure
 */
extern const struct irq_platform_ops stm32_gpio_irq_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

#endif // GPIO_IRQ_EXTRA_H_
