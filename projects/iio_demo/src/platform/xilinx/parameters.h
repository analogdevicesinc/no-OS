/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to xilinx platform used by iio_demo
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xparameters.h>
#include <xil_cache.h>
#include <xilinx_uart.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID		XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR
#endif

#else // _XPARAMETERS_PS_H_
#define UART_DEVICE_ID	XPAR_AXI_UART_DEVICE_ID
#define INTC_DEVICE_ID	XPAR_INTC_SINGLE_DEVICE_ID
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#endif // _XPARAMETERS_PS_H_

#define UART_EXTRA		&iio_demo_uart_extra_ip
#define UART_OPS		&xil_uart_ops

extern struct xil_uart_init_param iio_demo_uart_extra_ip;

/* This value can be modified based on the number
of samples needed to be stored in the device buffer
and based on the available RAM memory of the platform */
#define SAMPLES_PER_CHANNEL_PLATFORM    2000
#define UART_BAUDRATE	                115200

#endif /* __PARAMETERS_H__ */
