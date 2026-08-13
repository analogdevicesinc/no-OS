/***************************************************************************//**
 *   @file   maxim_coprocessor.c
 *   @brief  Maxim MAX78000 coprocessor (RISC-V CPU1) platform driver.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Manages lifecycle (boot, halt, remove) of the RISC-V RV32 coprocessor (CPU1).
 * The primary ARM Cortex-M4F core (CPU0) uses this driver to control the
 * auxiliary RISC-V core. Both cores share a unified address space and SRAM.
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
#include "no_os_util.h"
#include "mxc_sys.h"
#include "gcr_regs.h"
#include "fcr_regs.h"
#include "gpio.h"
#include "mxc_pins.h"
#include <errno.h>

/* Linker-supplied RISC-V flash start symbol (if boot_addr == 0).
 * Weak declaration: symbol may not exist in all linker scripts. */
extern uint32_t __FlashStart_ __attribute__((weak));

/**
 * @brief Initialize coprocessor descriptor.
 * Allocates a descriptor and stores the Maxim-specific init parameters.
 * Does NOT release the coprocessor from reset; call no_os_coprocessor_boot()
 * to start execution.
 * @param desc Output: pointer to allocated descriptor.
 * @param param Input: generic init parameters containing platform_ops and extra.
 * @return 0 on success, -EINVAL if param is NULL, -ENOMEM if allocation fails.
 */
static int max_coprocessor_init(struct no_os_coprocessor_desc **desc,
				const struct no_os_coprocessor_init_param *param)
{
	struct no_os_coprocessor_desc *descriptor;

	if (!param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->id = param->id;
	descriptor->platform_ops = param->platform_ops;
	descriptor->extra = param->extra;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Boot (release from reset and enable clock) the RISC-V coprocessor.
 * Sequence:
 * 1. Enable SEMA peripheral clock (for IPC).
 * 2. Write RISC-V boot address to FCR->urvbootaddr.
 * 3. Enable CPU1 peripheral clock.
 * 4. Release CPU1 from reset via GCR->rst1.
 *
 * After this call, the RISC-V core begins execution at the boot address.
 *
 * @param desc Coprocessor descriptor from max_coprocessor_init().
 * @return 0 on success, -EINVAL if desc or extra is NULL.
 */
static int max_coprocessor_boot(struct no_os_coprocessor_desc *desc)
{
	struct max_coprocessor_init_param *extra;
	uint32_t boot_addr;

	if (!desc || !desc->extra)
		return -EINVAL;

	extra = (struct max_coprocessor_init_param *)desc->extra;

	/* Determine boot address: use explicit value or linker symbol */
	boot_addr = extra->boot_addr ? extra->boot_addr : (uint32_t)&__FlashStart_;

	/* Optionally enable the RISC-V JTAG debug pins so a debugger can attach
	 * to the coprocessor. */
	if (extra->enable_debug)
		MXC_GPIO_Config(&gpio_cfg_rv_jtag);

	/* Enable SEMA peripheral clock for inter-core communication */
	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_SMPHR);

	/*
	 * Bring the RISC-V (CPU1) out of reset:
	 * 1. Enable the CPU1 peripheral clock.
	 * 2. Gate the CPU1 clock (pclkdis1) while programming the boot vector so
	 *    the core is quiescent when its reset vector changes.
	 * 3. Select flash as the RISC-V instruction source (urvctrl = 0) and program
	 *    FCR.urvbootaddr with the boot address.
	 * 4. Ungate the CPU1 clock.
	 * 5. Pulse rst1.CPU1: a self-clearing reset trigger (reads back 0 once the
	 *    reset completes, not a level-held line). The core then comes out of
	 *    reset and begins fetching from urvbootaddr.
	 */
	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CPU1);

	/* Gate CPU1 clock while programming the boot vector */
	MXC_GCR->pclkdis1 |= MXC_F_GCR_PCLKDIS1_CPU1;

	/* Ensure the RISC-V memory-select is at its default (fetch from flash). */
	MXC_FCR->urvctrl = 0;

	/* Set RISC-V boot address in Flash Control Register */
	MXC_FCR->urvbootaddr = boot_addr;

	/* Ungate CPU1 clock */
	MXC_GCR->pclkdis1 &= ~MXC_F_GCR_PCLKDIS1_CPU1;

	/* Pulse CPU1 reset (self-clearing) to release the RISC-V core */
	MXC_GCR->rst1 |= MXC_F_GCR_RST1_CPU1;

	return 0;
}

/**
 * @brief Halt (assert reset and disable clock) the RISC-V coprocessor.
 * @param desc Coprocessor descriptor from max_coprocessor_init().
 * @return 0 on success, -EINVAL if desc is NULL.
 */
static int max_coprocessor_halt(struct no_os_coprocessor_desc *desc)
{
	if (!desc)
		return -EINVAL;

	/* Assert CPU1 reset */
	MXC_GCR->rst1 &= ~MXC_F_GCR_RST1_CPU1;

	/* Disable CPU1 peripheral clock */
	MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CPU1);

	return 0;
}

/**
 * @brief Remove (free resources) the coprocessor descriptor.
 * @param desc Coprocessor descriptor from max_coprocessor_init().
 * @return 0 on success, -EINVAL if desc is NULL.
 */
static int max_coprocessor_remove(struct no_os_coprocessor_desc *desc)
{
	if (!desc)
		return -EINVAL;

	/* Halt before removing */
	max_coprocessor_halt(desc);

	no_os_free(desc);

	return 0;
}

/** Platform ops table for MAX78000 coprocessor */
const struct no_os_coprocessor_platform_ops max_coprocessor_ops = {
	.init = &max_coprocessor_init,
	.boot = &max_coprocessor_boot,
	.halt = &max_coprocessor_halt,
	.remove = &max_coprocessor_remove
};
