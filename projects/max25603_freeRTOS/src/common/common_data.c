/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Common data used by the MAX25603 FreeRTOS project.
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
#include "common_data.h"

struct no_os_gpio_init_param max25603_shunt_ip = {
	.port = GPIO_SHUNT_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_SHUNT_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param max25603_en1_gpio_ip = {
	.port = GPIO_EN1_PORT_NUM,
	.number = GPIO_EN1_PIN_NUM,
	.extra = PWM_GPIO_EXTRA,
	.platform_ops = GPIO_OPS,
};

struct no_os_pwm_init_param max25603_en1_ip = {
	.id = PWM_EN1_ID,
	.period_ns = PWM_EN1_PERIOD_NS,
	.duty_cycle_ns = PWM_EN1_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_EN1_EXTRA,
};

struct no_os_gpio_init_param max25603_en2_gpio_ip = {
	.port = GPIO_EN2_PORT_NUM,
	.number = GPIO_EN2_PIN_NUM,
	.extra = PWM_GPIO_EXTRA,
	.platform_ops = GPIO_OPS,
};

struct no_os_pwm_init_param max25603_en2_ip = {
	.id = PWM_EN2_ID,
	.period_ns = PWM_EN2_PERIOD_NS,
	.duty_cycle_ns = PWM_EN2_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_EN2_EXTRA,
};

struct no_os_gpio_init_param max25603_hb_gpio_ip = {
	.port = GPIO_HB_PORT_NUM,
	.number = GPIO_HB_PIN_NUM,
	.extra = PWM_GPIO_EXTRA,
	.platform_ops = GPIO_OPS,
};

struct no_os_pwm_init_param max25603_hb_ip = {
	.id = PWM_HB_ID,
	.period_ns = PWM_HB_PERIOD_NS,
	.duty_cycle_ns = PWM_HB_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_HB_EXTRA,
};

struct no_os_gpio_init_param max25603_lb_gpio_ip = {
	.port = GPIO_LB_PORT_NUM,
	.number = GPIO_LB_PIN_NUM,
	.extra = PWM_GPIO_EXTRA,
	.platform_ops = GPIO_OPS,
};

struct no_os_pwm_init_param max25603_lb_ip = {
	.id = PWM_LB_ID,
	.period_ns = PWM_LB_PERIOD_NS,
	.duty_cycle_ns = PWM_LB_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.pwm_gpio = &max25603_lb_gpio_ip,
	.extra = PWM_LB_EXTRA,
};

struct no_os_gpio_init_param gpio_ip = {
	.port = GPIO_EXT_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_EXT_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct max25603_init_param max25603_ip = {
	.en1_param = &max25603_en1_ip,
	.en2_param = &max25603_en2_ip,
	.hb_param = &max25603_hb_ip,
	.lb_param = &max25603_lb_ip,
	.shunt_param = &max25603_shunt_ip,
};
