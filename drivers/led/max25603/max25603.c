/***************************************************************************//**
 *   @file   max25603.c
 *   @brief  Source file of MAX25603 Driver.
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
#include "max25603.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

int max25603_sel_comp(struct max25603_desc *desc, enum max25603_comp comp)
{
	enum no_os_gpio_values gpio_val;
	int ret;

	gpio_val = comp == MAX25603_COMP2 ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW;
	ret = no_os_gpio_set_value(desc->shunt_desc, gpio_val);
	if (ret)
		return ret;

	return 0;
}

int max25603_set_enable(struct max25603_desc *desc, enum max25603_enable enable,
			uint32_t freq_hz, uint8_t duty)
{
	uint32_t period_ns;
	int ret;

	period_ns = NO_OS_DIV_ROUND_CLOSEST_ULL(NANO, freq_hz);

	switch (enable) {
	case MAX25603_DISABLE_EN:
		ret = max25603_set_enable(desc, MAX25603_EN2, NO_OS_DIV_ROUND_CLOSEST_ULL(NANO,
					  desc->en2_desc->period_ns), 99);
		if (ret)
			return ret;

		ret = max25603_set_enable(desc, MAX25603_EN1, NO_OS_DIV_ROUND_CLOSEST_ULL(NANO,
					  desc->en1_desc->period_ns), 99);
		if (ret)
			return ret;

		break;
	case MAX25603_EN1:
		if (!duty) {
			ret = no_os_pwm_set_duty_cycle(desc->en1_desc, 0);
			if (ret)
				return ret;

			ret = no_os_pwm_disable(desc->en1_desc);
			if (ret)
				return ret;

			break;
		}

		ret = no_os_pwm_disable(desc->en1_desc);
		if (ret)
			return ret;

		ret = no_os_pwm_set_period(desc->en1_desc, period_ns);
		if (ret)
			return ret;

		ret = no_os_pwm_set_duty_cycle(desc->en1_desc,
					       (period_ns * duty) / 100);
		if (ret)
			return ret;

		ret = no_os_pwm_enable(desc->en1_desc);
		if (ret)
			return ret;

		break;
	case MAX25603_EN2:
		if (!duty) {
			ret = no_os_pwm_set_duty_cycle(desc->en2_desc, 0);
			if (ret)
				return ret;

			ret = no_os_pwm_disable(desc->en2_desc);
			if (ret)
				return ret;

			break;
		}

		ret = no_os_pwm_disable(desc->en2_desc);
		if (ret)
			return ret;

		ret = no_os_pwm_set_period(desc->en2_desc, period_ns);
		if (ret)
			return ret;

		ret = no_os_pwm_set_duty_cycle(desc->en2_desc,
					       (period_ns * duty) / 100);
		if (ret)
			return ret;

		ret = no_os_pwm_enable(desc->en2_desc);
		if (ret)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	desc->enable = enable;

	return 0;
}

int max25603_set_beam(struct max25603_desc *desc, enum max25603_beam beam,
		      uint32_t freq_hz, uint8_t duty)
{
	uint32_t period_ns;
	int ret;

	period_ns = NO_OS_DIV_ROUND_CLOSEST_ULL(NANO, freq_hz);

	switch (beam) {
	case MAX25603_DISABLE_BEAM:
		ret = max25603_set_beam(desc, MAX25603_LOW_BEAM,
					NO_OS_DIV_ROUND_CLOSEST_ULL(NANO, desc->lb_desc->period_ns), 99);
		if (ret)
			return ret;

		ret = max25603_set_beam(desc, MAX25603_HIGH_BEAM,
					NO_OS_DIV_ROUND_CLOSEST_ULL(NANO, desc->hb_desc->period_ns), 99);
		if (ret)
			return ret;

		break;
	case MAX25603_HIGH_BEAM:
		if (!duty) {
			ret = no_os_pwm_set_duty_cycle(desc->hb_desc, desc->hb_desc->period_ns);
			if (ret)
				return ret;

			ret = no_os_pwm_disable(desc->hb_desc);
			if (ret)
				return ret;

			break;
		}

		ret = no_os_pwm_disable(desc->hb_desc);
		if (ret)
			return ret;

		ret = no_os_pwm_set_period(desc->hb_desc, period_ns);
		if (ret)
			return ret;

		ret = no_os_pwm_set_duty_cycle(desc->hb_desc,
					       (period_ns * duty) / 100);
		if (ret)
			return ret;

		ret = no_os_pwm_enable(desc->hb_desc);
		if (ret)
			return ret;

		break;
	case MAX25603_LOW_BEAM:
		if (!duty) {
			ret = no_os_pwm_set_duty_cycle(desc->lb_desc, desc->lb_desc->period_ns);
			if (ret)
				return ret;

			ret = no_os_pwm_disable(desc->lb_desc);
			if (ret)
				return ret;

			break;
		}

		ret = no_os_pwm_disable(desc->lb_desc);
		if (ret)
			return ret;

		ret = no_os_pwm_set_period(desc->lb_desc, period_ns);
		if (ret)
			return ret;

		ret = no_os_pwm_set_duty_cycle(desc->lb_desc,
					       (period_ns * duty) / 100);
		if (ret)
			return ret;

		ret = no_os_pwm_enable(desc->lb_desc);
		if (ret)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	desc->beam = beam;

	return 0;
}

int max25603_init(struct max25603_desc **desc,
		  struct max25603_init_param *init_param)
{
	struct max25603_desc *descriptor;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_pwm_init(&descriptor->en1_desc, init_param->en1_param);
	if (ret)
		goto err;

	ret = no_os_pwm_init(&descriptor->en2_desc, init_param->en2_param);
	if (ret)
		goto en1_err;

	ret = no_os_pwm_init(&descriptor->hb_desc, init_param->hb_param);
	if (ret)
		goto en2_err;

	ret = no_os_pwm_init(&descriptor->lb_desc, init_param->lb_param);
	if (ret)
		goto hb_err;

	ret = max25603_set_enable(descriptor, MAX25603_DISABLE_EN, 0, 0);
	if (ret)
		goto lb_err;

	ret = max25603_set_beam(descriptor, MAX25603_DISABLE_BEAM, 0, 0);
	if (ret)
		goto hb_err;

	ret = no_os_gpio_get(&descriptor->shunt_desc, init_param->shunt_param);
	if (ret)
		goto hb_err;

	ret = no_os_gpio_direction_output(descriptor->shunt_desc,
					  NO_OS_GPIO_LOW);
	if (ret)
		goto shunt_err;

	descriptor->comp = MAX25603_COMP1;

	ret = no_os_gpio_get_optional(&descriptor->flt_desc,
				      init_param->flt_param);
	if (ret)
		goto shunt_err;

	if (descriptor->flt_desc) {
		ret = no_os_gpio_direction_input(descriptor->flt_desc);
		if (ret)
			goto flt_err;
	}

	*desc = descriptor;

	return 0;

flt_err:
	no_os_gpio_remove(descriptor->flt_desc);
shunt_err:
	no_os_gpio_remove(descriptor->shunt_desc);
lb_err:
	no_os_pwm_remove(descriptor->lb_desc);
hb_err:
	no_os_pwm_remove(descriptor->hb_desc);
en2_err:
	no_os_pwm_remove(descriptor->en2_desc);
en1_err:
	no_os_pwm_remove(descriptor->en1_desc);
err:
	no_os_free(descriptor);

	return ret;
}

int max25603_remove(struct max25603_desc *desc)
{
	if (!desc)
		return -ENODEV;

	no_os_gpio_remove(desc->flt_desc);
	no_os_gpio_remove(desc->shunt_desc);
	no_os_pwm_remove(desc->lb_desc);
	no_os_pwm_remove(desc->hb_desc);
	no_os_pwm_remove(desc->en2_desc);
	no_os_pwm_remove(desc->en1_desc);
	no_os_free(desc);

	return 0;
}
