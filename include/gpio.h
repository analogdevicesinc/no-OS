/***************************************************************************//**
 *   @file   gpio.h
 *   @brief  Header file of GPIO Interface
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef GPIO_H_
#define GPIO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define GPIO_OUT	0x01
#define GPIO_IN		0x00

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct gpio_platform_ops
 * @brief Structure holding gpio function pointers that point to the platform
 * specific function
 */
struct gpio_platform_ops ;

/**
 * @struct gpio_init_param
 * @brief Structure holding the parameters for GPIO initialization.
 */
typedef struct gpio_init_param {
	/** GPIO number */
	int32_t		number;
	/** GPIO platform specific functions */
	const struct gpio_platform_ops *platform_ops;
	/** GPIO extra parameters (device specific) */
	void		*extra;
} gpio_init_param;

/**
 * @struct gpio_desc
 * @brief Structure holding the GPIO descriptor.
 */
typedef struct gpio_desc {
	/** GPIO number */
	int32_t		number;
	/** GPIO platform specific functions */
	const struct gpio_platform_ops *platform_ops;
	/** GPIO extra parameters (device specific) */
	void		*extra;
} gpio_desc;

/**
 * @enum gpio_values
 * @brief Enum that holds the possible output states of a GPIO.
 */
enum gpio_values {
	/** GPIO logic low */
	GPIO_LOW,
	/** GPIO logic high */
	GPIO_HIGH,
	/** GPIO high impedance */
	GPIO_HIGH_Z
};

/**
 * @struct gpio_platform_ops
 * @brief Structure holding gpio function pointers that point to the platform
 * specific function
 */
struct gpio_platform_ops {
	/** gpio initialization function pointer */
	int32_t (*gpio_ops_get)(struct gpio_desc **, const struct gpio_init_param *);
	/** gpio optional descriptor function pointer */
	int32_t (*gpio_ops_get_optional)(struct gpio_desc **,
					 const struct gpio_init_param *);
	/** gpio remove function pointer */
	int32_t (*gpio_ops_remove)(struct gpio_desc *);
	/** gpio direction input function pointer */
	int32_t (*gpio_ops_direction_input)(struct gpio_desc *);
	/** gpio direction output function pointer */
	int32_t (*gpio_ops_direction_output)(struct gpio_desc *, uint8_t);
	/** gpio get direction function pointer */
	int32_t (*gpio_ops_get_direction)(struct gpio_desc *, uint8_t *);
	/** gpio set value function pointer */
	int32_t (*gpio_ops_set_value)(struct gpio_desc *, uint8_t);
	/** gpio get value function pointer */
	int32_t (*gpio_ops_get_value)(struct gpio_desc *, uint8_t *);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Obtain the GPIO decriptor. */
int32_t gpio_get(struct gpio_desc **desc,
		 const struct gpio_init_param *param);

/* Obtain optional GPIO descriptor. */
int32_t gpio_get_optional(struct gpio_desc **desc,
			  const struct gpio_init_param *param);

/* Free the resources allocated by gpio_get() */
int32_t gpio_remove(struct gpio_desc *desc);

/* Enable the input direction of the specified GPIO. */
int32_t gpio_direction_input(struct gpio_desc *desc);

/* Enable the output direction of the specified GPIO. */
int32_t gpio_direction_output(struct gpio_desc *desc,
			      uint8_t value);

/* Get the direction of the specified GPIO. */
int32_t gpio_get_direction(struct gpio_desc *desc,
			   uint8_t *direction);

/* Set the value of the specified GPIO. */
int32_t gpio_set_value(struct gpio_desc *desc,
		       uint8_t value);

/* Get the value of the specified GPIO. */
int32_t gpio_get_value(struct gpio_desc *desc,
		       uint8_t *value);

#endif // GPIO_H_
