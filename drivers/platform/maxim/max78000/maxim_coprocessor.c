/***************************************************************************//**
 *   @file   maxim_coprocessor.c
 *   @brief  Maxim MAX78000 CAPI coprocessor driver.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Manages lifecycle (boot, halt, deinit) of the RISC-V RV32 coprocessor
 * (CPU1). The primary ARM Cortex-M4F core (CPU0) uses this driver to control
 * the auxiliary RISC-V core.
 *
 ******************************************************************************
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

#include "maxim_coprocessor.h"
#include "no_os_alloc.h"
#include "mxc_sys.h"
#include "gcr_regs.h"
#include "fcr_regs.h"
#include "gpio.h"
#include "mxc_pins.h"
#include <errno.h>

extern uint32_t __FlashStart_ __attribute__((weak));

struct max_coprocessor_state {
	struct max_coprocessor_init_param init;
};

static int max_coprocessor_init(struct capi_coprocessor_handle **handle,
				const struct capi_coprocessor_config *config)
{
	struct capi_coprocessor_handle *coproc;
	struct max_coprocessor_state *state;
	bool allocated = false;

	if (!handle || !config || !config->ops)
		return -EINVAL;

	if (!*handle) {
		coproc = no_os_calloc(1, sizeof(*coproc));
		if (!coproc)
			return -ENOMEM;
		allocated = true;
	} else {
		coproc = *handle;
	}

	state = no_os_calloc(1, sizeof(*state));
	if (!state) {
		if (allocated)
			no_os_free(coproc);
		return -ENOMEM;
	}

	if (config->extra)
		state->init = *(struct max_coprocessor_init_param *)config->extra;

	coproc->ops = config->ops;
	coproc->init_allocated = allocated;
	coproc->priv = state;
	*handle = coproc;

	return 0;
}

static int max_coprocessor_boot(struct capi_coprocessor_handle *handle)
{
	struct max_coprocessor_state *state;
	uint32_t boot_addr;

	if (!handle || !handle->priv)
		return -EINVAL;

	state = handle->priv;
	boot_addr = state->init.boot_addr ? state->init.boot_addr :
		    (uint32_t)&__FlashStart_;

	if (state->init.enable_debug)
		MXC_GPIO_Config(&gpio_cfg_rv_jtag);

	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_SMPHR);
	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CPU1);

	MXC_GCR->pclkdis1 |= MXC_F_GCR_PCLKDIS1_CPU1;
	MXC_FCR->urvctrl = 0;
	MXC_FCR->urvbootaddr = boot_addr;
	MXC_GCR->pclkdis1 &= ~MXC_F_GCR_PCLKDIS1_CPU1;
	MXC_GCR->rst1 |= MXC_F_GCR_RST1_CPU1;

	return 0;
}

static int max_coprocessor_halt(struct capi_coprocessor_handle *handle)
{
	if (!handle)
		return -EINVAL;

	MXC_GCR->rst1 &= ~MXC_F_GCR_RST1_CPU1;
	MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CPU1);

	return 0;
}

static int max_coprocessor_deinit(struct capi_coprocessor_handle *handle)
{
	if (!handle)
		return -EINVAL;

	max_coprocessor_halt(handle);
	no_os_free(handle->priv);
	if (handle->init_allocated)
		no_os_free(handle);

	return 0;
}

const struct capi_coprocessor_ops max_coprocessor_ops = {
	.init = &max_coprocessor_init,
	.deinit = &max_coprocessor_deinit,
	.boot = &max_coprocessor_boot,
	.halt = &max_coprocessor_halt
};
