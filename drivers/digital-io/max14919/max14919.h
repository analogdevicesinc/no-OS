/***************************************************************************//**
 *   @file   max14919.h
 *   @brief  Header file of MAX14919 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
