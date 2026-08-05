/***************************************************************************//**
 *   @file   xilinx_compat.h
 *   @brief  Compatibility defines for Vitis 2025+ BSP changes.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

/* MicroBlaze interrupt controller: Vitis 2025+ moved XPAR_INTC_MAX_NUM_INTR_INPUTS
 * to xintc_drv_config.h, but that header is only included when SDT (System Device
 * Tree) mode is enabled (#ifdef SDT in xintc.h).  In the legacy (non-SDT) BSP flow
 * that no-OS uses, xintc.h includes xparameters.h instead — which no longer defines
 * this macro in Vitis 2025+.  Provide the fallback here. */
#if !defined(XPAR_INTC_MAX_NUM_INTR_INPUTS) && defined(XPAR_XINTC_0_NUM_INTR_INPUTS)
#define XPAR_INTC_MAX_NUM_INTR_INPUTS	XPAR_XINTC_0_NUM_INTR_INPUTS
#endif

/* Interrupt ID renames: old XPAR_AXI_INTC_<periph>_INTERRUPT_INTR
 * became XPAR_FABRIC_<periph>_INTR in Vitis 2025+.
 * NOTE: As of the util.py fix that generates XPAR_AXI_INTC_* macros directly
 * from the XSA for MicroBlaze designs, these fallbacks are no longer needed.
 * They're removed to avoid redefinition warnings. */

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

/* DDR memory base address renames: Vitis 2025+ changed the macro names.
 * Old: XPAR_PSU_DDR_0_S_AXI_BASEADDR, XPAR_PS7_DDR_0_S_AXI_BASEADDR
 * New: XPAR_PSU_DDR_0_BASEADDRESS, XPAR_PS7_DDR_0_BASEADDRESS
 * Also provide XPAR_DDR_MEM_BASEADDR which some projects use as a generic name. */
#if !defined(XPAR_PSU_DDR_0_S_AXI_BASEADDR) && defined(XPAR_PSU_DDR_0_BASEADDRESS)
#define XPAR_PSU_DDR_0_S_AXI_BASEADDR		XPAR_PSU_DDR_0_BASEADDRESS
#endif
#if !defined(XPAR_PS7_DDR_0_S_AXI_BASEADDR) && defined(XPAR_PS7_DDR_0_BASEADDRESS)
#define XPAR_PS7_DDR_0_S_AXI_BASEADDR		XPAR_PS7_DDR_0_BASEADDRESS
#endif
#if !defined(XPAR_DDR_MEM_BASEADDR)
#if defined(XPAR_PSU_DDR_0_BASEADDRESS)
#define XPAR_DDR_MEM_BASEADDR			XPAR_PSU_DDR_0_BASEADDRESS
#elif defined(XPAR_PS7_DDR_0_BASEADDRESS)
#define XPAR_DDR_MEM_BASEADDR			XPAR_PS7_DDR_0_BASEADDRESS
#endif
#endif

/* MicroBlaze DDR4 memory controller: Vitis 2025+ uses different naming.
 * Old: XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR (from hsi::generate_bsp)
 * New: XPAR_DDR4_0_BASEADDRESS (from vitis.create_platform_component)
 * Note the suffix: BASEADDR vs BASEADDRESS */
#if !defined(XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR)
#if defined(XPAR_DDR4_0_BASEADDRESS)
#define XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR	XPAR_DDR4_0_BASEADDRESS
#elif defined(XPAR_MIG_0_BASEADDR)
#define XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR	XPAR_MIG_0_BASEADDR
#elif defined(XPAR_DDR4_0_BASEADDR)
#define XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR	XPAR_DDR4_0_BASEADDR
#endif
#endif

#endif /* _XILINX_COMPAT_H_ */
