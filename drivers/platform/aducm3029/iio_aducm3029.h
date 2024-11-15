/***************************************************************************//**
 *   @file   iio_aducm3029.h
 *   @brief  iio description of aducm3029 uc
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef IIO_SUPPORT

#ifndef IIO_ADCUM3029
#define IIO_ADCUM3029

#include "iio_types.h"
#include "aducm3029_adc.h"

#define ADUCM3029_TIMERS_NUMS	3
#define ADUCM3029_GPIOS_NUMS	44

struct iio_aducm3029_desc {
	struct adc_desc		*adc;
	struct no_os_timer_desc	*timer[ADUCM3029_TIMERS_NUMS];
	struct no_os_pwm_desc		*pwm[ADUCM3029_TIMERS_NUMS];
	struct no_os_gpio_desc	*gpio[ADUCM3029_GPIOS_NUMS];
	uint32_t		current_gpio;
};

extern struct iio_aducm3029_desc g_aducm3029_desc;
extern struct iio_device const iio_aducm3029_desc;

int get_pwm_attr(void *device, char *buf, uint32_t len,
		 const struct iio_ch_info *channel, intptr_t priv);
int set_pwm_attr(void *device, char *buf, uint32_t len,
		 const struct iio_ch_info *channel, intptr_t priv);

int get_global_attr(void *device, char *buf, uint32_t len,
		    const struct iio_ch_info *channel, intptr_t priv);
int set_global_attr(void *device, char *buf, uint32_t len,
		    const struct iio_ch_info *channel, intptr_t priv);

int get_gpio_attr(void *device, char *buf, uint32_t len,
		  const struct iio_ch_info *channel, intptr_t priv);
int set_gpio_attr(void *device, char *buf, uint32_t len,
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

enum iio_gpio_attributes {
	GPIO_ENABLE,
	GPIO_VALUE,
	GPIO_DIRECTION_OUTPUT,
	GPIO_NUMBER,
};

enum global_attributes {
	PINMUX_PORT_0,
	PINMUX_PORT_1,
	PINMUX_PORT_2,
	ADC_ENABLE
};

#endif

#endif //IIO_SUPPORT
