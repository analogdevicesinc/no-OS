/***************************************************************************//**
 *   @file   stm32/stm32_pwm.h
 *   @brief  Header file for stm32 pwm specifics.
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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

enum stm32_pwm_trigger {
	PWM_TS_ITR0,
	PWM_TS_ITR1,
	PWM_TS_ITR2,
	PWM_TS_ITR3,
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
	/** Enable trigger source */
	bool trigger_enable;
	/** Trigger source selection */
	enum stm32_pwm_trigger trigger_source;
	/** Trigger out selection */
	enum stm32_pwm_trigger_out trigger_output;
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
	/* Number of pulse repetitions */
	uint32_t repetitions;
	/* Enable dma */
	bool dma_enable;
};

/**
 * @brief STM32 specific PWM platform ops structure
 */
extern const struct no_os_pwm_platform_ops stm32_pwm_ops;

#endif // STM32_PWM_H_
