/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to xilinx platform used by ad469x_fmcz
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
#include "axi_pwm_extra.h"
#include "spi_engine.h"
#include "xilinx_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DCACHE_INVALIDATE		Xil_DCacheInvalidateRange

#define DMA_BASEADDR			XPAR_AXI_AD469X_DMA_BASEADDR
#define SPI_ENGINE_BASEADDR		XPAR_SPI_AD469X_SPI_AD469X_AXI_REGMAP_BASEADDR
#define RX_CLKGEN_BASEADDR		XPAR_SPI_CLKGEN_BASEADDR
#define AXI_PWMGEN_BASEADDR		XPAR_AD469X_TRIGGER_GEN_BASEADDR
#define SPI_ENG_REF_CLK_FREQ_HZ		XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ

#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID

#define NUM_SAMPLES			2000
#define BYTES_PER_SAMPLE		4
#define MAX_CHANNELS			17
#define ADC_DDR_BASEADDR		XPAR_DDR_MEM_BASEADDR + 0x800000
#define MAX_SIZE_BASE_ADDR		(NUM_SAMPLES * BYTES_PER_SAMPLE * MAX_CHANNELS)

extern struct xil_uart_init_param uart_extra_ip;
#define UART_EXTRA			&uart_extra_ip
#define UART_OPS			&xil_uart_ops
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR
#define UART_BAUDRATE			115200

extern struct axi_pwm_init_param pwm_extra_ip;
#define PWM_OPS				&axi_pwm_ops
#define PWM_EXTRA			&pwm_extra_ip
#define PWM_PERIOD			1000
#define PWM_DUTY			10

extern struct xil_gpio_init_param gpio_extra_ip;
#define GPIO_OPS			&xil_gpio_ops
#define GPIO_EXTRA			&gpio_extra_ip
#define GPIO_OFFSET			54
#define GPIO_RESETN_1			GPIO_OFFSET + 32
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID

extern struct spi_engine_init_param spi_eng_extra_ip;
#define SPI_DEVICE_ID			0
#define SPI_OPS				&spi_eng_platform_ops
#define SPI_EXTRA			&spi_eng_extra_ip
#define SPI_CS				0
#define SPI_BAUDRATE			80000000
#endif /* __PARAMETERS_H__ */
