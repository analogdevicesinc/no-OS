/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform dependent parameters for ADAR300X project (Xilinx).
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "xparameters.h"
#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#include "xilinx_uart.h"

/*
 * This project targets the ZynqMP PS on a ZCU102 only, so the peripherals are
 * named unconditionally rather than behind a _XPARAMETERS_PS_H_ test: that
 * macro is absent from Vitis 2025+ BSPs, and falling through to the MicroBlaze
 * branch silently resolves every id to 0 against the compat shims.
 *
 * Vitis 2025+ emits SDT BSPs whose LookupConfig() matches on base address and
 * which no longer define *_DEVICE_ID; legacy BSPs match on device id. Both are
 * supplied here and -DSDT (set by this project's CMakeLists) picks the one the
 * drivers consume.
 *
 * SPI0 and UART0 are the instances actually wired: the HDL top level connects
 * spi0_{sclk,csn,miso,mosi} to the FMC HPC1 pins and stubs spi1 off, and UART0
 * is the ZCU102 USB-UART console on MIO18/19.
 */
#define GPIO_BASEADDR			XPAR_XGPIOPS_0_BASEADDR
#define SPI_BASEADDR			XPAR_XSPIPS_0_BASEADDR
#define UART_BASEADDR			XPAR_XUARTPS_0_BASEADDR

#define GPIO_DEVICE_ID			XPAR_XGPIOPS_0_DEVICE_ID
#define SPI_DEVICE_ID			XPAR_XSPIPS_0_DEVICE_ID
#define UART_DEVICE_ID			XPAR_PSU_UART_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_0_INTR

#define UART_BAUDRATE			115200
#define UART_OPS			&xil_uart_ops
#define UART_EXTRA			&adar300x_uart_extra

/* spi_sel_a = spi0_csn[0] in the HDL top level */
#define SPI_CS				0
#define SPI_OPS				&xil_spi_ops
#define SPI_EXTRA			&adar300x_spi_extra

#define GPIO_OPS			&xil_gpio_ops
#define GPIO_EXTRA			&adar300x_gpio_extra

/*
 * ZynqMP GPIO banks 0..2 are the 78 MIO pins, so EMIO bit n is GPIO 78 + n.
 * The HDL top level routes gpio0..gpio7 from gpio_o[32..39], putting the ADAR
 * control pins at 78 + 32 + n. A wrong offset aims the driver at MIO pins that
 * belong to the carrier, so this must not be left to a fallback.
 */
#define GPIO_OFFSET			78

#define GPIO_MUX_SEL			(GPIO_OFFSET + 32 + 3)
#define GPIO_UPDATE			(GPIO_OFFSET + 32 + 4)
#define GPIO_BEAM_RESET			(GPIO_OFFSET + 32 + 5)
#define GPIO_MUTE			(GPIO_OFFSET + 32 + 6)
#define GPIO_RSTB			(GPIO_OFFSET + 32 + 7)

extern struct xil_uart_init_param adar300x_uart_extra;
extern struct xil_spi_init_param adar300x_spi_extra;
extern struct xil_gpio_init_param adar300x_gpio_extra;

#endif /* _PARAMETERS_H_ */
