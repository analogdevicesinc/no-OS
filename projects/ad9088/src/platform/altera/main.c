/***************************************************************************//**
 *   @file   altera/main.c
 *   @brief  Main file for the Altera/Nios V platform of the ad9088 project
 *           (AD9084-EBZ on Agilex 5).
 *   @author Mihaela-Georgeta Petrea (Mihaela-georgeta.Petrea@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#ifdef DMA_EXAMPLE
#include "dma_example.h"
#endif

/**
 * @brief Main function execution for the Altera/Nios V platform.
 *
 * stdout is routed to the EBZ JTAG UART (sys_uart @ 0x100) by the project-local
 * _write() override in parameters.c - the generic BSP's _write() targets a
 * nonexistent JTAG UART (0x90158), so pr_*() would otherwise be discarded.
 * Console is up as soon as main() runs.
 *
 * @return ret - Result of the enabled example's execution.
 */
int main(void)
{
	int ret = -EINVAL;

	/*
	 * newlib fully buffers stdout by default -- output would then only appear
	 * once ~1 KB has accumulated or the program exits, so an early hang looks
	 * like total silence. Make stdout unbuffered so every pr_*() reaches the
	 * polled JTAG UART immediately.
	 */
	setvbuf(stdout, NULL, _IONBF, 0);

	pr_info("ad9088: Nios V (Agilex 5) bring-up\n");

#ifdef BASIC_EXAMPLE
	ret = basic_example_main();
#endif

#ifdef DMA_EXAMPLE
	ret = dma_example_main();
#endif

	return ret;
}
