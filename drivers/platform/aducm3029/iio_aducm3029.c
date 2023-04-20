/***************************************************************************//**
 *   @file   iio_aducm3029.c
 *   @brief  Implementation of aducm3029 iio device
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

#ifdef IIO_SUPPORT

#include <stdio.h>
#include <inttypes.h>
#include <sys/platform.h>
#include "no_os_pwm.h"
#include "no_os_gpio.h"
#include "iio_aducm3029.h"
#include "aducm3029_adc.h"
#include "aducm3029_pwm.h"
#include "no_os_error.h"
#include "no_os_util.h"

#define GPIO_PIN(nb)	(1u << ((nb) & 0x0F))
#define GPIO_PORT(nb)	(((nb) & 0xF0) >> 4)

volatile uint32_t *pinmux_addrs[] = {
	((volatile uint32_t *)REG_GPIO0_CFG),
	((volatile uint32_t *)REG_GPIO1_CFG),
	((volatile uint32_t *)REG_GPIO2_CFG)
};

static struct adc_init_param default_adc_init_param = {0};
static struct no_os_pwm_init_param default_pwm_init_par = {
	.id = 0,
	.duty_cycle_ns = 5000000,
	.period_ns = 10000000,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = &aducm3029_pwm_ops,
	.extra = NULL
};
static struct no_os_gpio_init_param default_gpio_init_par = {
	.number = 0,
	.extra = NULL
};

/*
 * adc_mux[0][0] -> ADC0 port,
 * adc_mux[0][1] -> ADC0 pin,
 * adc_mux[0][2] -> mux_val
 */
static uint32_t adc_muxs[ADUCM3029_ADC_NUM_CH][3] = {
	{2, 3, 1},
	{2, 4, 1},
	{2, 5, 1},
	{2, 6, 1},
	{2, 7, 1},
	{2, 8, 1}
};

static uint32_t timers_muxs[ADUCM3029_TIMERS_NUMS][3] = {
	{0, 14, 1},
	{1, 11, 2},
	{2, 1, 2}
};

enum pin_type {
	PIN_TYPE_GPIO,
	PIN_TYPE_ADC,
	PIN_TYPE_TIMER
};

/* Enable in pin mux the needed type */
static int32_t set_pin(uint32_t id, enum pin_type type)
{
	volatile uint32_t *port_addr;
	uint32_t pin;
	uint32_t val;

	switch (type) {
	case PIN_TYPE_GPIO:
		port_addr = pinmux_addrs[GPIO_PORT(id)];
		pin = GPIO_PORT(id);
		/* All gpios are enabled on 0 besied 6, 7 and 17 */
		if (id == 6 || id == 7 || id == 17)
			val = 1;
		else
			val = 0;
		break;
	case PIN_TYPE_ADC:
		if (id >= ADUCM3029_ADC_NUM_CH)
			return -1;
		port_addr = pinmux_addrs[adc_muxs[id][0]];
		pin = adc_muxs[id][1];
		val = adc_muxs[id][2];
		break;
	case PIN_TYPE_TIMER:
		if (id >= ADUCM3029_TIMERS_NUMS)
			return -1;
		port_addr = pinmux_addrs[timers_muxs[id][0]];
		pin = timers_muxs[id][1];
		val = timers_muxs[id][2];
		break;
	}
	*port_addr &= ~(0b11 << (pin * 2));
	*port_addr |= val << (pin * 2);

	return 0;
}


/* Get global iio attributes */
int get_global_attr(void *device, char *buf, uint32_t len,
		    const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t val;
	struct iio_aducm3029_desc *desc = device;

	switch (priv) {
	case PINMUX_PORT_0:
	case PINMUX_PORT_1:
	case PINMUX_PORT_2:
		val = *pinmux_addrs[priv];
		return snprintf(buf, len, "%"PRIx32"", val);
	case ADC_ENABLE:
		val = !!desc->adc;
		return snprintf(buf, len, "%"PRIu32"", val);
	}

	return -EINVAL;
}

/* Set global iio attributes */
int set_global_attr(void *device, char *buf, uint32_t len,
		    const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	uint32_t i;
	uint32_t val = no_os_str_to_uint32(buf);
	struct iio_aducm3029_desc *desc = device;

	switch (priv) {
	case PINMUX_PORT_0:
	case PINMUX_PORT_1:
	case PINMUX_PORT_2:
		*pinmux_addrs[priv] = val;
		break;
	case ADC_ENABLE:
		if (val) {
			if (desc->adc) {
				ret = 0;
			} else {
				for (i = 0; i < ADUCM3029_ADC_NUM_CH; i++)
					set_pin(i, PIN_TYPE_ADC);
				ret = aducm3029_adc_init(&desc->adc,
							 &default_adc_init_param);
			}
		} else {
			ret = aducm3029_adc_remove(desc->adc);
			desc->adc = NULL;
		}
	}
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	return len;
}

/* Set get iio attributes */
int get_pwm_attr(void *device, char *buf, uint32_t len,
		 const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	uint32_t val;
	uint32_t idx;
	enum no_os_pwm_polarity pol;
	struct iio_aducm3029_desc *desc = device;

	idx = channel->ch_num - ADUCM3029_ADC_NUM_CH;
	switch (priv) {
	case PWM_ENABLE:
		ret = 0;
		val = !!desc->pwm[idx];
		break;
	case PWM_PERIOD:
		ret = no_os_pwm_get_period(desc->pwm[idx], &val);
		break;
	case PWM_DUTY_CYCLE:
		ret = no_os_pwm_get_duty_cycle(desc->pwm[idx], &val);
		break;
	case PWM_POLARITY_IS_HIGH:
		ret = no_os_pwm_get_polarity(desc->pwm[idx], &pol);
		val = !!pol;
		break;
	}
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	return snprintf(buf, len, "%"PRIu32"", val);
}

/* Set gpio pwm attributes */
int set_pwm_attr(void *device, char *buf, uint32_t len,
		 const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	uint32_t val = no_os_str_to_uint32(buf);
	uint32_t idx;
	struct iio_aducm3029_desc *desc = device;

	idx = channel->ch_num - ADUCM3029_ADC_NUM_CH;
	if (desc->pwm[idx]) {
		ret = no_os_pwm_disable(desc->pwm[idx]);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
	}
	switch (priv) {
	case PWM_ENABLE:
		if (val) {
			if (desc->pwm[idx]) {
				ret = 0;
			} else {
				set_pin(idx, PIN_TYPE_TIMER);
				default_pwm_init_par.id = idx;
				ret = no_os_pwm_init(&desc->pwm[idx],
						     &default_pwm_init_par);
			}
		} else {
			ret = no_os_pwm_remove(desc->pwm[idx]);
			desc->pwm[idx] = NULL;
		}
		break;
	case PWM_PERIOD:
		ret = no_os_pwm_set_period(desc->pwm[idx], val);
		break;
	case PWM_DUTY_CYCLE:
		ret = no_os_pwm_set_duty_cycle(desc->pwm[idx], val);
		break;
	case PWM_POLARITY_IS_HIGH:
		ret = no_os_pwm_set_polarity(desc->pwm[idx], !!val);
		break;
	}
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	if (desc->pwm[idx]) {
		ret = no_os_pwm_enable(desc->pwm[idx]);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
	}

	return len;
}

/* Get gpio iio attributes */
int get_gpio_attr(void *device, char *buf, uint32_t len,
		  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	uint8_t val;
	uint32_t idx;
	struct iio_aducm3029_desc *desc = device;

	idx = desc->current_gpio;
	switch (priv) {
	case GPIO_ENABLE:
		ret = 0;
		val = !!desc->gpio[idx];
		break;
	case GPIO_VALUE:
		ret = no_os_gpio_get_value(desc->gpio[idx], &val);
		break;
	case GPIO_DIRECTION_OUTPUT:
		ret = no_os_gpio_get_direction(desc->gpio[idx], &val);
		break;
	case GPIO_NUMBER:
		ret = 0;
		val = idx;
		break;
	}
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	return snprintf(buf, len, "%"PRIu8"", val);
}

/* Set gpio iio attributes */
int set_gpio_attr(void *device, char *buf, uint32_t len,
		  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	uint32_t val = no_os_str_to_uint32(buf);
	uint32_t idx;
	struct iio_aducm3029_desc *desc = device;

	idx = desc->current_gpio;
	switch (priv) {
	case GPIO_ENABLE:
		if (val) {
			if (desc->pwm[idx]) {
				ret = 0;
			} else {
				set_pin(idx, PIN_TYPE_GPIO);
				default_gpio_init_par.number = idx;
				ret = no_os_gpio_get(&desc->gpio[idx],
						     &default_gpio_init_par);
			}
		} else {
			ret = no_os_gpio_remove(desc->gpio[idx]);
			desc->gpio[idx] = NULL;
		}
		break;
	case GPIO_VALUE:
		ret = no_os_gpio_set_value(desc->gpio[idx], val);
		break;
	case GPIO_DIRECTION_OUTPUT:
		if (val)
			ret = no_os_gpio_direction_output(desc->gpio[idx], 0);
		else
			ret = no_os_gpio_direction_input(desc->gpio[idx]);

		break;
	case GPIO_NUMBER:
		ret = 0;
		desc->current_gpio = val;
		break;
	}
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	return len;
}

/* iio wrapper for aducm3029_adc_update_active_channels */
int32_t iio_aducm3029_adc_set_mask(struct iio_aducm3029_desc *desc,
				   uint32_t mask)
{
	if (!desc)
		return -1;

	return aducm3029_adc_update_active_channels(desc->adc, mask);
}

/* iio wrapper for aducm3029_adc_read */
int32_t iio_aducm3029_adc_read(struct iio_aducm3029_desc *desc, uint16_t *buff,
			       uint32_t nb_samples)
{
	if (!desc)
		return -1;

	return aducm3029_adc_read(desc->adc, buff, nb_samples);
}

#define IIO_PWM_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = get_pwm_attr,\
	.store = set_pwm_attr\
}

static struct iio_attribute pwm_attributes[] = {
	IIO_PWM_ATTR("en", PWM_ENABLE),
	IIO_PWM_ATTR("period", PWM_PERIOD),
	IIO_PWM_ATTR("duty_cycle", PWM_DUTY_CYCLE),
	IIO_PWM_ATTR("polarity_is_high", PWM_POLARITY_IS_HIGH),
	END_ATTRIBUTES_ARRAY,
};

#define IIO_GPIO_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = get_gpio_attr,\
	.store = set_gpio_attr\
}

static struct iio_attribute gpio_attributes[] = {
	IIO_GPIO_ATTR("en", GPIO_ENABLE),
	IIO_GPIO_ATTR("value", GPIO_VALUE),
	IIO_GPIO_ATTR("is_output", GPIO_DIRECTION_OUTPUT),
	IIO_GPIO_ATTR("number", GPIO_NUMBER),
	END_ATTRIBUTES_ARRAY,
};

static struct scan_type adc_scan_type = {
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.sign = 'u',
	.is_big_endian = false
};

#define ADCUM3029_ADC_CH(idx)  {\
	.name = "adc" # idx, \
	.ch_type = IIO_VOLTAGE,\
	.channel = idx,\
	.scan_index = idx,\
	.scan_type = &adc_scan_type,\
	.indexed = true}

#define ADCUM3029_PWM_CH(idx)  {\
	.name = "pwm" # idx, \
	.ch_type = IIO_VOLTAGE,\
	.channel = idx + ADUCM3029_ADC_NUM_CH,\
	.indexed = true,\
	.attributes = pwm_attributes,\
	.ch_out = true}

#define ADUCM3029_GPIO_CONTROLLER(idx)  {\
	.name = "gpio_controller", \
	.ch_type = IIO_VOLTAGE,\
	.channel = idx + ADUCM3029_ADC_NUM_CH + ADUCM3029_TIMERS_NUMS,\
	.indexed = true,\
	.attributes = gpio_attributes,\
	.ch_out = true}

static struct iio_channel aducm3029_channels[] = {
	ADCUM3029_ADC_CH(0),
	ADCUM3029_ADC_CH(1),
	ADCUM3029_ADC_CH(2),
	ADCUM3029_ADC_CH(3),
	ADCUM3029_ADC_CH(4),
	ADCUM3029_ADC_CH(5),
	ADCUM3029_PWM_CH(0),
	ADCUM3029_PWM_CH(1),
	ADCUM3029_PWM_CH(2),
	ADUCM3029_GPIO_CONTROLLER(0),
};

#define GLOBAL_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = get_global_attr,\
	.store = set_global_attr\
}

static struct iio_attribute aducm3029_attributes[] = {
	GLOBAL_ATTR("pinmux_port0_cfg", PINMUX_PORT_0),
	GLOBAL_ATTR("pinmux_port1_cfg", PINMUX_PORT_1),
	GLOBAL_ATTR("pinmux_port2_cfg", PINMUX_PORT_2),
	GLOBAL_ATTR("adc_enable", ADC_ENABLE),
	END_ATTRIBUTES_ARRAY,
};

struct iio_device const iio_aducm3029_desc = {
	.num_ch = NO_OS_ARRAY_SIZE(aducm3029_channels),
	.channels = aducm3029_channels,
	.attributes = aducm3029_attributes,
	.pre_enable = (int32_t (*)())iio_aducm3029_adc_set_mask,
	.read_dev = (int32_t (*)())iio_aducm3029_adc_read,
};

struct iio_aducm3029_desc g_aducm3029_desc;

#endif //IIO_SUPPORT
