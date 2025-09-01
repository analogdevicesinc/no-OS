/***************************************************************************//**
 *   @file   adl8113.h
 *   @brief  Header file for ADL8113 Driver.
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
#ifndef ADL8113_H_
#define ADL8113_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"

/**
 * @enum adl8113_mode
 * @brief ADL8113 Operation Mode
 */
enum adl8113_mode {
	ADL8113_INTERNAL_AMPLIFIER,
	ADL8113_INTERNAL_BYPASS,
	ADL8113_EXTERNAL_BYPASS_A,
	ADL8113_EXTERNAL_BYPASS_B
};

/**
 * @struct adl8113_dev
 * @brief ADL8113 Device Descriptor.
 */
struct adl8113_dev {
	struct no_os_gpio_desc	*gpio_va;
	struct no_os_gpio_desc	*gpio_vb;
	enum adl8113_mode	current_mode;
};

/**
 * @struct adl8113_init_param
 * @brief ADL8113 Initialization Parameters structure.
 */
struct adl8113_init_param {
	struct no_os_gpio_init_param	gpio_va;
	struct no_os_gpio_init_param	gpio_vb;
	enum adl8113_mode		initial_mode;
};

/* Set operation mode (LNA or Bypass). */
int adl8113_set_mode(struct adl8113_dev *dev, enum adl8113_mode mode);

/* Get current operation mode. */
int adl8113_get_mode(struct adl8113_dev *dev, enum adl8113_mode *mode);

/* Initialize the device. */
int adl8113_init(struct adl8113_dev **device,
		 struct adl8113_init_param *init_param);

/* Free the resources allocated by adl8113_init(). */
int adl8113_remove(struct adl8113_dev *dev);

#endif /* ADL8113_H_ */