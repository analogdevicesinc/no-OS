/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to xilinx platform used by pulsar-adc
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
#include <stdio.h>
#include <xparameters.h>
#include <xil_cache.h>
#include <xilinx_uart.h>

#include "axi_pwm_extra.h"
#include "spi_engine.h"

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

#define UART_EXTRA			&uart_extra_ip
#define UART_OPS			&xil_uart_ops

#define DCACHE_INVALIDATE		Xil_DCacheInvalidateRange

#define UART_BAUDRATE			115200

#define DMA_BASEADDR			XPAR_AXI_PULSAR_ADC_DMA_BASEADDR
#define SPI_ENGINE_BASEADDR		XPAR_SPI_PULSAR_ADC_SPI_PULSAR_ADC_AXI_REGMAP_BASEADDR
#define RX_CLKGEN_BASEADDR		XPAR_SPI_CLKGEN_BASEADDR
#define AXI_PWMGEN_BASEADDR		XPAR_PULSAR_ADC_TRIGGER_GEN_BASEADDR
#define ADC_DDR_BASEADDR		(XPAR_DDR_MEM_BASEADDR + 0x800000)

#define SAMPLES_PER_CHANNEL_PLATFORM	2000
#define MAX_SIZE_BASE_ADDR		(SAMPLES_PER_CHANNEL_PLATFORM * sizeof(uint32_t))

#define SPI_ENG_REF_CLK_FREQ_HZ		XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ

#define REFCLK_RATE			160000000

#define SPI_DEVICE_ID			0
#define SPI_OPS				&spi_eng_platform_ops
#define SPI_EXTRA			&spi_eng_init_param
#define SPI_CS				0
#define SPI_BAUDRATE			80000000

#define PWM_OPS				&axi_pwm_ops
#define PWM_EXTRA			&pulsar_adc_axi_pwm_init
#define PWM_PERIOD			555
#define PWM_DUTY			10

#define PULSAR_ADC_ADC_REF_VOLTAGE		5000

extern struct xil_uart_init_param uart_extra_ip;
extern struct spi_engine_init_param spi_eng_init_param;
extern struct axi_pwm_init_param pulsar_adc_axi_pwm_init;
#endif /* __PARAMETERS_H__ */
