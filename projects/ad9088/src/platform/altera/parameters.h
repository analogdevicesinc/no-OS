/***************************************************************************//**
 *   @file   altera/parameters.h
 *   @brief  Definitions specific to the Altera/Nios V platform of the ad9088
 *           project (AD9084-EBZ on Agilex 5).
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "system.h"		/* Generated Nios V BSP: *_BASE, *_IRQ, ... */
#include "altera_spi.h"
#include "altera_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/*
 * PORTING NOTE - AD9084-EBZ Agilex 5 design (ad9084_ebz_nios_a5e).
 *
 * The base addresses below are the absolute, CPU-view (sys_cpu.data_manager)
 * addresses extracted from the EBZ design's software handoff
 * (projects/ad9088/system_bd.sopcinfo) and cross-checked live over JTAG:
 * the JTAG UART (0x100) and the clock-domain Avalon SPI (sys_spi @ 0x40) were
 * confirmed by reading their registers and round-tripping the ADF4382
 * scratchpad (0x5A/0xA5) with the core halted.
 *
 * The EBZ map does NOT match the generic soft-SoC BSP (system.h), whose
 * peripherals live at 0x90xxx. We therefore hard-code the EBZ addresses here
 * instead of using the generic system.h *_BASE symbols. system.h is still
 * included for CPU/HAL constants.
 *
 * Address map (span):
 *   sys_gpio_in   0x00000   sys_gpio_out 0x00020   sys_spi   0x00040 (clocks)
 *   sys_gpio_bd   0x000d0   sys_uart     0x00100   sys_timer 0x00200
 *   apollo_rx_jesd204 link_reconfig 0xc0000 / link_management 0xc4000
 *   apollo_tx_jesd204 link_reconfig 0xc8000 / link_management 0xcc000
 *   apollo_rx_tpl 0xd2000   apollo_tx_tpl 0xd4000
 *   apollo_rx_dma 0xd8000   apollo_tx_dma 0xdc000
 *   sys_gpio_out  0x00020 (AD9084 resetb=bit30) apollo_spi 0xea000 (AD9084)
 *   apollo_gpio   0xe8000 (Apollo IRQ/status, input-only)
 *   apollo_rx_data_offload 0x100000   apollo_tx_data_offload 0x110000
 *   jesd204_phy_a 0x1000000 (RX xcvr)  jesd204_phy_b 0x2000000 (TX xcvr)
 *   sys_int_mem   0x10000000 (1.5M OCM) sys_ddr_window 0x10200000 (1M window)
 */

/*
 * UART: the EBZ JTAG UART (sys_uart) is at 0x100. The generic BSP's HAL stdio
 * targets the generic design's JTAG UART (ALT_STDOUT_BASE 0x90158), which does
 * not exist on this board, so pr_*() must NOT go through libc stdio here - it is
 * routed through a polled no-OS JTAG UART at UART_JTAG_BASEADDR (see main.c).
 */
#define UART_DEVICE_ID			0
#define UART_BAUDRATE			115200
#define UART_OPS			NULL
#define UART_EXTRA			NULL
#define UART_IRQ_ID			0
#define UART_JTAG_BASEADDR		0x00000100 /* sys_uart (JTAG UART) */

/*
 * SPI: the EBZ has two Avalon SPI cores.
 *   sys_spi    @ 0x40    - clock domain (ADF4382 CS0 / HMC7044 CS1 / ADF4030 CS4)
 *   apollo_spi @ 0xEA000 - Apollo/AD9084 (CS0)
 * Chip-selects below are the board wiring.
 */
extern struct altera_spi_init_param	clk_spi_extra_ip;
extern struct altera_spi_init_param	apollo_spi_extra_ip;

#define CLK_SPI_BASEADDR		0x00000040 /* sys_spi    (clocks) */
#define APOLLO_SPI_BASEADDR		0x000EA000 /* apollo_spi (AD9084) */

#define CLK_SPI_DEVICE_ID		0
#define SPI_OPS_CLK			&altera_spi_ops
#define SPI_EXTRA_CLK			&clk_spi_extra_ip
#define SPI_CS_ADF4382			0
#define SPI_CS_HMC7044			1
#define SPI_CS_ADF4030			4

#define APOLLO_SPI_DEVICE_ID		0
#define SPI_OPS_APOLLO			&altera_spi_ops
#define SPI_EXTRA_APOLLO		&apollo_spi_extra_ip
#define SPI_CS_APOLLO			0

/*
 * GPIO: the AD9084 resetb pin is driven by sys_gpio_out (0x20) bit 30.
 * HDL nios_a5e/system_top.v: `assign resetb = gpio_o[62]` with
 * sys_gpio_out_export = gpio_o[63:32], so resetb is sys_gpio_out bit 62-32 = 30.
 * The altera_gpio driver addresses a PIO data bit as (number - 32), so
 * number = GPIO_OFFSET + GPIO_RESET_N = 32 + 30 = 62 selects that bit.
 * PIO outputs power up at 0, i.e. resetb=0 (held in reset), until software
 * drives this high. NOTE: apollo_gpio (0xE8000) is a SEPARATE input-only PIO
 * (Apollo IRQ/status, width 20) and cannot drive reset - do not use it here.
 */
extern struct altera_gpio_init_param	altera_gpio_param;

#define GPIO_BASEADDR			0x00000020 /* sys_gpio_out (resetb=bit 30)*/
#define GPIO_OPS			&altera_gpio_ops
#define GPIO_EXTRA			&altera_gpio_param
#define GPIO_OFFSET			32
#define GPIO_RESET_N			30

/*
 * GTS transceiver refclk bring-up (Agilex 5, GTS PHY). The GTS PHYs get no
 * reference clock until software gates them on. HDL nios_a5e/system_top.v:
 *   refclk_ready_1 = gpio_o[56] (RX), refclk_ready_0 = gpio_o[57] (TX)
 * both drive the gts_refclk_reset state machine, and the GTS TX PLL refclk
 * buffer is then requested on through each bank's "gts-pll" reconfig window
 * (reg 0x0e, byte +2 <- 0xff). The HDL instantiates ONE intel_systemclk_gts
 * PLL per bank (a5e/system_qsys.tcl: foreach pll {a b}) - "a single PLL only
 * ever brought up the bank whose refclk it was driven from" - so BOTH banks
 * must be poked. Each PLL's register sits at the same +0xA6000 offset inside
 * its PHY's 4 MB reconfig aperture (ad9084_ebz_qsys.tcl: jesd204_phy_a @
 * 0x01000000 = RX, jesd204_phy_b @ 0x02000000 = TX); the TX address matches
 * the Linux ad9084 DTS gts-pll window (0x020a6000).
 * A PIO data bit is addressed as (number - 32), so gpio_o[56]/[57] are
 * GPIO_OFFSET + 24/25 = 56/57.
 */
#define GPIO_REFCLK_READY_RX		(GPIO_OFFSET + 24) /* gpio_o[56] */
#define GPIO_REFCLK_READY_TX		(GPIO_OFFSET + 25) /* gpio_o[57] */
#define GTS_PLL_RX_BASEADDR		0x010A6000 /* gts_pll_a (PHY A / RX) */
#define GTS_PLL_TX_BASEADDR		0x020A6000 /* gts_pll_b (PHY B / TX) */
#define GTS_REFCLK_BUFFER_WORD		0x38	/* reg 0x0e * 4 */
#define GTS_REFCLK_BUFFER_REQ_OFFSET	0x3A	/* reg 0x0e byte +2 */

/* Capture depth, in samples per converter. */
#define ADC_BUFFER_SAMPLES		16384

/*
 * AD9084 datapath - absolute CPU-view addresses from system_bd.sopcinfo,
 * cross-checked against the HDL (adi_jesd204 hw.tcl interface exports).
 *
 * The adi_jesd204 wrapper (apollo_rx/tx_jesd204) exposes TWO AXI-lite slaves,
 * with counter-intuitive names:
 *   link_reconfig   (0xc0000 RX / 0xc8000 TX) = EXPORT_OF axi_jesd204_rx/tx.s_axi
 *                    -> the JESD204 LINK core (VERSION@0x00, MAGIC@0x0c,
 *                       LINK_DISABLE@0xc0, LINK_STATUS@0x280). This is RX/TX_JESD.
 *   link_management (0xc4000 RX / 0xcc000 TX) = EXPORT_OF axi_xcvr.s_axi
 *                    -> the axi_adxcvr control core (SYNTH_CONF@0x24, RESETN@0x10,
 *                       STATUS@0x14). This is RX/TX_XCVR.
 * The raw E-Tile PHY reconfig buses (jesd204_phy_a/b @ 0x1000000 / 0x2000000)
 * are low-level Avalon slaves NOT driven by the adxcvr driver.
 */
#define RX_DMA_BASEADDR			0x000D8000 /* apollo_rx_dma           */
#define TX_DMA_BASEADDR			0x000DC000 /* apollo_tx_dma           */
#define TX_DATA_OFFLOAD_BASEADDR	0x00110000 /* apollo_tx_data_offload  */
#define RX_DATA_OFFLOAD_BASEADDR	0x00100000 /* apollo_rx_data_offload  */
#define RX_CORE_BASEADDR		0x000D2000 /* apollo_rx_tpl           */
#define TX_CORE_BASEADDR		0x000D4000 /* apollo_tx_tpl           */
#define RX_JESD_BASEADDR		0x000C0000 /* apollo_rx_jesd204 link  */
#define TX_JESD_BASEADDR		0x000C8000 /* apollo_tx_jesd204 link  */
#define RX_XCVR_BASEADDR		0x000C4000 /* apollo_rx_jesd204 adxcvr*/
#define TX_XCVR_BASEADDR		0x000CC000 /* apollo_tx_jesd204 adxcvr*/

#endif /* __PARAMETERS_H__ */
