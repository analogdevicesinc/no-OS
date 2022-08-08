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
