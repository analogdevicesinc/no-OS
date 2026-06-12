/***************************************************************************//**
 *   @file   xilinx_compat.h
 *   @brief  Compatibility defines for Vitis 2025+ BSP changes.
 *   @author Stefan Popa (stefan.popa@analog.com)
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
#ifndef _XILINX_COMPAT_H_
#define _XILINX_COMPAT_H_

/*
 * Vitis 2025+ removed XPAR_*_DEVICE_ID defines from xparameters.h and renamed
 * some interrupt and parameter macros. Provide fallback defines so existing
 * code builds with both old and new BSPs.
 *
 * This header is automatically appended to the BSP-generated xparameters.h
 * during create_project (util.py), so all projects get these defines without
 * needing per-project changes.
 */

/* CPU clock frequency renames: Vitis 2025+ generates the universal
 * XPAR_CPU_CORE_CLOCK_FREQ_HZ instead of the per-core
 * XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ.  The BSP's xparameters_ps.h
 * unconditionally references the old name at the expansion of
 * XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ, so we must provide it before
 * bspconfig.h pulls in xparameters_ps.h. */
#if !defined(XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ) && defined(XPAR_CPU_CORE_CLOCK_FREQ_HZ)
#define XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ	XPAR_CPU_CORE_CLOCK_FREQ_HZ
#endif

/* Pull in bspconfig.h to get PLATFORM_ZYNQ / PLATFORM_ZYNQMP / PLATFORM_MB
 * macros.  In Vitis 2025+ these are no longer reachable through xparameters.h
 * alone; bspconfig.h also brings xparameters_ps.h on PS designs. */
#include "bspconfig.h"

/* Zynq PS7 */
#ifndef XPAR_PS7_SPI_0_DEVICE_ID
#define XPAR_PS7_SPI_0_DEVICE_ID	0
#endif
#ifndef XPAR_PS7_SPI_1_DEVICE_ID
#define XPAR_PS7_SPI_1_DEVICE_ID	1
#endif
#ifndef XPAR_PS7_GPIO_0_DEVICE_ID
#define XPAR_PS7_GPIO_0_DEVICE_ID	0
#endif
#ifndef XPAR_XUARTPS_0_DEVICE_ID
#define XPAR_XUARTPS_0_DEVICE_ID	0
#endif

/* GIC — xparameters_ps.h defines this as 0U for PS designs; use 0U here
 * to match so that GCC does not warn on identical redefinition. */
#ifndef XPAR_SCUGIC_SINGLE_DEVICE_ID
#define XPAR_SCUGIC_SINGLE_DEVICE_ID	0U
#endif

/* ZynqMP PSU */
#ifndef XPAR_PSU_GPIO_0_DEVICE_ID
#define XPAR_PSU_GPIO_0_DEVICE_ID	0
#endif
#ifndef XPAR_PSU_SPI_0_DEVICE_ID
#define XPAR_PSU_SPI_0_DEVICE_ID	0
#endif
#ifndef XPAR_PSU_SPI_1_DEVICE_ID
#define XPAR_PSU_SPI_1_DEVICE_ID	1
#endif
#ifndef XPAR_PSU_UART_0_DEVICE_ID
#define XPAR_PSU_UART_0_DEVICE_ID	0
#endif

/* Versal PSV */
#ifndef XPAR_XUARTPSV_0_DEVICE_ID
#define XPAR_XUARTPSV_0_DEVICE_ID	0
#endif

/* AXI / PL (Microblaze) — both canonical (XPAR_<DRIVER>_<N>_DEVICE_ID)
 * and instance-name (XPAR_<BLOCK_DESIGN_NAME>_DEVICE_ID) forms. */
#ifndef XPAR_GPIO_0_DEVICE_ID
#define XPAR_GPIO_0_DEVICE_ID		0
#endif
#ifndef XPAR_AXI_GPIO_DEVICE_ID
#define XPAR_AXI_GPIO_DEVICE_ID		0
#endif
#ifndef XPAR_SPI_0_DEVICE_ID
#define XPAR_SPI_0_DEVICE_ID		0
#endif
#ifndef XPAR_AXI_SPI_DEVICE_ID
#define XPAR_AXI_SPI_DEVICE_ID		0
#endif
#ifndef XPAR_AXI_UART_DEVICE_ID
#define XPAR_AXI_UART_DEVICE_ID		0
#endif
#ifndef XPAR_INTC_SINGLE_DEVICE_ID
#define XPAR_INTC_SINGLE_DEVICE_ID	0
#endif

/* Note: XPAR_INTC_MAX_NUM_INTR_INPUTS is intentionally NOT provided here.
 * On Vitis 2025+ the BSP's xintc.h pulls in xintc_drv_config.h, which always
 * defines this macro.  Providing a fallback from xparameters.h (which only
 * exposes XPAR_XINTC_0_NUM_INTR_INPUTS) would be redefined later by
 * xintc_drv_config.h, triggering a redefinition warning.  No no-OS code uses
 * this macro outside the xintc.h include chain, so the BSP definition suffices. */

/* Interrupt ID renames: old XPAR_AXI_INTC_<periph>_INTERRUPT_INTR
 * became XPAR_FABRIC_<periph>_INTR in Vitis 2025+. */
#if !defined(XPAR_AXI_INTC_AXI_TIMER_INTERRUPT_INTR) && defined(XPAR_FABRIC_AXI_TIMER_INTR)
#define XPAR_AXI_INTC_AXI_TIMER_INTERRUPT_INTR	XPAR_FABRIC_AXI_TIMER_INTR
#endif
#if !defined(XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR) && defined(XPAR_FABRIC_AXI_UART_INTR)
#define XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR	XPAR_FABRIC_AXI_UART_INTR
#endif

/* SPI device ID and clock frequency renames: Vitis 2025+ uses XPAR_XSPIPS_*
 * instead of the platform-specific XPAR_PS7_SPI_* / XPAR_PSU_SPI_* names,
 * and removed _DEVICE_ID macros entirely.
 * Define both device 0 and device 1 — xilinx_spi.c token-pastes the device
 * index, so all instances the BSP provides must be covered. */
#ifndef XPAR_XSPIPS_0_DEVICE_ID
#define XPAR_XSPIPS_0_DEVICE_ID			0
#endif
#ifndef XPAR_XSPIPS_1_DEVICE_ID
#define XPAR_XSPIPS_1_DEVICE_ID			1
#endif

/* GPIO PS device ID: Vitis 2025+ removed this macro. */
#ifndef XPAR_XGPIOPS_0_DEVICE_ID
#define XPAR_XGPIOPS_0_DEVICE_ID		0
#endif
#if !defined(XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ) && defined(XPAR_XSPIPS_0_SPI_CLK_FREQ_HZ)
#define XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ		XPAR_XSPIPS_0_SPI_CLK_FREQ_HZ
#endif
#if !defined(XPAR_PS7_SPI_1_SPI_CLK_FREQ_HZ) && defined(XPAR_XSPIPS_1_SPI_CLK_FREQ_HZ)
#define XPAR_PS7_SPI_1_SPI_CLK_FREQ_HZ		XPAR_XSPIPS_1_SPI_CLK_FREQ_HZ
#endif
#if !defined(XPAR_PSU_SPI_0_SPI_CLK_FREQ_HZ) && defined(XPAR_XSPIPS_0_SPI_CLK_FREQ_HZ)
#define XPAR_PSU_SPI_0_SPI_CLK_FREQ_HZ		XPAR_XSPIPS_0_SPI_CLK_FREQ_HZ
#endif
#if !defined(XPAR_PSU_SPI_1_SPI_CLK_FREQ_HZ) && defined(XPAR_XSPIPS_1_SPI_CLK_FREQ_HZ)
#define XPAR_PSU_SPI_1_SPI_CLK_FREQ_HZ		XPAR_XSPIPS_1_SPI_CLK_FREQ_HZ
#endif

/* UART PS interrupt ID renames: Vitis 2025+ changed _INTR to _INTERRUPTS. */
#if !defined(XPAR_XUARTPS_0_INTR) && defined(XPAR_XUARTPS_0_INTERRUPTS)
#define XPAR_XUARTPS_0_INTR			XPAR_XUARTPS_0_INTERRUPTS
#endif
#if !defined(XPAR_XUARTPS_1_INTR) && defined(XPAR_XUARTPS_1_INTERRUPTS)
#define XPAR_XUARTPS_1_INTR			XPAR_XUARTPS_1_INTERRUPTS
#endif

#endif /* _XILINX_COMPAT_H_ */
