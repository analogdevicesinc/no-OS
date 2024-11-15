/***************************************************************************//**
* @file parameters.h
* @brief Parameters definition for AD7606X-FMC.
* @author Alexandru Ardelean (aardelean@baylibre.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "xparameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD7606X_MODE_SPI_ENGINE			0
#define AD7606X_MODE_PARALLEL			1

#if defined(XPAR_AXI_SPI_ENGINE_0_BASEADDR)
#define AD7606X_CORE_BASEADDR			XPAR_AXI_SPI_ENGINE_0_BASEADDR
#define AD7606X_MODE				AD7606X_MODE_SPI_ENGINE
#define AD7606X_RX_CLKGEN_BASEADDR		XPAR_SPI_CLKGEN_BASEADDR
#elif defined(XPAR_AXI_AD7606X_BASEADDR)
#define AD7606X_CORE_BASEADDR			XPAR_AXI_AD7606X_BASEADDR
#define AD7606X_MODE				AD7606X_MODE_PARALLEL
#define AD7606X_RX_CLKGEN_BASEADDR		0
#endif

#define AD7606X_DMA_BASEADDR			XPAR_AXI_DMAC_0_BASEADDR
#define AXI_PWMGEN_BASEADDR			XPAR_AXI_PWM_GEN_0_BASEADDR
#define SPI_AD7606X_CS				0
#define GPIO_DEVICE_ID				XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET				(32 + 54)
#define GPIO_ADC_OS0				(GPIO_OFFSET + 0)
#define GPIO_ADC_OS1				(GPIO_OFFSET + 1)
#define GPIO_ADC_OS2				(GPIO_OFFSET + 2)
#define GPIO_ADC_RANGE				(GPIO_OFFSET + 3)
#define GPIO_ADC_STBY				(GPIO_OFFSET + 4)
#define GPIO_ADC_RESET				(GPIO_OFFSET + 5)
#define GPIO_ADC_FIRST_DATA			(GPIO_OFFSET + 6)
#define GPIO_ADC_SER_PAR			(GPIO_OFFSET + 7)
#define ADC_DDR_BASEADDR			(XPAR_DDR_MEM_BASEADDR + 0x800000)

#define AD7606X_TRIGGER_PULSE_WIDTH_NS		120

#ifdef _XPARAMETERS_PS_H_
#define UART_DEVICE_ID				XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID				XPAR_SCUGIC_SINGLE_DEVICE_ID

#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID				XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#endif

#else // _XPARAMETERS_PS_H_
#define UART_DEVICE_ID				XPAR_AXI_UART_DEVICE_ID
#define INTC_DEVICE_ID				XPAR_INTC_SINGLE_DEVICE_ID
#define UART_IRQ_ID				XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#endif // _XPARAMETERS_PS_H_

#define UART_BAUDRATE				115200
#define UART_EXTRA				&uart_extra_ip
#define UART_OPS				&xil_uart_ops

#endif /* PARAMETERS_H_ */
