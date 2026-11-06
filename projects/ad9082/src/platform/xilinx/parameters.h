/***************************************************************************//**
 *   @file   ad9082/src/platform/xilinx/parameters.h
 *   @brief  Platform parameters for the AD9082 VPK180 (Versal) project.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <xparameters.h>
#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#include "app_config.h"

/*
 * This project targets the Versal SoC only (VPK180).  PLATFORM_VERSAL is set by
 * CMakeLists.txt for the board; the auto-detect below additionally covers a
 * Vitis-IDE build (no -D on the command line).  The APU (Cortex-A72) is the
 * only CPU wired up today -- a Cortex-R5 case will be added under the same
 * PLATFORM_VERSAL gate later (its differences are the console UART / interrupt
 * decode, not the AXI GPIO / PS-SPI / GT-reset map below).
 */
#if defined(XPAR_CPU_CORTEXA72_CORE_CLOCK_FREQ_HZ) && !defined(PLATFORM_VERSAL)
#define PLATFORM_VERSAL
#endif

#ifndef PLATFORM_VERSAL
#error "projects/ad9082 supports the Versal platform only (VPK180)."
#endif

/*
 * The Versal BSP exposes the PSV (SBSA) UART interrupt as the raw device-tree
 * specifier XPAR_XUARTPSV_0_INTERRUPTS; XGet_IntrId()/XGet_IntrOffset() from
 * <xinterrupt_wrap.h> decode it to a GIC id.
 */
#if defined(XPAR_XUARTPSV_0_INTERRUPTS)
#include <xinterrupt_wrap.h>
#endif

#define UART_BAUDRATE		115200

#define GPIO_OFFSET		0
#define PHY_CS			0

/* VPK180: MxFE reset-gpios = <&axi_gpio 23> (from the Linux dts). */
#define PHY_RESET		(GPIO_OFFSET + 23)

/*
 * VPK180 (Versal Premium): AXI GPIO in the PL, MxFE (AD9082) on PS SPI0,
 * HMC7044 on PS SPI1, PSV (SBSA) console UART.  Device IDs resolve via
 * xilinx_compat.h (0/1 indices) on the SDT BSP.  No ADXCVR / CLKGEN IP.
 */
#define GPIO_DEVICE_ID		XPAR_AXI_GPIO_DEVICE_ID
#define PHY_SPI_DEVICE_ID	XPAR_XSPIPS_0_DEVICE_ID
#define CLK_SPI_DEVICE_ID	XPAR_XSPIPS_1_DEVICE_ID
#define UART_DEVICE_ID		XPAR_XUARTPSV_0_DEVICE_ID
#define UART_IRQ_ID		(XGet_IntrId(XPAR_XUARTPSV_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XUARTPSV_0_INTERRUPTS))
#define CLK_CS			0

/*
 * GT (serdes) reset GPIOs on the AXI GPIO -- these replace the ADXCVR on
 * Versal, where the GT line rate is fixed by the PDI and only the reset
 * handshake is driven from software.  Pin numbers from the Linux dts.
 */
#define RX_GT_RESET_DONE	(GPIO_OFFSET + 32)
#define TX_GT_RESET_DONE	(GPIO_OFFSET + 33)
#define RX_GT_PLL_RESET		(GPIO_OFFSET + 36)
#define TX_GT_PLL_RESET		(GPIO_OFFSET + 37)
#define RX_GT_DP_RESET		(GPIO_OFFSET + 38)
#define TX_GT_DP_RESET		(GPIO_OFFSET + 39)

#define RX_JESD_BASEADDR	XPAR_AXI_MXFE_RX_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR	XPAR_AXI_MXFE_TX_JESD_TX_AXI_BASEADDR

#ifdef XPAR_RX_MXFE_TPL_CORE_ADC_TPL_CORE_BASEADDR
#define RX_CORE_BASEADDR	XPAR_RX_MXFE_TPL_CORE_ADC_TPL_CORE_BASEADDR
#else
#define RX_CORE_BASEADDR	XPAR_RX_MXFE_TPL_CORE_TPL_CORE_BASEADDR
#endif
#ifdef XPAR_TX_MXFE_TPL_CORE_DAC_TPL_CORE_BASEADDR
#define TX_CORE_BASEADDR	XPAR_TX_MXFE_TPL_CORE_DAC_TPL_CORE_BASEADDR
#else
#define TX_CORE_BASEADDR	XPAR_TX_MXFE_TPL_CORE_TPL_CORE_BASEADDR
#endif

#define RX_DMA_BASEADDR		XPAR_AXI_MXFE_RX_DMA_BASEADDR
#define TX_DMA_BASEADDR		XPAR_AXI_MXFE_TX_DMA_BASEADDR

#ifdef IIO_SUPPORT
#define MAX_DAC_BUF_SAMPLES	10000000
#define MAX_ADC_BUF_SAMPLES	10000000
#endif

#endif /* _PARAMETERS_H_ */
