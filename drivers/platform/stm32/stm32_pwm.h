/***************************************************************************//**
 *   @file   stm32/stm32_pwm.h
 *   @brief  Header file for stm32 pwm specifics.
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef STM32_PWM_H_
#define STM32_PWM_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_irq.h"
#include "stm32_hal.h"

enum stm32_pwm_timer {
	STM32_PWM_TIMER_TIM = 0,
	STM32_PWM_TIMER_LPTIM = 1,
};

enum TimOCMode {
	TIM_OC_TOGGLE = 0,
	TIM_OC_PWM1 = 1,
	TIM_OC_PWM2 = 2,
};

enum stm32_pwm_trigger {
	PWM_TS_NONE,
	PWM_TS_ITR0,
	PWM_TS_ITR1,
	PWM_TS_ITR2,
	PWM_TS_ITR3,
	PWM_TS_ETR,
};

enum stm32_pwm_trigger_polarity {
	PWM_TRIG_POL_RISING,
	PWM_TRIG_POL_FALLING,
};

enum stm32_pwm_trigger_out {
	PWM_TRGO_RESET,
	PWM_TRGO_ENABLE,
	PWM_TRGO_UPDATE,
	PWM_TRGO_OC1,
	PWM_TRGO_OC1REF,
	PWM_TRGO_OC2REF,
	PWM_TRGO_OC3REF,
	PWM_TRGO_OC4REF,
};

enum stm32_pwm_slave_mode {
	STM32_PWM_SM_DISABLE,
	STM32_PWM_SM_TRIGGER,
	STM32_PWM_SM_EXTERNAL1,
	STM32_PWM_SM_GATED,
};

/**
 * @struct stm32_pwm_init_param
 * @brief Structure holding the STM32 PWM parameters.
 */
struct stm32_pwm_init_param {
	/** PWM Timer Handle */
	void *htimer;
	/** Type of timer used for PWM */
	enum stm32_pwm_timer pwm_timer;
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
	/** Slave mode */
	enum stm32_pwm_slave_mode slave_mode;
	/** Trigger source selection */
	enum stm32_pwm_trigger trigger_source;
	/** Trigger out selection */
	enum stm32_pwm_trigger_out trigger_output;
	/** Trigger polarity selection */
	enum stm32_pwm_trigger_polarity trigger_polarity;
	/* Enable one pulse */
	bool onepulse_enable;
	/* Number of pulse repetitions */
	uint32_t repetitions;
	/* Enable dma */
	bool dma_enable;
	/** Timer callback */
	struct no_os_callback_desc timer_callback;
};

/**
 * @struct stm32_pwm_desc
 * @brief Structure holding the STM32 PWM descriptor.
 */
struct stm32_pwm_desc {
	/** PWM Timer Handle */
	void *htimer;
	/** Type of timer used for PWM */
	enum stm32_pwm_timer pwm_timer;
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
	/* Number of pulse repetitions */
	uint32_t repetitions;
	/* Enable dma */
	bool dma_enable;
	/* Enable one pulse */
	bool onepulse_enable;
};

/**
 * @brief STM32 specific PWM platform ops structure
 */
extern const struct no_os_pwm_platform_ops stm32_pwm_ops;

#endif // STM32_PWM_H_
