/***************************************************************************//**
 *   @file   stm32/stm32_pwm.h
 *   @brief  Header file for stm32 pwm specifics.
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#ifndef PWM_EXTRA_H_
#define PWM_EXTRA_H_

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
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

/**
 * @struct stm32_pwm_init_param
 * @brief Structure holding the STM32 PWM parameters.
 */
struct stm32_pwm_init_param {
	/** PWM Timer base */
	TIM_TypeDef *base;
	/** PWM Timer Channel */
	uint32_t timer_chn;
	/** PWM Timer GPIO init parameters */
	//stm32_gpio_init_param *gpio;
	gpio_init_param *gpio;
	/** Get timer clock frequency function */
	uint32_t (*get_timer_clock)(void);
} stm32_pwm_init_param;

/**
 * @struct stm32_pwm_desc
 * @brief Structure holding the STM32 PWM descriptor.
 */
struct stm32_pwm_desc {
	/** PWM Timer Instance */
	TIM_HandleTypeDef htimer;
	/** Timer GPIO instance */
	gpio_desc *gpio;
	/** PWM Timer period */
	uint32_t timer_period;
	/** PWM Timer Channel */
	uint32_t timer_chn;
} stm32_pwm_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the PWM generator device */
int32_t stm32_pwm_init(struct pwm_desc **desc,
		       const struct pwm_init_param *param);

/* Free the resources used by the PWM generator device */
int32_t stm32_pwm_remove(struct pwm_desc *desc);

/* Enable PWM generator device */
int32_t stm32_pwm_enable(struct pwm_desc *desc);

/* Disable PWM generator device */
int32_t stm32_pwm_disable(struct pwm_desc *desc);

#endif // PWM_EXTRA_H_
