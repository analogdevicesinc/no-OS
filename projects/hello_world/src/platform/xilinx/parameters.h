/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Xilinx platform parameters for hello_world project.
 *
 *           This file selects the correct UART and GPIO peripheral identifiers
 *           based on the target hardware. All values come from the BSP-generated
 *           xparameters.h, so the same source builds correctly for:
 *
 *             - Any Zynq-7000 board (Cora Z7, ZED, ZC706, ...): PS UART 1
 *             - ZCU102 (ZynqMP): PS UART 0
 *             - MicroBlaze (PL-only): AXI UART Lite
 *
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include <xparameters.h>
#include <xil_cache.h>
#include <xilinx_uart.h>
#include <xilinx_gpio.h>

/*
 * _XPARAMETERS_PS_H_ is defined whenever the design contains an ARM
 * Processing System (PS) -- i.e. Zynq-7000 or ZynqMP.  When it is NOT
 * defined we are running on a MicroBlaze soft-core inside the PL and must
 * use an AXI UART Lite peripheral instead.
 */
#ifdef _XPARAMETERS_PS_H_

#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define GPIO_DEVICE_ID		XPAR_XGPIOPS_0_DEVICE_ID

/*
 * The ZCU102 routes its JTAG/USB-UART adapter to UART0; all other Zynq-7000
 * and ZynqMP boards (Cora Z7, ZED, ZC706, ...) use UART1.
 *
 * LED_GPIO_PIN is the PS GPIO number of an LED on the board. It is optional:
 * the example only blinks an LED on the boards that define it. On the ZCU102
 * this is the PS_LED1 LED (DS50), wired to the PS pin MIO23.
 */
#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID		XPAR_XUARTPS_0_INTR
#define LED_GPIO_PIN		23
#else
#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR
#endif

#else /* MicroBlaze / PL-only */

#define UART_DEVICE_ID		XPAR_AXI_UART_DEVICE_ID
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR

#endif /* _XPARAMETERS_PS_H_ */

#define UART_BAUDRATE		115200
#define UART_EXTRA		&hello_world_uart_extra_ip
#define UART_OPS		&xil_uart_ops

#ifdef LED_GPIO_PIN
#define GPIO_EXTRA		&hello_world_gpio_extra_ip
#define GPIO_OPS		&xil_gpio_ops

extern struct xil_gpio_init_param hello_world_gpio_extra_ip;
#endif

extern struct xil_uart_init_param hello_world_uart_extra_ip;

#endif /* PARAMETERS_H_ */
