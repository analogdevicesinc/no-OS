/***************************************************************************//**
 *   @file   adg1712.h
 *   @brief  Header file of ADG1712 Driver.
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
#ifndef ADG1712_H_
#define ADG1712_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"

enum adg1712_switch {
	ADG1712_SW1,
	ADG1712_SW2,
	ADG1712_SW3,
	ADG1712_SW4,
};

struct adg1712_dev {
	struct no_os_gpio_desc *gpio_in1;
	struct no_os_gpio_desc *gpio_in2;
	struct no_os_gpio_desc *gpio_in3;
	struct no_os_gpio_desc *gpio_in4;
};

struct adg1712_init_param {
	struct no_os_gpio_init_param gpio_in1;
	struct no_os_gpio_init_param gpio_in2;
	struct no_os_gpio_init_param gpio_in3;
	struct no_os_gpio_init_param gpio_in4;
};

/** Set the state of a specific switch. */
int adg1712_set_switch_state(struct adg1712_dev *dev,
			     enum adg1712_switch sw,
			     bool enable);

/** Get the current state of a specific switch. */
int adg1712_get_switch_state(struct adg1712_dev *dev,
			     enum adg1712_switch sw,
			     bool *enable);

/** Initialize the ADG1712 device. */
int adg1712_init(struct adg1712_dev **device,
		 struct adg1712_init_param *init_param);

/** Free resources allocated by adg1712_init(). */
int adg1712_remove(struct adg1712_dev *dev);

#endif // ADG1712_H_
