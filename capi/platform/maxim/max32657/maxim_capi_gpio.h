/***************************************************************************//**
 *   @file   maxim_capi_gpio.h
 *   @brief  Header file for GPIO functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#ifndef MAXIM_CAPI_GPIO_H_
#define MAXIM_CAPI_GPIO_H_

#include "gpio.h"
#include "capi_gpio.h"

enum max_capi_gpio_vssel {
	MAX_CAPI_GPIO_VSSEL_VDDIO = MXC_GPIO_VSSEL_VDDIO,
	MAX_CAPI_GPIO_VSSEL_VDDIOH = MXC_GPIO_VSSEL_VDDIOH,
};

enum max_capi_gpio_func {
	MAX_CAPI_GPIO_FUNC_IN = MXC_GPIO_FUNC_IN,
	MAX_CAPI_GPIO_FUNC_OUT = MXC_GPIO_FUNC_OUT,
	MAX_CAPI_GPIO_FUNC_ALT_1 = MXC_GPIO_FUNC_ALT1,
	MAX_CAPI_GPIO_FUNC_ALT_2 = MXC_GPIO_FUNC_ALT2,
	MAX_CAPI_GPIO_FUNC_ALT_3 = MXC_GPIO_FUNC_ALT3,
	MAX_CAPI_GPIO_FUNC_ALT_4 = MXC_GPIO_FUNC_ALT4,
};

enum max_capi_gpio_pad {
	MAX_CAPI_GPIO_PAD_NONE = MXC_GPIO_PAD_NONE,
	MAX_CAPI_GPIO_PAD_PULL_UP = MXC_GPIO_PAD_WEAK_PULL_UP,
	MAX_CAPI_GPIO_PAD_PULL_DOWN = MXC_GPIO_PAD_WEAK_PULL_DOWN,
};

enum max_capi_gpio_drvstr {
	MAX_CAPI_GPIO_DRVSTR_0 = MXC_GPIO_DRVSTR_0,
	MAX_CAPI_GPIO_DRVSTR_1 = MXC_GPIO_DRVSTR_1,
	MAX_CAPI_GPIO_DRVSTR_2 = MXC_GPIO_DRVSTR_2,
	MAX_CAPI_GPIO_DRVSTR_3 = MXC_GPIO_DRVSTR_3,
};

struct max_capi_gpio_extra_config {
	enum max_capi_gpio_vssel vssel;
	enum max_capi_gpio_func func;
	enum max_capi_gpio_pad pad;
	enum max_capi_gpio_drvstr drvstr;
};

struct max_capi_gpio_port_priv {
	uint32_t id;
	mxc_gpio_regs_t *port;
	uint32_t direction_mask;
	struct max_capi_gpio_extra_config *extra;
};

extern const struct capi_gpio_ops max_capi_gpio_ops;

#endif /* MAXIM_CAPI_GPIO_H_ */
