/***************************************************************************//**
 *   @file   adg2404.h
 *   @brief  Header file of ADG2404 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#ifndef ADG2404_H_
#define ADG2404_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"

enum adg2404_channel {
	ADG2404_CH_OFF = 0,
	ADG2404_S1,
	ADG2404_S2,
	ADG2404_S3,
	ADG2404_S4,
};


struct adg2404_dev {
	struct no_os_gpio_desc	*gpio_a0;
	struct no_os_gpio_desc	*gpio_a1;
	struct no_os_gpio_desc	*gpio_en;
};

struct adg2404_init_param {
	struct no_os_gpio_init_param	gpio_a0;
	struct no_os_gpio_init_param	gpio_a1;
	struct no_os_gpio_init_param	gpio_en;
};

/* Select the multiplexer channel. */
int adg2404_select_channel(struct adg2404_dev *dev,
			   enum adg2404_channel channel);
/* Enable/disable the multiplexer. */
int adg2404_enable(struct adg2404_dev *dev, bool enable);
/* Initialize the device. */
int adg2404_init(struct adg2404_dev **device,
		 struct adg2404_init_param *init_param);
/* Free the resources allocated by adg2404_init(). */
int adg2404_remove(struct adg2404_dev *dev);

#endif // ADG2404_H_
