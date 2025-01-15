/***************************************************************************//**
 *   @file   stm32/stm32_timer.h
 *   @brief  Header file for the stm32 timer driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef STM32_TIMER_H_
#define STM32_TIMER_H_

#ifdef HAL_TIM_MODULE_ENABLED

#include <stdint.h>
#include "no_os_timer.h"
#include "stm32_hal.h"

/**
 * @struct stm32_timer_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 * specific timer parameters.
 */
struct stm32_timer_init_param {
	TIM_HandleTypeDef *htimer;
};

/**
 * @struct stm32_timer_desc
 * @brief stm32 platform specific timer descriptor
 */
struct stm32_timer_desc {
	/** timer instance */
	TIM_HandleTypeDef *htimer;
};

/**
 * @brief stm32 specific timer platform ops structure
 */
extern const struct no_os_timer_platform_ops stm32_timer_ops;

#endif

#endif // STM32_TIMER_H_
