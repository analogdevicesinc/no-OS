/***************************************************************************//**
 *   @file   stm32/stm32_pwm.h
 *   @brief  Header file for stm32 pwm specifics.
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef STM32_PWM_H_
#define STM32_PWM_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "stm32_gpio.h"
#include "stm32_hal.h"

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum TimOCMode {
	TIM_OC_TOGGLE = 0,
	TIM_OC_PWM1 = 1,
	TIM_OC_PWM2 = 2,
};

/**
 * @struct stm32_pwm_init_param
 * @brief Structure holding the STM32 PWM parameters.
 */
struct stm32_pwm_init_param {
	/** Timer prescaler (0 to 0xFFFF) */
	uint32_t prescaler;
	/** Timer autoreload enable */
	bool timer_autoreload;
	/** Timer output compare Mode */
	enum TimOCMode mode;
	/** PWM timer channel */
	uint32_t timer_chn;
	/** Complementary channel */
	bool complementary_channel;
	/** Get timer source clock function */
	uint32_t (*get_timer_clock)(void);
	/** Get timer source clock divider */
	uint32_t clock_divider;
	/** Timer callback */
	struct no_os_callback_desc timer_callback;
};

/**
 * @struct stm32_pwm_desc
 * @brief Structure holding the STM32 PWM descriptor.
 */
struct stm32_pwm_desc {
	/** PWM Timer Instance */
	TIM_HandleTypeDef htimer;
	/** Timer GPIO instance */
	struct no_os_gpio_desc *gpio;
	/** Timer prescaler */
	uint32_t prescaler;
	/** Timer autoreload enable */
	bool timer_autoreload;
	/** Timer output compare Mode */
	enum TimOCMode mode;
	/** PWM timer channel */
	uint32_t timer_chn;
	/** Complementary channel */
	bool complementary_channel;
	/** Get timer source clock function */
	uint32_t (*get_timer_clock)(void);
	/** Get timer source clock divider */
	uint32_t clock_divider;
	/** Interrupt controller descriptor */
	struct no_os_irq_ctrl_desc *nvic_tim;
	/** Timer callback */
	struct no_os_callback_desc timer_callback;
};

/**
 * @brief STM32 specific PWM platform ops structure
 */
extern const struct no_os_pwm_platform_ops stm32_pwm_ops;

#endif // STM32_PWM_H_
