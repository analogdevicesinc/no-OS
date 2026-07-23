/*******************************************************************************
 *   @file   maxim_capi_timer.c
 *   @brief  Implementation of Timer and PWM functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#include <errno.h>
#include <stdint.h>
#include "maxim_capi_irq.h"
#include "maxim_capi_timer.h"
#include "tmr.h"

/** Static variables **********************************************************/

static struct capi_timer_handle *timer[MXC_CFG_TMR_INSTANCES] = {NULL};

/** Forward declarations ******************************************************/

void max_capi_timer_isr(struct capi_timer_handle *handle);

/** Helper functions **********************************************************/

/**
 * @brief Get the prescaler for PWM mode
 * @param div Clock division value (this should be a power of 2)
 * @param prescaler enum value to be provided
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_timer_get_prescaler_for_pwm(uint32_t div,
		mxc_tmr_pres_t *prescaler)
{
	int i;

	if (!prescaler)
		return -EINVAL;

	i = 1;

	while (div / MAX_CAPI_PWM_GET_PRESCALER(i) > MAX_CAPI_PWM_TMR_MAX_VAL) {
		i++;
		if (i > MAX_CAPI_MAX_PRESCALER_INDEX)
			return -ERANGE;
	}

	*prescaler = MAX_CAPI_PWM_PRESCALER_VAL(i);

	return 0;
}

/**
 * @brief Get the prescaler for counter/compare modes
 * @param div Clock division value needed to achieve target frequency
 * @param prescaler enum value to be provided
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_timer_get_prescaler_for_counter(uint32_t div,
		mxc_tmr_pres_t *prescaler)
{
	int i;

	if (!prescaler)
		return -EINVAL;

	for (i = 1; i <= MAX_CAPI_MAX_PRESCALER_INDEX; i++) {
		if (MAX_CAPI_PWM_GET_PRESCALER(i) >= div) {
			*prescaler = MAX_CAPI_PWM_PRESCALER_VAL(i);
			return 0;
		}
	}

	return -ERANGE;
}

/**
 * @brief Configure a GPIO pin based on the identifier and channel
 * @param identifier Timer ID
 * @param use_alternate_pin Whether to use the alternate pin or not
 * @param vssel Voltage for the GPIO pin
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_timer_configure_gpio(uint32_t identifier,
		bool use_alternate_pin,
		enum max_capi_gpio_vssel vssel)
{
	mxc_gpio_cfg_t gpio_config;

	if (!use_alternate_pin) {
		if (identifier == 0)
			gpio_config = gpio_cfg_tmr0;
		else if (identifier == 1)
			gpio_config = gpio_cfg_tmr1;
		else if (identifier == 2)
			gpio_config = gpio_cfg_tmr2;
		else if (identifier == 3)
			gpio_config = gpio_cfg_tmr3;
		else if (identifier == 4)
			gpio_config = gpio_cfg_tmr4;
		else if (identifier == 5)
			gpio_config = gpio_cfg_tmr5;
		else
			return -EINVAL;
	} else {
		if (identifier == 0)
			gpio_config = gpio_cfg_tmr0b;
		else if (identifier == 2)
			return -ENOTSUP;
		else if (identifier == 1)
			gpio_config = gpio_cfg_tmr1b;
		else if (identifier == 3)
			gpio_config = gpio_cfg_tmr3b;
		else if (identifier == 4)
			gpio_config = gpio_cfg_tmr4b;
		else if (identifier == 5)
			gpio_config = gpio_cfg_tmr5b;
		else
			return -EINVAL;
	}

	gpio_config.vssel = (mxc_gpio_vssel_t)vssel;

	return MXC_GPIO_Config(&gpio_config);
}

/**
 * @brief Get clock frequency given a clock source
 * @param clock_source The clock source (from enum max_capi_timer_clock_source)
 * @param freq_hz Where to store the frequency
 * @param msdk_clock Where to store the msdk clock identifier
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_timer_get_clock_frequency(enum max_capi_timer_clock_source
		clock_source,
		uint32_t *freq_hz, mxc_tmr_clock_t *msdk_clock)
{
	if (!freq_hz || !msdk_clock)
		return -EINVAL;

	switch (clock_source) {
	case MAX_CAPI_TIMER_CLOCK_APB:
		*freq_hz = PeripheralClock;
		*msdk_clock = MXC_TMR_APB_CLK;
		return 0;

	case MAX_CAPI_TIMER_CLOCK_IBRO:
		*freq_hz = IBRO_FREQ;
		*msdk_clock = MXC_TMR_IBRO_CLK;
		return 0;

	case MAX_CAPI_TIMER_CLOCK_ERTCO:
		*freq_hz = ERTCO_FREQ;
		*msdk_clock = MXC_TMR_ERTCO_CLK;
		return 0;

	case MAX_CAPI_TIMER_CLOCK_EXTERNAL:
	case MAX_CAPI_TIMER_CLOCK_INRO:
	case MAX_CAPI_TIMER_CLOCK_IBRO_DIV8:
		return -ENOTSUP;

	default:
		return -EINVAL;
	}
}

/** Timer functions implementation ********************************************/

/**
 * @brief Initialize the timer peripheral. Defaults to 32-bit mode if not
 * 	  specified in the extra init params.
 * @param handle The timer handle
 * @param config The timer config
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_init(struct capi_timer_handle **handle,
			const struct capi_timer_config *config)
{
	int ret;
	struct capi_timer_handle *timer_handle;
	struct max_capi_timer_priv *timer_priv;
	const struct max_capi_timer_extra *timer_extra;
	enum max_capi_timer_bit_mode bit_mode = MAX_CAPI_TIMER_BIT_MODE_32BIT;
	enum max_capi_timer_clock_source clock_source = MAX_CAPI_TIMER_CLOCK_APB;
	mxc_tmr_clock_t msdk_clock;
	uint32_t clock_hz;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier >= MXC_CFG_TMR_INSTANCES)
		return -EINVAL;

	if (timer[config->identifier] != NULL) {
		*handle = timer[config->identifier];
		return 0;
	}

	if (config->output_freq_hz == 0)
		return -EINVAL;

	if (*handle == NULL) {
		timer_handle = capi_calloc(1, sizeof(*timer_handle));
		if (!timer_handle)
			return -ENOMEM;
		timer_handle->init_allocated = true;
	} else {
		timer_handle = *handle;
		timer_handle->init_allocated = false;
	}

	timer_priv = capi_calloc(1, sizeof(*timer_priv));
	if (!timer_priv) {
		ret = -ENOMEM;
		goto free_handle;
	}

	timer_handle->priv = timer_priv;
	timer_handle->ops = config->ops;

	if (config->input_clock_identifier)
		clock_source = config->input_clock_identifier;

	ret = _max_capi_timer_get_clock_frequency(clock_source, &clock_hz, &msdk_clock);
	if (ret)
		goto free_priv;

	timer_priv->clock_source = clock_source;
	timer_priv->clock_freq_hz = clock_hz;
	timer_priv->msdk_clock = msdk_clock;

	timer_priv->identifier = config->identifier;
	timer_priv->frequency = config->output_freq_hz;

	if (config->extra) {
		timer_extra = config->extra;
		bit_mode = timer_extra->bit_mode;
		if (bit_mode != MAX_CAPI_TIMER_BIT_MODE_32BIT &&
		    bit_mode != MAX_CAPI_TIMER_BIT_MODE_16BIT_DUAL) {
			ret = -EINVAL;
			goto free_priv;
		}
	}

	struct capi_irq_config irq_config = {
		.irq_ctrl_id = 0,
	};

	ret = capi_irq_init(&irq_config);
	if (ret)
		goto free_priv;

	IRQn_Type timer_irq = MXC_TMR_GET_IRQ(timer_priv->identifier);
	ret = capi_irq_connect(timer_irq, max_capi_timer_isr, timer_handle);
	if (ret)
		goto free_priv;

	ret = capi_irq_enable(timer_irq);
	if (ret)
		goto disconnect_irq;

	timer_priv->bit_mode = bit_mode;

	timer_priv->global_callback = NULL;
	timer_priv->global_callback_arg = NULL;
	timer_priv->global_events_enabled = 0;

	timer_priv->channel[0] = NULL;
	timer_priv->channel[1] = NULL;

	timer[config->identifier] = timer_handle;
	*handle = timer_handle;

	return 0;

disconnect_irq:
	capi_irq_connect(MXC_TMR_GET_IRQ(timer_priv->identifier), NULL, NULL);
free_priv:
	capi_free(timer_priv);
free_handle:
	if (timer_handle->init_allocated)
		capi_free(timer_handle);

	timer[config->identifier] = NULL;

	return ret;
}

/**
 * @brief Deinitialize the timer peripheral
 * @param handle The timer handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_deinit(struct capi_timer_handle *handle)
{
	struct max_capi_timer_priv *timer_priv;
	mxc_tmr_regs_t *tmr_reg;
	uint8_t id;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);
	id = timer_priv->identifier;

	MXC_TMR_Stop(tmr_reg);
	MXC_TMR_DisableInt(tmr_reg);
	MXC_TMR_Shutdown(tmr_reg);

	if (timer_priv->channel[0]) {
		capi_free(timer_priv->channel[0]);
		timer_priv->channel[0] = NULL;
	}
	if (timer_priv->channel[1]) {
		capi_free(timer_priv->channel[1]);
		timer_priv->channel[1] = NULL;
	}

	capi_irq_disable(MXC_TMR_GET_IRQ(timer_priv->identifier));
	capi_irq_connect(MXC_TMR_GET_IRQ(timer_priv->identifier), NULL, NULL);

	capi_free(handle->priv);

	if (handle->init_allocated)
		capi_free(handle);

	timer[id] = NULL;

	return 0;
}

/**
 * @brief Timer start function
 * @param handle The timer handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_start(struct capi_timer_handle *handle)
{
	struct max_capi_timer_priv *timer_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;

	MXC_TMR_Start(MXC_TMR_GET_TMR(timer_priv->identifier));

	return 0;
}

/**
 * @brief Timer stop function
 * @param handle The timer handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_stop(struct capi_timer_handle *handle)
{
	struct max_capi_timer_priv *timer_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;

	MXC_TMR_Stop(MXC_TMR_GET_TMR(timer_priv->identifier));

	return 0;
}

/**
 * @brief Configure the counter for a timer
 * @param handle The timer handle
 * @param config The timer counter config
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_counter_config(struct capi_timer_handle *handle,
				  const struct capi_timer_counter_config *config)
{
	int ret;
	struct max_capi_timer_priv *timer_priv;

	if (!handle || !handle->priv || !config)
		return -EINVAL;

	timer_priv = handle->priv;

	if (config->direction == CAPI_TIMER_COUNT_DOWN)
		return -ENOTSUP;

	if (!timer_priv->channel[0]) {
		ret = max_capi_timer_channel_init(handle, 0);
		if (ret)
			return ret;
	}

	if (config->rollover)
		timer_priv->channel[0]->config.mode = MXC_TMR_MODE_CONTINUOUS;
	else
		timer_priv->channel[0]->config.mode = MXC_TMR_MODE_ONESHOT;

	if (config->max > 0) {
		if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_16BIT_DUAL
		    && config->max > UINT16_MAX)
			return -EINVAL;

		timer_priv->channel[0]->config.cmp_cnt = config->max;
		timer_priv->channel[0]->compare.value = config->max;
	}

	if (!timer_priv->channel[0]->enabled) {
		ret = max_capi_timer_channel_enable(handle, 0);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Get the value of a timer counter
 * @param handle The timer handle
 * @param counter Pointer to where the value will be stored
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_counter_get(struct capi_timer_handle *handle,
			       uint32_t *counter)
{
	struct max_capi_timer_priv *timer_priv;

	if (!handle || !handle->priv || !counter)
		return -EINVAL;

	timer_priv = handle->priv;

	*counter = MXC_TMR_GetCount(MXC_TMR_GET_TMR(timer_priv->identifier));

	return 0;
}

/**
 * @brief Initialize a channel - allocates memory and sets defaults
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_init(struct capi_timer_handle *handle, uint32_t chan)
{
	int ret;
	struct max_capi_timer_priv *timer_priv;
	uint32_t prescaler_div;
	mxc_tmr_pres_t prescaler;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;
	}

	if (timer_priv->channel[chan]) {
		capi_free(timer_priv->channel[chan]);
		timer_priv->channel[chan] = NULL;
	}

	timer_priv->channel[chan] = capi_calloc(1,
						sizeof(struct max_capi_timer_channel_state));
	if (!timer_priv->channel[chan])
		return -ENOMEM;

	prescaler_div = timer_priv->clock_freq_hz / timer_priv->frequency;
	ret = _max_capi_timer_get_prescaler_for_counter(prescaler_div, &prescaler);
	if (ret)
		goto free_channel;

	timer_priv->frequency = timer_priv->clock_freq_hz /
				MAX_CAPI_PWM_PRESCALER_TRUE(prescaler);

	timer_priv->channel[chan]->config.pres = prescaler;
	timer_priv->channel[chan]->config.mode = MXC_TMR_MODE_CONTINUOUS;
	timer_priv->channel[chan]->config.clock = timer_priv->msdk_clock;
	timer_priv->channel[chan]->config.cmp_cnt = 1;
	timer_priv->channel[chan]->config.pol = 0;
	timer_priv->channel[chan]->vssel = MAX_CAPI_GPIO_VSSEL_VDDIO;
	timer_priv->channel[chan]->enabled = false;

	return 0;

free_channel:
	capi_free(timer_priv->channel[chan]);
	timer_priv->channel[chan] = NULL;

	return ret;
}

/**
 * @brief Deinitialize a channel - frees allocated memory
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_deinit(struct capi_timer_handle *handle,
				  uint32_t chan)
{
	struct max_capi_timer_priv *timer_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;
	}

	if (!timer_priv->channel[chan])
		return 0;

	if (timer_priv->channel[chan]->enabled) {
		mxc_tmr_regs_t *tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);
		if (chan == 0) {
			tmr_reg->ctrl0 &= ~MXC_F_TMR_CTRL0_EN_A;
			tmr_reg->ctrl1 &= ~MXC_F_TMR_CTRL1_IE_A;
		} else {
			tmr_reg->ctrl0 &= ~MXC_F_TMR_CTRL0_EN_B;
			tmr_reg->ctrl1 &= ~MXC_F_TMR_CTRL1_IE_B;
		}

		timer_priv->channel[chan]->enabled = false;
	}

	capi_free(timer_priv->channel[chan]);
	timer_priv->channel[chan] = NULL;

	return 0;
}

/**
 * @brief Configure a channel - stores configuration in memory without applying
 * 	  to hardware yet.
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @param ch_config The configuration for the channel
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_config(struct capi_timer_handle *handle,
				  uint32_t chan,
				  const struct capi_timer_channel_config *ch_config)
{
	int ret;
	struct max_capi_timer_priv *timer_priv;
	const struct max_capi_timer_channel_extra *channel_extra;
	enum max_capi_gpio_vssel vssel;
	mxc_tmr_regs_t *tmr_reg;
	uint32_t freq_period, low_time, freq_duty, prescaler_div;
	uint32_t period_ticks, duty_ticks;
	mxc_tmr_pres_t prescaler;

	if (!handle || !handle->priv || !ch_config)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;
	}

	if (!timer_priv->channel[chan])
		return -EINVAL;

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		timer_priv->channel[chan]->config.bitMode = MXC_TMR_BIT_MODE_32;
	} else {
		if (chan == 0) {
			timer_priv->channel[chan]->config.bitMode = MXC_TMR_BIT_MODE_16A;
		} else {
			if (ch_config->mode != CAPI_TIMER_COMPARE_MODE &&
			    ch_config->mode != MAX_CAPI_TIMER_MODE_ONESHOT &&
			    ch_config->mode != MAX_CAPI_TIMER_MODE_CONTINUOUS) {
				return -ENOTSUP;
			}

			timer_priv->channel[chan]->config.bitMode = MXC_TMR_BIT_MODE_16B;
		}
	}

	timer_priv->channel[chan]->init_pin = false;
	timer_priv->channel[chan]->use_alternate_pin = false;

	switch (ch_config->mode) {
	case CAPI_TIMER_COMPARE_MODE:
		timer_priv->channel[chan]->config.mode = MXC_TMR_MODE_COMPARE;
		timer_priv->channel[chan]->compare.value =
			ch_config->config.compare.match_value;
		timer_priv->channel[chan]->init_pin =
			ch_config->config.compare.generate_pulse_on_match;

		/* Polarity validation only matters when actually generating output */
		if (ch_config->config.compare.generate_pulse_on_match &&
		    ch_config->config.compare.polarity != CAPI_TIMER_ON_COMPARE_TOGGLE)
			return -EINVAL;
		break;

	case CAPI_TIMER_PWM_MODE:
		timer_priv->channel[chan]->config.mode = MXC_TMR_MODE_PWM;

		if (ch_config->config.pwm.period_ns == 0)
			return -EINVAL;
		if (ch_config->config.pwm.active_ns > ch_config->config.pwm.period_ns)
			return -EINVAL;

		freq_period = (1000000000UL / ch_config->config.pwm.period_ns);
		if (freq_period == 0)
			return -EINVAL;

		prescaler_div = timer_priv->clock_freq_hz / freq_period;
		ret = _max_capi_timer_get_prescaler_for_pwm(prescaler_div, &prescaler);
		if (ret)
			return ret;

		period_ticks = MXC_TMR_GetPeriod(tmr_reg,
						 timer_priv->msdk_clock,
						 MAX_CAPI_PWM_PRESCALER_TRUE(prescaler),
						 freq_period);

		if (ch_config->config.pwm.active_ns == ch_config->config.pwm.period_ns) {
			/* 100% duty cycle */
			duty_ticks = period_ticks;
		} else {
			low_time = ch_config->config.pwm.period_ns - ch_config->config.pwm.active_ns;
			freq_duty = (1000000000UL / low_time);
			duty_ticks = MXC_TMR_GetPeriod(tmr_reg,
						       timer_priv->msdk_clock,
						       MAX_CAPI_PWM_PRESCALER_TRUE(prescaler),
						       freq_duty);
		}

		timer_priv->channel[chan]->pwm.period_ticks = period_ticks;
		timer_priv->channel[chan]->pwm.duty_cycle_ticks = duty_ticks;

		timer_priv->channel[chan]->config.pres = prescaler;
		timer_priv->channel[chan]->config.cmp_cnt = period_ticks;
		timer_priv->channel[chan]->config.pol = ch_config->config.pwm.inverted_polarity;

		timer_priv->channel[chan]->init_pin = true;
		break;

	case MAX_CAPI_TIMER_MODE_ONESHOT:
		timer_priv->channel[chan]->config.mode = MXC_TMR_MODE_ONESHOT;
		timer_priv->channel[chan]->compare.value =
			ch_config->config.compare.match_value;
		timer_priv->channel[chan]->init_pin =
			ch_config->config.compare.generate_pulse_on_match;
		break;

	case MAX_CAPI_TIMER_MODE_CONTINUOUS:
		timer_priv->channel[chan]->config.mode = MXC_TMR_MODE_CONTINUOUS;
		timer_priv->channel[chan]->compare.value =
			ch_config->config.compare.match_value;
		timer_priv->channel[chan]->init_pin =
			ch_config->config.compare.generate_pulse_on_match;
		break;

	/**
	 * These timer modes require a timer input pin, but there
	 * doesn't seem to be documentation regarding this. Perhaps
	 * there aren't timer input pins on the MAX32657?
	 */
	case CAPI_TIMER_CAPTURE_MODE:
	case MAX_CAPI_TIMER_MODE_DUAL_EDGE:
	case MAX_CAPI_TIMER_MODE_GATED:
	case MAX_CAPI_TIMER_MODE_CAPTURE_COMPARE:
	case MAX_CAPI_TIMER_MODE_COUNTER:
		return -ENOTSUP;

	default:
		return -EINVAL;
	}

	vssel = MAX_CAPI_GPIO_VSSEL_VDDIO;
	if (ch_config->extra) {
		channel_extra = ch_config->extra;
		vssel = channel_extra->vssel;
		timer_priv->channel[chan]->init_pin = channel_extra->init_pin;
		timer_priv->channel[chan]->use_alternate_pin = channel_extra->use_alternate_pin;
	}
	timer_priv->channel[chan]->vssel = vssel;

	if (chan == 1 && timer_priv->channel[chan]->init_pin)
		return -ENOTSUP;

	return 0;
}

/**
 * @brief Enable a channel - applies stored configuration
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_enable(struct capi_timer_handle *handle,
				  uint32_t chan)
{
	int ret;
	struct max_capi_timer_priv *timer_priv;
	mxc_tmr_regs_t *tmr_reg;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);

	if (!timer_priv->channel[chan])
		return -EINVAL;

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;

		if (chan == 1) {
			if (timer_priv->channel[chan]->config.mode != MXC_TMR_MODE_COMPARE &&
			    timer_priv->channel[chan]->config.mode != MXC_TMR_MODE_ONESHOT &&
			    timer_priv->channel[chan]->config.mode != MXC_TMR_MODE_CONTINUOUS) {
				return -ENOTSUP;
			}
		}
	}

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		ret = MXC_TMR_Init(tmr_reg, &timer_priv->channel[chan]->config, false);
		if (ret)
			return ret;
	} else {
		uint32_t other_channel = (chan == 0) ? 1 : 0;
		bool other_enabled = (timer_priv->channel[other_channel] &&
				      timer_priv->channel[other_channel]->enabled);

		if (!other_enabled) {
			ret = MXC_TMR_Init(tmr_reg, &timer_priv->channel[chan]->config, false);
			if (ret)
				return ret;
		} else {
			uint32_t mask = (other_channel == 0) ? 0x0000FFFF : 0xFFFF0000;
			uint32_t ctrl0 = tmr_reg->ctrl0 & mask;
			uint32_t ctrl1 = tmr_reg->ctrl1 & mask;
			uint32_t cmp = tmr_reg->cmp & mask;
			uint32_t cnt = tmr_reg->cnt & mask;

			ret = MXC_TMR_Init(tmr_reg, &timer_priv->channel[chan]->config, false);
			if (ret)
				return ret;

			tmr_reg->ctrl0 = (tmr_reg->ctrl0 & ~mask) | ctrl0;
			tmr_reg->ctrl1 = (tmr_reg->ctrl1 & ~mask) | ctrl1;
			tmr_reg->cmp = (tmr_reg->cmp & ~mask) | cmp;
			tmr_reg->cnt = (tmr_reg->cnt & ~mask) | cnt;
		}
	}

	switch (timer_priv->channel[chan]->config.mode) {
	case MXC_TMR_MODE_PWM:
		ret = MXC_TMR_SetPWM(tmr_reg, timer_priv->channel[chan]->pwm.duty_cycle_ticks);
		if (ret)
			return ret;
		break;

	case MXC_TMR_MODE_COMPARE:
	case MXC_TMR_MODE_ONESHOT:
	case MXC_TMR_MODE_CONTINUOUS:
		if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
			MXC_TMR_SetCompare(tmr_reg, timer_priv->channel[chan]->compare.value);
		} else {
			uint32_t compare = timer_priv->channel[chan]->compare.value;
			if (compare > UINT16_MAX)
				return -EINVAL;

			uint32_t cmp_reg = MXC_TMR_GetCompare(tmr_reg);
			if (chan == 0) {
				cmp_reg &= ~0x0000FFFF;
				cmp_reg |= compare;
			} else {
				cmp_reg &= ~0xFFFF0000;
				cmp_reg |= compare << 16;
			}
			MXC_TMR_SetCompare(tmr_reg, cmp_reg);
		}
		break;

	case MXC_TMR_MODE_CAPTURE:
	case MXC_TMR_MODE_CAPTURE_COMPARE:
	case MXC_TMR_MODE_DUAL_EDGE:
	case MXC_TMR_MODE_GATED:
	case MXC_TMR_MODE_COUNTER:
		return -ENOTSUP;

	default:
		break;
	}

	if (timer_priv->channel[chan]->init_pin
	    && !timer_priv->channel[chan]->gpio_initialized) {
		ret = _max_capi_timer_configure_gpio(timer_priv->identifier,
						     timer_priv->channel[chan]->use_alternate_pin,
						     timer_priv->channel[chan]->vssel);
		if (ret)
			return ret;

		timer_priv->channel[chan]->gpio_initialized = true;
	}

	timer_priv->channel[chan]->enabled = true;

	return 0;
}

/**
 * @brief Disable a channel - stops the specific timer
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_disable(struct capi_timer_handle *handle,
				   uint32_t chan)
{
	struct max_capi_timer_priv *timer_priv;
	mxc_tmr_regs_t *tmr_reg;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;
	}

	if (!timer_priv->channel[chan])
		return -EINVAL;

	if (chan == 0)
		tmr_reg->ctrl0 &= ~MXC_F_TMR_CTRL0_CLKEN_A;
	else
		tmr_reg->ctrl0 &= ~MXC_F_TMR_CTRL0_CLKEN_B;

	timer_priv->channel[chan]->enabled = false;

	return 0;
}

/**
 * @brief Set the compare value for a channel
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @param compare The compare value
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_compare_set(struct capi_timer_handle *handle,
				       uint32_t chan, uint32_t compare)
{
	struct max_capi_timer_priv *timer_priv;
	mxc_tmr_regs_t *tmr_reg;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;
		if (compare > UINT16_MAX)
			return -EINVAL;
	}

	if (!timer_priv->channel[chan])
		return -EINVAL;

	timer_priv->channel[chan]->compare.value = compare;

	if (timer_priv->channel[chan]->enabled) {
		if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
			MXC_TMR_SetCompare(tmr_reg, compare);
		} else {
			uint32_t cmp_reg = MXC_TMR_GetCompare(tmr_reg);
			if (chan == 0) {
				cmp_reg &= ~0x0000FFFF;
				cmp_reg |= compare;
			} else {
				cmp_reg &= ~0xFFFF0000;
				cmp_reg |= compare << 16;
			}
			MXC_TMR_SetCompare(tmr_reg, cmp_reg);
		}
	}

	return 0;
}

/**
 * @brief Get the compare value for a channel
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @param compare Where to store the compare value
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_compare_get(struct capi_timer_handle *handle,
				       uint32_t chan, uint32_t *compare)
{
	struct max_capi_timer_priv *timer_priv;
	mxc_tmr_regs_t *tmr_reg;

	if (!handle || !handle->priv || !compare)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;
	}

	if (!timer_priv->channel[chan])
		return -EINVAL;

	*compare = timer_priv->channel[chan]->compare.value;

	return 0;
}

/**
 * @brief Get the capture value for a channel.
 * 	  Since capture mode is not supported because it requires an input pin,
 * 	  this returns -ENOTSUP instead.
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @param capture Where to store the capture value
 * @return -ENOTSUP
 */
int max_capi_timer_channel_capture_get(struct capi_timer_handle *handle,
				       uint32_t chan, uint32_t *capture)
{
	return -ENOTSUP;
}

/**
 * @brief Convert nsec to ticks
 * @param handle The timer handle
 * @param duration_ns The duration in nanoseconds
 * @param ticks Where to store the ticks for given duration_ns
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_nsec_to_ticks(const struct capi_timer_handle *handle,
				 uint64_t duration_ns, uint32_t *ticks)
{
	const struct max_capi_timer_priv *timer_priv;
	uint64_t ns_per_tick, result;

	if (!handle || !handle->priv || !ticks)
		return -EINVAL;

	timer_priv = handle->priv;

	ns_per_tick = (1000000000UL / timer_priv->frequency);
	result = (duration_ns / ns_per_tick);

	if (result > UINT32_MAX)
		return -EOVERFLOW;

	*ticks = (uint32_t)result;

	return 0;
}

/**
 * @brief Convert ticks to nsec
 * @param handle The timer handle
 * @param ticks The number of ticks
 * @param duration_ns Where to store the nanoseconds for given number of ticks
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_ticks_to_nsec(const struct capi_timer_handle *handle,
				 uint64_t ticks, uint32_t *duration_ns)
{
	const struct max_capi_timer_priv *timer_priv;
	uint64_t ns_per_tick, result;

	if (!handle || !handle->priv || !duration_ns)
		return -EINVAL;

	timer_priv = handle->priv;

	ns_per_tick = (1000000000UL / timer_priv->frequency);
	result = ticks * ns_per_tick;

	if (result > UINT32_MAX)
		return -EOVERFLOW;

	*duration_ns = (uint32_t)result;

	return 0;
}

/**
 * @brief Enable an event
 * @param handle The timer handle
 * @param event The event to enable
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_event_irq_enable(struct capi_timer_handle *handle,
				    uint32_t event)
{
	struct max_capi_timer_priv *timer_priv;
	mxc_tmr_regs_t *tmr_reg;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);

	if (event >= CAPI_TIMER_GLOBAL_EVENT_LIMIT)
		return -EINVAL;

	switch (event) {
	case CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW:
		break;
	default:
		return -EINVAL;
	}

	tmr_reg->ctrl1 |= MXC_F_TMR_CTRL1_IE_A;
	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_16BIT_DUAL)
		tmr_reg->ctrl1 |= MXC_F_TMR_CTRL1_IE_B;

	timer_priv->global_events_enabled |= (1 << event);

	return 0;
}

/**
 * @brief Disable an event
 * @param handle The timer handle
 * @param event The event to disable
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_event_irq_disable(struct capi_timer_handle *handle,
				     uint32_t event)
{
	struct max_capi_timer_priv *timer_priv;
	mxc_tmr_regs_t *tmr_reg;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);

	if (event >= CAPI_TIMER_GLOBAL_EVENT_LIMIT)
		return -EINVAL;

	switch (event) {
	case CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW:
		break;
	default:
		return -EINVAL;
	}

	timer_priv->global_events_enabled &= ~(1 << event);

	if (timer_priv->global_events_enabled == 0) {
		bool has_channel_events = false;
		if (timer_priv->channel[0] && timer_priv->channel[0]->events_enabled)
			has_channel_events = true;
		if (timer_priv->channel[1] && timer_priv->channel[1]->events_enabled)
			has_channel_events = true;

		if (!has_channel_events) {
			tmr_reg->ctrl1 &= ~MXC_F_TMR_CTRL1_IE_A;
			if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_16BIT_DUAL)
				tmr_reg->ctrl1 &= ~MXC_F_TMR_CTRL1_IE_B;
		}
	}

	return 0;
}

/**
 * @brief Register a callback for global events
 * @param handle The timer handle
 * @param callback The callback to register
 * @param callback_arg Argument to pass to the callback
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_register_event_callback(struct capi_timer_handle *handle,
		capi_timer_event_callback callback,
		void *callback_arg)
{
	struct max_capi_timer_priv *timer_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;

	timer_priv->global_callback = callback;
	timer_priv->global_callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Enable interrupts for a specific channel
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @param event The event to enable
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_irq_enable(struct capi_timer_handle *handle,
				      uint32_t chan, uint32_t event)
{
	struct max_capi_timer_priv *timer_priv;
	mxc_tmr_regs_t *tmr_reg;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;
	}

	if (!timer_priv->channel[chan])
		return -EINVAL;

	if (event >= CAPI_TIMER_CHANNEL_EVENT_LIMIT)
		return -EINVAL;

	switch (event) {
	case CAPI_TIMER_CHANNEL_EVENT_COMPARE:
		break;
	case CAPI_TIMER_CHANNEL_EVENT_CAPTURE:
		return -ENOTSUP;
	default:
		return -EINVAL;
	}

	timer_priv->channel[chan]->events_enabled |= (1 << event);

	if (chan == 0)
		tmr_reg->ctrl1 |= MXC_F_TMR_CTRL1_IE_A;
	else
		tmr_reg->ctrl1 |= MXC_F_TMR_CTRL1_IE_B;

	return 0;
}

/**
 * @brief Disable interrupts for a specific channel
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @param event The event to disable
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_irq_disable(struct capi_timer_handle *handle,
				       uint32_t chan, uint32_t event)
{
	struct max_capi_timer_priv *timer_priv;
	mxc_tmr_regs_t *tmr_reg;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;
	tmr_reg = MXC_TMR_GET_TMR(timer_priv->identifier);

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;
	}

	if (!timer_priv->channel[chan])
		return -EINVAL;

	if (event >= CAPI_TIMER_CHANNEL_EVENT_LIMIT)
		return -EINVAL;

	switch (event) {
	case CAPI_TIMER_CHANNEL_EVENT_COMPARE:
		break;
	case CAPI_TIMER_CHANNEL_EVENT_CAPTURE:
		return -ENOTSUP;
	default:
		return -EINVAL;
	}

	timer_priv->channel[chan]->events_enabled &= ~(1 << event);

	if (timer_priv->channel[chan]->events_enabled == 0) {
		if (chan == 0)
			tmr_reg->ctrl1 &= ~MXC_F_TMR_CTRL1_IE_A;
		else
			tmr_reg->ctrl1 &= ~MXC_F_TMR_CTRL1_IE_B;
	}

	return 0;
}

/**
 * @brief Register a callback for a specific channel
 * @param handle The timer handle
 * @param chan The channel (0 = Timer A, 1 = Timer B)
 * @param callback The callback to register
 * @param callback_arg Argument to pass to the callback
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_channel_register_callback(struct capi_timer_handle *handle,
		uint32_t chan,
		capi_timer_channel_callback callback,
		void *callback_arg)
{
	struct max_capi_timer_priv *timer_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	timer_priv = handle->priv;

	if (timer_priv->bit_mode == MAX_CAPI_TIMER_BIT_MODE_32BIT) {
		if (chan != 0)
			return -EINVAL;
	} else {
		if (chan > 1)
			return -EINVAL;
	}

	if (!timer_priv->channel[chan])
		return -EINVAL;

	timer_priv->channel[chan]->callback = callback;
	timer_priv->channel[chan]->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Check if an IRQ is pending
 * @param handle The timer handle
 * @param pending Where to store if pending or not
 * @return 0 on success, negative error code otherwise
 */
int max_capi_timer_is_irq_pending(struct capi_timer_handle *handle,
				  bool *pending)
{
	struct max_capi_timer_priv *timer_priv;
	uint32_t flags;

	if (!handle || !handle->priv || !pending)
		return -EINVAL;

	timer_priv = handle->priv;

	flags = MXC_TMR_GetFlags(MXC_TMR_GET_TMR(timer_priv->identifier));

	*pending = (flags & (MXC_F_TMR_INTFL_IRQ_A | MXC_F_TMR_INTFL_IRQ_B));

	return 0;
}

/**
 * @brief The ISR for the timer peripheral
 * @param handle The timer handle
 */
void max_capi_timer_isr(struct capi_timer_handle *handle)
{
	struct max_capi_timer_priv *timer_priv;
	uint32_t flags;

	if (!handle || !handle->priv)
		return;

	timer_priv = handle->priv;
	flags = MXC_TMR_GetFlags(MXC_TMR_GET_TMR(timer_priv->identifier));

	if (flags & MXC_F_TMR_INTFL_IRQ_A) {
		if ((timer_priv->global_events_enabled & (1 <<
				CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW))
		    && timer_priv->global_callback) {
			timer_priv->global_callback(CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW,
						    timer_priv->global_callback_arg,
						    0);
		}

		if (timer_priv->channel[0]
		    && (timer_priv->channel[0]->events_enabled) & (1 <<
				    CAPI_TIMER_CHANNEL_EVENT_COMPARE)
		    && timer_priv->channel[0]->callback) {
			timer_priv->channel[0]->callback(CAPI_TIMER_CHANNEL_EVENT_COMPARE,
							 0,
							 timer_priv->channel[0]->callback_arg,
							 0);
		}
	}

	if (flags & MXC_F_TMR_INTFL_IRQ_B) {
		if ((timer_priv->global_events_enabled & (1 <<
				CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW))
		    && timer_priv->global_callback) {
			timer_priv->global_callback(CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW,
						    timer_priv->global_callback_arg,
						    1);
		}

		if (timer_priv->channel[1]
		    && (timer_priv->channel[1]->events_enabled & (1 <<
				    CAPI_TIMER_CHANNEL_EVENT_COMPARE))
		    && timer_priv->channel[1]->callback) {
			timer_priv->channel[1]->callback(CAPI_TIMER_CHANNEL_EVENT_COMPARE,
							 1,
							 timer_priv->channel[1]->callback_arg,
							 0);
		}
	}

	MXC_TMR_ClearFlags(MXC_TMR_GET_TMR(timer_priv->identifier));
}

struct capi_timer_ops max_capi_timer_ops = {
	.init = max_capi_timer_init,
	.deinit = max_capi_timer_deinit,
	.start = max_capi_timer_start,
	.stop = max_capi_timer_stop,
	.counter_config = max_capi_timer_counter_config,
	.counter_get = max_capi_timer_counter_get,
	.event_irq_enable = max_capi_timer_event_irq_enable,
	.event_irq_disable = max_capi_timer_event_irq_disable,
	.register_event_callback = max_capi_timer_register_event_callback,
	.channel_init = max_capi_timer_channel_init,
	.channel_deinit = max_capi_timer_channel_deinit,
	.channel_config = max_capi_timer_channel_config,
	.channel_enable = max_capi_timer_channel_enable,
	.channel_disable = max_capi_timer_channel_disable,
	.channel_compare_set = max_capi_timer_channel_compare_set,
	.channel_compare_get = max_capi_timer_channel_compare_get,
	.channel_capture_get = max_capi_timer_channel_capture_get,
	.channel_irq_enable = max_capi_timer_channel_irq_enable,
	.channel_irq_disable = max_capi_timer_channel_irq_disable,
	.channel_register_callback = max_capi_timer_channel_register_callback,
	.is_irq_pending = max_capi_timer_is_irq_pending,
	.isr = max_capi_timer_isr,
	.nsec_to_ticks = max_capi_timer_nsec_to_ticks,
	.ticks_to_nsec = max_capi_timer_ticks_to_nsec,
};
