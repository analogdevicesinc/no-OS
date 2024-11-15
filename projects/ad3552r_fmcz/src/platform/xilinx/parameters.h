/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <xparameters.h>
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "xil_cache.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* GPIO Indexes */
#ifndef XILINX_PLATFORM
#define GPIO_RESET_N				0
#define GPIO_LDAC_N				1
#define GPIO_SPI_QPI				2
#define GPIO_ALERT_N				3
#define GPIO_SYNC_EVENTS			4
#define GPIO_RED				5 //GPIO_6
#define GPIO_GREEN				6 //GPIO_7
#define GPIO_BLUE				7 //GPIO_8
#define TOTAL_GPIOS				8
#else
#define GPIO_LDAC_N		0
#define GPIO_ALERT_N		1
#define GPIO_RED		2 /* GPIO_6 */
#define GPIO_GREEN		3 /* GPIO_7 */
#define GPIO_BLUE		4 /* GPIO_8 */
#define GPIO_9			5
#define GPIO_RESET_N		6
#define GPIO_SPI_QPI		7
#define TOTAL_GPIOS		8
#endif /* XILINX_PLATFORM */

#define GPIO_BANK_0_PINS	32
#define GPIO_BANK_1_PINS	22
#define GPIO_BANK_2_PINS	32
#define GPIO_BANK_3_OFFSET	(GPIO_BANK_0_PINS + GPIO_BANK_1_PINS + \
				 GPIO_BANK_2_PINS)

#define GPIO_OFFSET		GPIO_BANK_3_OFFSET

#define MAX_BUFF_SAMPLES	10000
#define UART_BAUDRATE		115200

#define SPI_DEVICE_ID		XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID		XPAR_PS7_GPIO_0_DEVICE_ID
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

#define UART_IRQ_ID		XPAR_XUARTPS_1_INTR

#define TX_CORE_BASEADDR        XPAR_AXI_AD3552R_DAC_BASEADDR
#define TX_DMA_BASEADDR         XPAR_AXI_DAC_DMA_BASEADDR
#define TX_CLKGEN_BASEADDR      XPAR_AXI_CLKGEN_BASEADDR

#define GPIO_OPS		&xil_gpio_ops
#define GPIO_EXTRA		&xil_gpio_param
#define SPI_EXTRA		&xil_spi_param

#endif // __PARAMETERS_H__
