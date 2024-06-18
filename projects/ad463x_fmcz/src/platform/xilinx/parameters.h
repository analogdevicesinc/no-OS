/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to xilinx platform used by ad463x_fmcz
 *           project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <xparameters.h>
#include <xil_cache.h>
#include <xilinx_uart.h>
#include "xilinx_gpio.h"

#include "axi_pwm_extra.h"
#include "spi_engine.h"

#if 0
#define AD463x_DMA_BASEADDR             XPAR_AXI_AD463X_DMA_BASEADDR
#define AD463x_SPI_ENGINE_BASEADDR      XPAR_SPI_AD463X_SPI_AD463X_AXI_REGMAP_BASEADDR
#define RX_CLKGEN_BASEADDR		XPAR_SPI_CLKGEN_BASEADDR
#define AXI_PWMGEN_BASEADDR		XPAR_CNV_GENERATOR_BASEADDR

#define AD463x_SPI_CS                   0

#define GPIO_OFFSET			54
#define GPIO_RESETN_1			GPIO_OFFSET + 32
#define GPIO_PGIA_0			GPIO_OFFSET + 33
#define GPIO_PGIA_1			GPIO_OFFSET + 34
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID

#define UART_BAUDRATE			115200
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif
/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID

#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID			XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR
#endif

#else // _XPARAMETERS_PS_H_
#define UART_DEVICE_ID			XPAR_AXI_UART_DEVICE_ID
#define INTC_DEVICE_ID			XPAR_INTC_SINGLE_DEVICE_ID
#define UART_IRQ_ID			XPAR_AXI_INTC_AXI_UART_INTERRUPT_INTR
#endif // _XPARAMETERS_PS_H_


#define UART_BAUDRATE			115200
#define UART_EXTRA			&uart_extra_ip
#define UART_OPS			&xil_uart_ops

#define DCACHE_INVALIDATE		Xil_DCacheInvalidateRange

#define DMA_BASEADDR			XPAR_AXI_AD463X_DMA_BASEADDR
#define SPI_ENGINE_BASEADDR		XPAR_SPI_AD463X_SPI_AD463X_AXI_REGMAP_BASEADDR
#define RX_CLKGEN_BASEADDR		XPAR_SPI_CLKGEN_BASEADDR
#define AXI_PWMGEN_BASEADDR		XPAR_CNV_GENERATOR_BASEADDR

#define SAMPLES_PER_CHANNEL_PLATFORM	100000
#define MAX_SIZE_BASE_ADDR		(SAMPLES_PER_CHANNEL_PLATFORM * 2 * sizeof(uint32_t))

#define SPI_ENG_REF_CLK_FREQ_HZ		XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ

#define REFCLK_RATE			160000000

#define SPI_DEVICE_ID			0
#define SPI_OPS				&spi_eng_platform_ops
#define SPI_EXTRA			&spi_eng_init_param
#define SPI_CS				0
#define SPI_BAUDRATE			80000000

#define NO_OS_PWM_ID			0
#define PWM_OPS				&axi_pwm_ops
#define PWM_EXTRA			&ad4630_axi_pwm_init
#define TRIGGER_PERIOD_NS		500
#define TRIGGER_DUTY_NS			10

#define AD400X_ADC_REF_VOLTAGE		5000

#define GPIO_OPS			&xil_gpio_ops
#define GPIO_EXTRA			&gpio_extra_param
#define GPIO_OFFSET			54
#define GPIO_RESETN_1			GPIO_OFFSET + 32
#define GPIO_RESETN_PORT		0

#define GPIO_PGIA_0			GPIO_OFFSET + 33
#define GPIO_PGIA_0_PORT		0
#define GPIO_PGIA_1			GPIO_OFFSET + 34
#define GPIO_PGIA_1_PORT		0

#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID


extern struct xil_uart_init_param uart_extra_ip;
extern struct spi_engine_init_param spi_eng_init_param;
extern struct axi_pwm_init_param ad4630_axi_pwm_init;
extern struct xil_gpio_init_param gpio_extra_param;
#endif /* __PARAMETERS_H__ */
