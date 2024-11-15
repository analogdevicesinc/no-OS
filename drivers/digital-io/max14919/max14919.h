/***************************************************************************//**
 *   @file   max14919.h
 *   @brief  Header file of MAX14919 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MAX14919_H
#define _MAX14919_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_error.h"

#define MAX14919_OUT_CHANNELS		4

enum max14919_out_state {
	MAX14919_OUT_OFF,
	MAX14919_OUT_ON
};

enum max14919_out_chan {
	MAX14919_OUT1,
	MAX14919_OUT2,
	MAX14919_OUT3,
	MAX14919_OUT4
};

/**
 * @brief Structure holding the MAX14919 initialization parameter
*/
struct max14919_init_param {
	struct no_os_gpio_init_param *in_param[MAX14919_OUT_CHANNELS];
	struct no_os_gpio_init_param *inrush_param;
	struct no_os_gpio_init_param *fault_param;
	struct no_os_gpio_init_param *rev_param;
};

/**
 * @brief Structure holding te MAX14919 device descriptor
*/
struct max14919_desc {
	struct no_os_gpio_desc *in_desc[MAX14919_OUT_CHANNELS];
	struct no_os_gpio_desc *inrush_desc;
	struct no_os_gpio_desc *fault_desc;
	struct no_os_gpio_desc *rev_desc;
	enum max14919_out_state out_state[MAX14919_OUT_CHANNELS];
};

/** Set output channels state. */
int max14919_set_out(struct max14919_desc *desc,
		     enum max14919_out_state *state);

/** Set 2x current limiting state. */
int max14919_set_climit(struct max14919_desc *desc, bool state);

/** Get FAULT state from the FAULT pin. */
int max14919_get_fault(struct max14919_desc *desc, uint8_t* fault);

/** Detect REV polarity of the current. */
int max14919_detect_rev(struct max14919_desc *desc, uint8_t* rev);

/** Initialize the MAX14919 device descriptor. */
int max14919_init(struct max14919_desc **desc,
		  struct max14919_init_param *init_param);

/** Free resources allocated by the init function. */
int max14919_remove(struct max14919_desc *desc);

#endif /* _MAX14919_H */
