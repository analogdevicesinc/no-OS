/***************************************************************************//**
 *   @file   maxim_pwm.c
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

#include "tmr.h"
#include "mxc_pins.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_units.h"
#include "maxim_pwm.h"

/**
 * @brief Convert the prescaler from a clock division value to
 * an enum's value for the init function.
 * @param div - clock division value.
 * @param prescaler - enum value to be provided to the init function.
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int _get_prescaler(uint32_t div, mxc_tmr_pres_t *prescaler)
{
	int i;

	i = 1;

	while (div / MAX_PWM_GET_PRESCALER(i) > MAX_PWM_TMR_MAX_VAL)
		i++;

	*prescaler = MAX_PWM_PRESCALER_VAL(i);

	return 0;
}

/**
 * @brief Enable the PWM.
 * @param desc - PWM descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int max_pwm_enable(struct no_os_pwm_desc *desc)
{
	MXC_TMR_Start(MXC_TMR_GET_TMR(desc->id));

	return 0;
}

/**
 * @brief Disable the PWM.
 * @param desc - PWM descriptor.
 * @return 0 in case of success, negative error otherwise.
 */
int max_pwm_disable(struct no_os_pwm_desc *desc)
{
	MXC_TMR_Stop(MXC_TMR_GET_TMR(desc->id));

	return 0;
}

/**
 * @brief Configure the VDDIO level for the PWM pins
 * @param id - the identification number.
 * @param vssel - the VDDIO level.
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int _max_pwm_pins_vddio(uint32_t id, mxc_gpio_vssel_t vssel)
{
	mxc_gpio_cfg_t pwm_pins;

	switch (id) {
	case 0:
		pwm_pins = gpio_cfg_tmr0;
		break;
	case 1:
		pwm_pins = gpio_cfg_tmr1;
		break;
	case 2:
		pwm_pins = gpio_cfg_tmr2;
		break;
	case 3:
		pwm_pins = gpio_cfg_tmr3;
		break;
	case 4:
		pwm_pins = gpio_cfg_tmr4;
		break;
	case 5:
		pwm_pins = gpio_cfg_tmr5;
		break;
	default:
		return -EINVAL;
	}

	pwm_pins.vssel = vssel;

	return MXC_GPIO_Config(&pwm_pins);
}

/**
 * @brief Set the PWM period.
 * @param desc - Pointer where the configured instance is stored.
 * @param period_ns - PWM period.
 * @return 0 in case of success, negative error code otherwise.
 */
int max_pwm_set_period(struct no_os_pwm_desc *desc,
		       uint32_t period_ns)
{
	struct max_pwm_extra *pwm_extra;
	uint32_t period_ticks, div;
	mxc_tmr_pres_t prescaler;
	mxc_tmr_cfg_t cfg;
	int ret;

	pwm_extra = desc->extra;

	div = PeripheralClock / NO_OS_DIV_ROUND_CLOSEST_ULL(NANO, period_ns);
	ret = _get_prescaler(div, &prescaler);
	if (ret)
		return ret;

	period_ticks = PeripheralClock / (NO_OS_DIV_ROUND_CLOSEST_ULL(NANO,
					  period_ns) * MAX_PWM_PRESCALER_TRUE(prescaler));

	cfg.pres = prescaler;
	cfg.mode = TMR_MODE_PWM;
	cfg.cmp_cnt = period_ticks;
	cfg.pol = desc->polarity;

	max_pwm_disable(desc);

	MXC_TMR_Init(MXC_TMR_GET_TMR(desc->id), &cfg);

	ret = _max_pwm_pins_vddio(desc->id, pwm_extra->vssel);
	if (ret)
		return ret;

	pwm_extra->tmr_cfg = cfg;
	desc->period_ns = period_ns;

	return 0;
}

/**
 * @brief Get the PWM period.
 * @param desc - Pointer where the configured instance is stored.
 * @param period_ns - PWM period.
 * @return 0 in case of success, negative error code otherwise.
 */
int max_pwm_get_period(struct no_os_pwm_desc *desc,
		       uint32_t *period_ns)
{
	*period_ns = desc->period_ns;

	return 0;
}

/**
 * @brief Set the PWM duty cycle.
 * @param desc - Pointer where the configured instance is stored.
 * @param duty_cycle_ns - PWM duty cycle.
 * @return 0 in case of success, negative error code otherwise.
 */
int max_pwm_set_duty_cycle(struct no_os_pwm_desc *desc,
			   uint32_t duty_cycle_ns)
{
	struct max_pwm_extra *pwm_extra;
	uint32_t duty_ticks;
	int ret;

	pwm_extra = desc->extra;

	duty_ticks = PeripheralClock / (NO_OS_DIV_ROUND_CLOSEST_ULL(NANO,
					duty_cycle_ns) * MAX_PWM_PRESCALER_TRUE(pwm_extra->tmr_cfg.pres));

	/* Set duty cycle to a minimum. */
	if (!duty_ticks)
		duty_ticks = 1;

	ret = MXC_TMR_SetPWM(MXC_TMR_GET_TMR(desc->id), duty_ticks);
	if (ret)
		return ret;

	/*  Set actual duty cycle value. */
	desc->duty_cycle_ns = duty_ticks * MAX_PWM_PRESCALER_TRUE(
				      pwm_extra->tmr_cfg.pres) * NO_OS_DIV_ROUND_CLOSEST_ULL(NANO,PeripheralClock);

	return 0;
}

/**
 * @brief Get the PWM duty cycle.
 * @param desc - Pointer where the configured instance is stored.
 * @param duty_cycle_ns - PWM duty cycle.
 * @return 0 in case of success, negative error code otherwise.
 */
int max_pwm_get_duty_cycle(struct no_os_pwm_desc *desc,
			   uint32_t *duty_cycle_ns)
{
	*duty_cycle_ns = desc->duty_cycle_ns;

	return 0;
}

/**
 * @brief Initialize the PWM.
 * @param desc - PWM descriptor.
 * @param param - The structure containing PWM init parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int max_pwm_init(struct no_os_pwm_desc **desc,
		 const struct no_os_pwm_init_param *param)
{
	struct max_pwm_init_param *extra_init;
	struct no_os_pwm_desc *descriptor;
	struct max_pwm_extra *pwm_extra;
	int ret;

	descriptor = (struct no_os_pwm_desc *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	pwm_extra = (struct max_pwm_extra *)no_os_calloc(1, sizeof(*pwm_extra));
	if (!pwm_extra) {
		ret = -ENOMEM;
		goto error;
	}

	descriptor->id = param->id;
	descriptor->polarity = param->polarity;

	if (param->extra) {
		extra_init = param->extra;
		pwm_extra->vssel = extra_init->vssel;
	}

	descriptor->extra = pwm_extra;
	ret = max_pwm_set_period(descriptor, param->period_ns);
	if (ret)
		goto free_pwm_extra;

	ret = max_pwm_set_duty_cycle(descriptor, param->duty_cycle_ns);
	if (ret)
		goto free_pwm_extra;

	max_pwm_enable(descriptor);

	*desc = descriptor;

	return 0;

free_pwm_extra:
	no_os_free(pwm_extra);
error:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by max_pwm_init().
 * @param desc - PWM descriptor
 * @return 0 in case of success, negative error code otherwise.
 */
int max_pwm_remove(struct no_os_pwm_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_TMR_Shutdown(MXC_TMR_GET_TMR(desc->id));
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
* @brief Maxim platform specific callbacks for the PWM API
*/
const struct no_os_pwm_platform_ops max_pwm_ops = {
	.pwm_ops_enable = (int32_t (*)())max_pwm_enable,
	.pwm_ops_disable = (int32_t (*)())max_pwm_disable,
	.pwm_ops_set_period = (int32_t (*)())max_pwm_set_period,
	.pwm_ops_get_period = (int32_t (*)())max_pwm_get_period,
	.pwm_ops_set_duty_cycle = (int32_t (*)())max_pwm_set_duty_cycle,
	.pwm_ops_get_duty_cycle = (int32_t (*)())max_pwm_get_duty_cycle,
	.pwm_ops_init = (int32_t (*)())max_pwm_init,
	.pwm_ops_remove = (int32_t (*)())max_pwm_remove
};
