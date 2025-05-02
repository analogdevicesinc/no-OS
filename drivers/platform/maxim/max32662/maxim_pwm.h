/***************************************************************************//**
 *   @file   maxim_pwm.h
 *   @brief  Maxim platform specific implementation PWM API.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef MAXIM_PWM_H_
#define MAXIM_PWM_H_

#include "tmr.h"
#include "gpio.h"
#include "no_os_pwm.h"
#include "no_os_util.h"

#define MAX_PWM_TMR_MAX_VAL		NO_OS_GENMASK(15, 0)
#define MAX_PWM_PRESCALER_VAL(n)	((n - 1) * 16)
#define MAX_PWM_PRESCALER_TRUE(n)	NO_OS_BIT((n) / 16)
#define MAX_PWM_GET_PRESCALER(n)	NO_OS_BIT((n) - 1)

struct max_pwm_init_param {
	mxc_gpio_vssel_t vssel;
};

struct max_pwm_extra {
	mxc_gpio_vssel_t vssel;
	mxc_tmr_cfg_t tmr_cfg;
};

/**
 * @brief Maxim specific PWM ops
 */
extern const struct no_os_pwm_platform_ops max_pwm_ops;

#endif // MAXIM_PWM_H_
