/***************************************************************************//**
 *   @file   iio_aducm3029.h
 *   @brief  iio description of aducm3029 uc
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

#ifndef IIO_ADCUM3029
#define IIO_ADCUM3029

#include "iio_types.h"
#include "adc.h"

#define ADUCM3029_ADC_NUM_CH	6
#define ADUCM3029_TIMERS_NUMS	3
#define ADUCM3029_GPIOS_NUMS	44

struct iio_aducm3029_desc {
	struct adc_desc		*adc;
	struct timer_desc	*timer[ADUCM3029_TIMERS_NUMS];
	struct pwm_desc		*pwm[ADUCM3029_TIMERS_NUMS];
	struct gpio_desc	*gpio[ADUCM3029_GPIOS_NUMS];
	uint32_t		current_gpio;
};

static struct iio_aducm3029_desc g_aducm3029_desc;

ssize_t get_pwm_attr(void *device, char *buf, size_t len,
		     const struct iio_ch_info *channel, intptr_t priv);
ssize_t set_pwm_attr(void *device, char *buf, size_t len,
		     const struct iio_ch_info *channel, intptr_t priv);

ssize_t get_global_attr(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel, intptr_t priv);
ssize_t set_global_attr(void *device, char *buf, size_t len,
			const struct iio_ch_info *channel, intptr_t priv);

ssize_t get_gpio_attr(void *device, char *buf, size_t len,
		      const struct iio_ch_info *channel, intptr_t priv);
ssize_t set_gpio_attr(void *device, char *buf, size_t len,
		      const struct iio_ch_info *channel, intptr_t priv);

int32_t iio_aducm3029_adc_set_mask(struct iio_aducm3029_desc *desc,
				   uint32_t mask);
int32_t iio_aducm3029_adc_read(struct iio_aducm3029_desc *desc, uint16_t *buff,
			       uint32_t nb_samples);

enum iio_pwm_attributes {
	PWM_ENABLE,
	PWM_PERIOD,
	PWM_DUTY_CYCLE,
	PWM_POLARITY_IS_HIGH
};

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

enum iio_gpio_attributes {
	GPIO_ENABLE,
	GPIO_VALUE,
	GPIO_DIRECTION_OUTPUT,
	GPIO_NUMBER,
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

static struct scan_type adc_scan_type = {
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.sign = 'u',
	.is_big_endian = false
};

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

enum global_attributes {
	PINMUX_PORT_0,
	PINMUX_PORT_1,
	PINMUX_PORT_2,
	ADC_ENABLE
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

static struct iio_device iio_aducm3029_desc = {
	.num_ch = sizeof(aducm3029_channels) / sizeof(aducm3029_channels[0]),
	.channels = aducm3029_channels,
	.attributes = aducm3029_attributes,
	.prepare_transfer = (int32_t (*)())iio_aducm3029_adc_set_mask,
	.read_dev = (int32_t (*)())iio_aducm3029_adc_read,
};

#endif
