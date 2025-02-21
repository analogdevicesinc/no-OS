/***************************************************************************//**
 *   @file   no_os_gpio.h
 *   @brief  Header file of GPIO Interface
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef _NO_OS_GPIO_H_
#define _NO_OS_GPIO_H_

#include <stdint.h>

#define NO_OS_GPIO_OUT	0x01
#define NO_OS_GPIO_IN		0x00

/**
 * @struct no_os_gpio_platform_ops
 * @brief Structure holding gpio function pointers that point to the platform
 * specific function
 */
struct no_os_gpio_platform_ops ;

/**
 * @enum no_os_gpio_pull_up
 * @brief Enum that holds the possible pull up/ pull down resistor configuration.
 */
enum no_os_gpio_pull_up {
	NO_OS_PULL_NONE,
	/** Strong pull up */
	NO_OS_PULL_UP,
	/** Strong pull down */
	NO_OS_PULL_DOWN,
	NO_OS_PULL_UP_WEAK,
	NO_OS_PULL_DOWN_WEAK
};

/**
 * @struct no_os_gpio_init_param
 * @brief Structure holding the parameters for GPIO initialization.
 */
struct no_os_gpio_init_param {
	/** Port number */
	int32_t		port;
	/** GPIO number */
	int32_t		number;
	/** Pull up/down resistor configuration */
	enum no_os_gpio_pull_up pull;
	/** GPIO platform specific functions */
	const struct no_os_gpio_platform_ops *platform_ops;
	/** GPIO extra parameters (device specific) */
	void		*extra;
};

/**
 * @struct no_os_gpio_desc
 * @brief Structure holding the GPIO descriptor.
 */
struct no_os_gpio_desc {
	/** Port number */
	int32_t		port;
	/** GPIO number */
	int32_t		number;
	/** Pull up/down resistor configuration */
	enum no_os_gpio_pull_up pull;
	/** GPIO platform specific functions */
	const struct no_os_gpio_platform_ops *platform_ops;
	/** GPIO extra parameters (device specific) */
	void		*extra;
};

/**
 * @enum no_os_gpio_values
 * @brief Enum that holds the possible output states of a GPIO.
 */
enum no_os_gpio_values {
	/** GPIO logic low */
	NO_OS_GPIO_LOW,
	/** GPIO logic high */
	NO_OS_GPIO_HIGH,
	/** GPIO high impedance */
	NO_OS_GPIO_HIGH_Z
};

/**
 * @struct no_os_gpio_platform_ops
 * @brief Structure holding gpio function pointers that point to the platform
 * specific function
 */
struct no_os_gpio_platform_ops {
	/** gpio initialization function pointer */
	int32_t (*gpio_ops_get)(struct no_os_gpio_desc **,
				const struct no_os_gpio_init_param *);
	/** gpio optional descriptor function pointer */
	int32_t (*gpio_ops_get_optional)(struct no_os_gpio_desc **,
					 const struct no_os_gpio_init_param *);
	/** gpio remove function pointer */
	int32_t (*gpio_ops_remove)(struct no_os_gpio_desc *);
	/** gpio direction input function pointer */
	int32_t (*gpio_ops_direction_input)(struct no_os_gpio_desc *);
	/** gpio direction output function pointer */
	int32_t (*gpio_ops_direction_output)(struct no_os_gpio_desc *, uint8_t);
	/** gpio get direction function pointer */
	int32_t (*gpio_ops_get_direction)(struct no_os_gpio_desc *, uint8_t *);
	/** gpio set value function pointer */
	int32_t (*gpio_ops_set_value)(struct no_os_gpio_desc *, uint8_t);
	/** gpio get value function pointer */
	int32_t (*gpio_ops_get_value)(struct no_os_gpio_desc *, uint8_t *);
};

/* Obtain the GPIO decriptor. */
int32_t no_os_gpio_get(struct no_os_gpio_desc **desc,
		       const struct no_os_gpio_init_param *param);

/* Obtain optional GPIO descriptor. */
int32_t no_os_gpio_get_optional(struct no_os_gpio_desc **desc,
				const struct no_os_gpio_init_param *param);

/* Free the resources allocated by no_os_gpio_get(). */
int32_t no_os_gpio_remove(struct no_os_gpio_desc *desc);

/* Enable the input direction of the specified GPIO. */
int32_t no_os_gpio_direction_input(struct no_os_gpio_desc *desc);

/* Enable the output direction of the specified GPIO. */
int32_t no_os_gpio_direction_output(struct no_os_gpio_desc *desc,
				    uint8_t value);

/* Get the direction of the specified GPIO. */
int32_t no_os_gpio_get_direction(struct no_os_gpio_desc *desc,
				 uint8_t *direction);

/* Set the value of the specified GPIO. */
int32_t no_os_gpio_set_value(struct no_os_gpio_desc *desc,
			     uint8_t value);

/* Get the value of the specified GPIO. */
int32_t no_os_gpio_get_value(struct no_os_gpio_desc *desc,
			     uint8_t *value);

#endif // _NO_OS_GPIO_H_
