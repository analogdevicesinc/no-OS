/***************************************************************************//**
 *   @file   aducm3029_pwm.c
 *   @brief  Implementation of aducm3029_pwm.c
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "no_os_pwm.h"
#include "stdlib.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include <drivers/tmr/adi_tmr.h>
#include <drivers/pwr/adi_pwr.h>

#define NO_OS_NS_PER_SEC 1000000000

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

static int32_t aducm3029_config_clock_init(ADI_TMR_CONFIG *cfg, uint32_t period,
		uint32_t duty, uint32_t *match)
{
	float		base_time;
	float		aux;
	uint32_t	clk_freq;
	int32_t		ret;
	/*
	 * ADI_TMR_CLOCK_HFOSC (26MHz) &&  ADI_TMR_PRESCALER_1
	 * -> base_period = 38nS -> max_period = 2,490330 ms
	 * ADI_TMR_CLOCK_HFOSC (26MHz) &&  ADI_TMR_PRESCALER_16
	 * -> base_period = 615nS -> max_period = 40,304025 ms
	 * ADI_TMR_CLOCK_HFOSC (26MHz) &&  ADI_TMR_PRESCALER_64
	 * -> base_period = 2461nS -> max_period = 161,281635 ms
	 * ADI_TMR_CLOCK_HFOSC (26MHz) &&  ADI_TMR_PRESCALER_256
	 * -> base_period = 9846nS -> max_period = 645,257610 ms
	 *
	 * ADI_TMR_CLOCK_LFOSC (32KHz) &&  ADI_TMR_PRESCALER_1
	 * -> base_period = 31,25uS -> max_period = 2,04796875 s
	 * ADI_TMR_CLOCK_LFOSC (32KHz) &&  ADI_TMR_PRESCALER_16
	 * -> base_period = 500uS -> max_period = 32,767500 s
	 * ADI_TMR_CLOCK_LFOSC (32KHz) &&  ADI_TMR_PRESCALER_64
	 * -> base_period = 2mS -> max_period = 131,070 s
	 * ADI_TMR_CLOCK_LFOSC (32KHz) &&  ADI_TMR_PRESCALER_256
	 * -> base_period = 8mS -> max_period = 524,280 s
	 */

	/* Use HFOSC because in general periods for pwms are lower than 645 ms*/
	cfg->eClockSource = ADI_TMR_CLOCK_HFOSC;
	ret = adi_pwr_GetClockFrequency(ADI_CLOCK_HCLK, &clk_freq);
	if (ret != ADI_PWR_SUCCESS)
		return -ret;

	/* Select prescaler */
	aux = (float)UINT16_MAX / clk_freq * NO_OS_NS_PER_SEC;
	if (period > 256 * aux)
		return -EINVAL;
	if (period > 64 * aux) {
		cfg->ePrescaler = ADI_TMR_PRESCALER_256;
		base_time = 256;
	} else if (period > 16 * aux) {
		cfg->ePrescaler = ADI_TMR_PRESCALER_64;
		base_time = 64;
	} else if (period > 1 * aux) {
		cfg->ePrescaler = ADI_TMR_PRESCALER_16;
		base_time = 16;
	} else {
		cfg->ePrescaler = ADI_TMR_PRESCALER_1;
		if (cfg->bSyncBypass)
			base_time = 1;
		else
			base_time = 0;
	}
	base_time = base_time * NO_OS_NS_PER_SEC / clk_freq;
	cfg->nLoad = period / base_time;
	cfg->nAsyncLoad = cfg->nLoad;
	*match = cfg->nLoad - duty / base_time;

	return 0;
}

static int32_t aducm3029_update_pwm_config(struct no_os_pwm_desc *desc)
{
	ADI_TMR_CONFIG		cfg;
	ADI_TMR_PWM_CONFIG	pwm_cfg;
	ADI_TMR_RESULT		ret;
	uint32_t 		match_val;

	if (!desc)
		return -EINVAL;

	cfg = (ADI_TMR_CONFIG) {
		.bCountingUp = false,
		.bPeriodic = true,
		.bReloading = true,
		.bSyncBypass = true
	};
	ret = aducm3029_config_clock_init(&cfg, desc->period_ns, desc->duty_cycle_ns,
					  &match_val);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;
	do {
		ret = adi_tmr_ConfigTimer(desc->id, &cfg);
	} while (ret == ADI_TMR_DEVICE_BUSY);
	if (ret != ADI_TMR_SUCCESS)
		return -ret;

	pwm_cfg.eOutput = ADI_TMR_PWM_OUTPUT_0;
	pwm_cfg.bMatch = true;
	pwm_cfg.bIdleHigh = (desc->polarity == NO_OS_PWM_POLARITY_HIGH);
	pwm_cfg.nMatchValue = match_val;
	ret = adi_tmr_ConfigPwm(desc->id, &pwm_cfg);
	if (ret != ADI_TMR_SUCCESS)
		return -ret;

	return 0;
}

/* Initialize the PWM generator device */
int32_t aducm3029_pwm_init(struct no_os_pwm_desc **desc,
			   const struct no_os_pwm_init_param *param)
{
	ADI_TMR_RESULT	ret;
	struct no_os_pwm_desc	*ldesc;

	if (!desc || !param)
		return -EINVAL;

	if (param->id < ADI_TMR_DEVICE_GP0  || param->id > ADI_TMR_DEVICE_GP2)
		return -EINVAL;

	ldesc = (struct no_os_pwm_desc *)no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	ldesc->id = param->id;
	ldesc->duty_cycle_ns = param->duty_cycle_ns;
	ldesc->period_ns = param->period_ns;
	ldesc->polarity = param->polarity;

	ret = adi_tmr_Init(ldesc->id,  NULL, NULL, false);
	if (ret != ADI_TMR_SUCCESS)
		goto error;

	ret = aducm3029_update_pwm_config(ldesc);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error;

	*desc = ldesc;

	return 0;
error:
	*desc = NULL;
	no_os_free(ldesc);

	return ret;
}

/* Free the resources used by the PWM generator device */
int32_t aducm3029_pwm_remove(struct no_os_pwm_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/* Enable PWM generator device */
int32_t aducm3029_pwm_enable(struct no_os_pwm_desc *desc)
{
	ADI_TMR_RESULT		ret;

	if (!desc)
		return -EINVAL;

	ret = adi_tmr_Enable(desc->id, true);
	if (ret != ADI_TMR_SUCCESS)
		return -ret;

	return 0;
}

/* Disable PWM generator device */
int32_t aducm3029_pwm_disable(struct no_os_pwm_desc *desc)
{
	ADI_TMR_RESULT		ret;

	if (!desc)
		return -EINVAL;

	ret = adi_tmr_Enable(desc->id, false);
	if (ret != ADI_TMR_SUCCESS)
		return -ret;

	return 0;
}

/* Set period of PWM generator device */
int32_t aducm3029_pwm_set_period(struct no_os_pwm_desc *desc,
				 uint32_t period_ns)
{
	if (!desc)
		return -EINVAL;

	desc->period_ns = period_ns;

	return aducm3029_update_pwm_config(desc);
}

/* Get period of PWM generator device */
int32_t aducm3029_pwm_get_period(struct no_os_pwm_desc *desc,
				 uint32_t *period_ns)
{
	if (!desc || !period_ns)
		return -EINVAL;

	*period_ns = desc->period_ns;

	return 0;
}

/* Set duty cycle of PWM generator device */
int32_t aducm3029_pwm_set_duty_cycle(struct no_os_pwm_desc *desc,
				     uint32_t duty_cycle_ns)
{
	if (!desc)
		return -EINVAL;

	desc->duty_cycle_ns = duty_cycle_ns;

	return aducm3029_update_pwm_config(desc);
}

/* Get period of PWM generator device */
int32_t aducm3029_pwm_get_duty_cycle(struct no_os_pwm_desc *desc,
				     uint32_t *duty_cycle_ns)
{
	if (!desc || !duty_cycle_ns)
		return -EINVAL;

	*duty_cycle_ns = desc->duty_cycle_ns;

	return 0;
}

/* Set polarity of PWM generator device */
int32_t aducm3029_pwm_set_polarity(struct no_os_pwm_desc *desc,
				   enum no_os_pwm_polarity polarity)
{
	if (!desc)
		return -EINVAL;

	desc->polarity = polarity;

	return aducm3029_update_pwm_config(desc);
}

/* Set polarity of PWM generator device */
int32_t aducm3029_pwm_get_polarity(struct no_os_pwm_desc *desc,
				   enum no_os_pwm_polarity *polarity)
{
	if (!desc || !polarity)
		return -EINVAL;

	*polarity = desc->polarity;

	return 0;
}

/**
* @brief ADUCM3029 platform specific PWM platform ops structure
*/
const struct no_os_pwm_platform_ops aducm3029_pwm_ops = {
	.pwm_ops_init = &aducm3029_pwm_init,
	.pwm_ops_enable = &aducm3029_pwm_enable,
	.pwm_ops_disable = &aducm3029_pwm_disable,
	.pwm_ops_set_period = &aducm3029_pwm_set_period,
	.pwm_ops_get_period = &aducm3029_pwm_get_period,
	.pwm_ops_set_duty_cycle = &aducm3029_pwm_set_duty_cycle,
	.pwm_ops_get_duty_cycle = &aducm3029_pwm_get_duty_cycle,
	.pwm_ops_set_polarity = &aducm3029_pwm_set_polarity,
	.pwm_ops_get_polarity = &aducm3029_pwm_get_polarity,
	.pwm_ops_remove = &aducm3029_pwm_remove
};
