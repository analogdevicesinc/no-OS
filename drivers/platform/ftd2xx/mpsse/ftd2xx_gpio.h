/*******************************************************************************
 *   @file   ftd2xx_gpio.h
 *   @brief  Implementation of GPIO driver for ftd2xx platform.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#ifndef FTD2XX_GPIO_H_
#define FTD2XX_GPIO_H_

#include <stdbool.h>

#include "no_os_gpio.h"
#include "ftd2xx.h"

#define FTD2XX_GPIO_PIN(x)		NO_OS_BIT(x)
#define FTD2XX_GPIO_READ_CMD(id)	(0x81 + (id))

#define FTD2XX_MAX_PIN_NB		7
#define FTD2XX_MAX_PORT_NB		4

extern uint8_t ftd2xx_gpio_pins_dir[FTD2XX_MAX_PORT_NB];
extern uint8_t ftd2xx_gpio_pins_val[FTD2XX_MAX_PORT_NB];

/**
 * @brief ftd2xx platform specific gpio platform ops structure
 */
extern const struct no_os_gpio_platform_ops ftd2xx_gpio_ops;

/**
 * @struct ftd2xx_gpio_init
 * @brief ftd2xx platform specific initialization parameter
 */
struct ftd2xx_gpio_init {
	uint8_t extra_pins_dir;
};

/**
 * @struct ftd2xx_gpio_desc
 * @brief ftd2xx platform specific gpio descriptor
 */
struct ftd2xx_gpio_desc {
	/** Specific device handle */
	FT_HANDLE *ftHandle;
};

#endif /* FTD2XX_GPIO_H*/
