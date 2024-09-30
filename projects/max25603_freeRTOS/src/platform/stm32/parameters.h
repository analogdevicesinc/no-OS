/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of STM32 platform data used by MAX25603 FreeRTOS
 * 	     project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_pwm.h"
#include "stm32_gpio.h"

#define GPIO_LB_PORT_NUM	1
#define GPIO_LB_PIN_NUM		4

#define GPIO_HB_PORT_NUM	0
#define GPIO_HB_PIN_NUM		7

#define GPIO_EN1_PORT_NUM	1
#define GPIO_EN1_PIN_NUM	0

#define GPIO_EN2_PORT_NUM	1
#define GPIO_EN2_PIN_NUM	1

#define PWM_EN1_ID		3
#define PWM_EN1_PERIOD_NS	200000
#define PWM_EN1_DUTY_NS		100000

#define PWM_EN2_ID		3
#define PWM_EN2_PERIOD_NS	200000
#define PWM_EN2_DUTY_NS		100000

#define PWM_LB_ID		3
#define PWM_LB_PERIOD_NS	200000
#define PWM_LB_DUTY_NS		100000

#define PWM_HB_ID		3
#define PWM_HB_PERIOD_NS	200000
#define PWM_HB_DUTY_NS		100000

#define PWM_GPIO_EXTRA		&max25603_gpio_pwm_extra
#define PWM_LB_EXTRA		&max25603_pwm_lb_extra
#define PWM_HB_EXTRA		&max25603_pwm_hb_extra
#define PWM_EN1_EXTRA		&max25603_pwm_en1_extra
#define PWM_EN2_EXTRA		&max25603_pwm_en2_extra
#define PWM_OPS			&stm32_pwm_ops

#define GPIO_SHUNT_PORT_NUM	0
#define GPIO_SHUNT_PIN_NUM	10

#define GPIO_EXT_PORT_NUM	0
#define GPIO_EXT_PIN_NUM	9

#define GPIO_OPS		&stm32_gpio_ops
#define GPIO_EXTRA		&max25603_gpio_extra

extern struct stm32_pwm_init_param max25603_pwm_lb_extra;
extern struct stm32_pwm_init_param max25603_pwm_hb_extra;
extern struct stm32_pwm_init_param max25603_pwm_en1_extra;
extern struct stm32_pwm_init_param max25603_pwm_en2_extra;
extern struct stm32_gpio_init_param max25603_gpio_extra;
extern struct stm32_gpio_init_param max25603_gpio_pwm_extra;

#endif /* __PARAMETERS_H__ */
