/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to xilinx platform used by iio
 *           project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "xilinx_gpio.h"
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
#define UART_DEVICE_ID		XPAR_AXI_UART_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_SINGLE_DEVICE_ID
#define UART_IRQ_ID		XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#endif // _XPARAMETERS_PS_H_

#define UART_EXTRA		&iio_uart_extra_ip
#define UART_OPS		&xil_uart_ops

extern struct xil_uart_init_param iio_uart_extra_ip;

#define RX_CORE_BASEADDR	XPAR_AXI_PULSAR_LVDS_BASEADDR
#define RX_DMA_BASEADDR		XPAR_AXI_PULSAR_LVDS_DMA_BASEADDR
#define RX_CLKGEN_BASEADDR	XPAR_REFERENCE_CLKGEN_BASEADDR
#define AXI_PWMGEN_BASEADDR	XPAR_AXI_PWM_GEN_BASEADDR
#define ADC_DDR_BASEADDR	XPAR_DDR_MEM_BASEADDR + 0x800000

#define GPIO_DEVICE_ID		XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET		32 + 54

#define GPIO_EN0_FMC		GPIO_OFFSET
#define GPIO_EN1_FMC		GPIO_OFFSET+1
#define GPIO_EN2_FMC		GPIO_OFFSET+2
#define GPIO_EN3_FMC		GPIO_OFFSET+3

/* This value can be modified based on the number
of samples needed to be stored in the device buffer
and based on the available RAM memory of the platform */
#define SAMPLES_PER_CHANNEL_PLATFORM	20000
#define MAX_SIZE_BASE_ADDR		(SAMPLES_PER_CHANNEL_PLATFORM * 4)

#define UART_BAUDRATE		115200

extern struct axi_pwm_init_param axi_pwm_0_extra;
#define PWM_0_PERIOD_NS		200
#define PWM_0_DUTY_NS		16
#define PWM_0_PHASE		0

#define PWM_1_PERIOD_NS		200
#define PWM_1_DUTY_NS		144
#define PWM_1_PHASE		0

#define GPIO_OPS		&xil_gpio_ops
#define GPIO_EXTRA		&xil_gpio_init

#define DCACHE_INVALIDATE	Xil_DCacheInvalidateRange

#endif /* __PARAMETERS_H__ */

