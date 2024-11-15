/***************************************************************************//**
 *   @file   max14914.h
 *   @brief  Header file of MAX14914 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MAX14914_H
#define _MAX14914_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_util.h"

enum max14914_state {
	MAX14914_DIGITAL_OUTPUT_PP,
	MAX14914_DIGITAL_OUTPUT_HIGH_SIDE,
	MAX14914_DIGITAL_OUTPUT_LOW_SIDE,
	MAX14914_DIGITAL_INPUT_IEC_TYPE1_3,
	MAX14914_DIGITAL_INPUT_IEC_TYPE2
};

/**
 * @brief Structure holding the MAX14914 device descriptor
*/
struct max14914_desc {
	enum max14914_state current_state;
	struct no_os_gpio_desc *fault_gpio;
	struct no_os_gpio_desc *doilvl_gpio;
	struct no_os_gpio_desc *di_en_gpio;
	struct no_os_gpio_desc *in_gpio;
	struct no_os_gpio_desc *pp_gpio;
};

/**
 * @brief Structure holding the MAX14914 initialization parameter.
*/
struct max14914_init_param {
	struct no_os_gpio_init_param *fault_init_param;
	struct no_os_gpio_init_param *doilvl_init_param;
	struct no_os_gpio_init_param *di_en_init_param;
	struct no_os_gpio_init_param *in_init_param;
	struct no_os_gpio_init_param *pp_init_param;
};

/* Set the state of the MAX14914. */
int max14914_set_state(struct max14914_desc *desc, enum max14914_state state);

/* Initialize the MAX14914 device descriptor. */
int max14914_init(struct max14914_desc **desc,
		  struct max14914_init_param *init_param);

/* Free resources allocate by the init function. */
int max14914_remove(struct max14914_desc *desc);

#endif /* _MAX14914_H */
