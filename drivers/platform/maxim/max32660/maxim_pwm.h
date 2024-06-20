/***************************************************************************//**
 *   @file   maxim_pwm.h
 *   @brief  Maxim platform specific implementation PWM API.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
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
