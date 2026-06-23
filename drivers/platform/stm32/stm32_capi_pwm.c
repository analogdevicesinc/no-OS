/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <errno.h>
#include "capi_alloc.h"
#include "stm32_capi_pwm.h"
#include "stm32_capi_timer.h"

#ifdef HAL_TIM_MODULE_ENABLED

static int apply_pwm_config(struct stm32_capi_pwm_desc *desc)
{
	struct capi_timer_channel_config ch_cfg = {0};

	ch_cfg.mode = CAPI_TIMER_PWM_MODE;
	ch_cfg.config.pwm.period_ns = desc->period_ns;
	ch_cfg.config.pwm.active_ns = desc->duty_cycle_ns;
	ch_cfg.config.pwm.inverted_polarity = desc->inverted_polarity;

	return capi_timer_channel_config(desc->timer, desc->channel, &ch_cfg);
}

int stm32_capi_pwm_init(struct stm32_capi_pwm_desc **desc,
			const struct stm32_capi_pwm_config *config)
{
	struct stm32_capi_pwm_desc *pwm;
	struct stm32_capi_timer_extra_config extra = {0};
	struct capi_timer_config tim_cfg = {0};
	struct capi_timer_handle *timer = NULL;
	int ret;

	if (!desc || !config)
		return -EINVAL;

	pwm = capi_calloc(1, sizeof(*pwm));
	if (!pwm)
		return -ENOMEM;

	if (config->get_input_clock)
		extra.get_input_clock = config->get_input_clock;

	tim_cfg.ops = &stm32_capi_timer_ops;
	tim_cfg.identifier = config->timer_id;
	tim_cfg.input_clock_hz = config->input_clock_hz;
	tim_cfg.extra = &extra;

	ret = capi_timer_init(&timer, &tim_cfg);
	if (ret)
		goto err_free;

	ret = capi_timer_channel_init(timer, config->channel);
	if (ret)
		goto err_timer;

	pwm->timer = timer;
	pwm->channel = config->channel;
	pwm->period_ns = config->period_ns;
	pwm->duty_cycle_ns = config->duty_cycle_ns;
	pwm->inverted_polarity = config->inverted_polarity;
	pwm->enabled = false;

	ret = apply_pwm_config(pwm);
	if (ret)
		goto err_channel;

	*desc = pwm;
	return 0;

err_channel:
	capi_timer_channel_deinit(timer, config->channel);
err_timer:
	capi_timer_deinit(timer);
err_free:
	capi_free(pwm);
	return ret;
}

int stm32_capi_pwm_remove(struct stm32_capi_pwm_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (desc->enabled)
		capi_timer_channel_disable(desc->timer, desc->channel);

	capi_timer_channel_deinit(desc->timer, desc->channel);
	capi_timer_stop(desc->timer);
	capi_timer_deinit(desc->timer);
	capi_free(desc);

	return 0;
}

int stm32_capi_pwm_enable(struct stm32_capi_pwm_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = capi_timer_channel_enable(desc->timer, desc->channel);
	if (ret)
		return ret;

	ret = capi_timer_start(desc->timer);
	if (ret) {
		capi_timer_channel_disable(desc->timer, desc->channel);
		return ret;
	}

	desc->enabled = true;
	return 0;
}

int stm32_capi_pwm_disable(struct stm32_capi_pwm_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = capi_timer_channel_disable(desc->timer, desc->channel);
	if (ret)
		return ret;

	desc->enabled = false;
	return 0;
}

int stm32_capi_pwm_set_period(struct stm32_capi_pwm_desc *desc,
			      uint64_t period_ns)
{
	bool was_enabled;
	int ret;

	if (!desc)
		return -EINVAL;

	was_enabled = desc->enabled;
	if (was_enabled) {
		ret = stm32_capi_pwm_disable(desc);
		if (ret)
			return ret;
	}

	desc->period_ns = period_ns;
	ret = apply_pwm_config(desc);

	if (was_enabled && !ret)
		ret = stm32_capi_pwm_enable(desc);

	return ret;
}

int stm32_capi_pwm_get_period(struct stm32_capi_pwm_desc *desc,
			      uint64_t *period_ns)
{
	if (!desc || !period_ns)
		return -EINVAL;

	*period_ns = desc->period_ns;
	return 0;
}

int stm32_capi_pwm_set_duty_cycle(struct stm32_capi_pwm_desc *desc,
				  uint64_t duty_cycle_ns)
{
	bool was_enabled;
	int ret;

	if (!desc)
		return -EINVAL;

	was_enabled = desc->enabled;
	if (was_enabled) {
		ret = stm32_capi_pwm_disable(desc);
		if (ret)
			return ret;
	}

	desc->duty_cycle_ns = duty_cycle_ns;
	ret = apply_pwm_config(desc);

	if (was_enabled && !ret)
		ret = stm32_capi_pwm_enable(desc);

	return ret;
}

int stm32_capi_pwm_get_duty_cycle(struct stm32_capi_pwm_desc *desc,
				  uint64_t *duty_cycle_ns)
{
	if (!desc || !duty_cycle_ns)
		return -EINVAL;

	*duty_cycle_ns = desc->duty_cycle_ns;
	return 0;
}

int stm32_capi_pwm_set_polarity(struct stm32_capi_pwm_desc *desc,
				bool inverted)
{
	bool was_enabled;
	int ret;

	if (!desc)
		return -EINVAL;

	was_enabled = desc->enabled;
	if (was_enabled) {
		ret = stm32_capi_pwm_disable(desc);
		if (ret)
			return ret;
	}

	desc->inverted_polarity = inverted;
	ret = apply_pwm_config(desc);

	if (was_enabled && !ret)
		ret = stm32_capi_pwm_enable(desc);

	return ret;
}

int stm32_capi_pwm_get_polarity(struct stm32_capi_pwm_desc *desc,
				bool *inverted)
{
	if (!desc || !inverted)
		return -EINVAL;

	*inverted = desc->inverted_polarity;
	return 0;
}

#endif /* HAL_TIM_MODULE_ENABLED */
