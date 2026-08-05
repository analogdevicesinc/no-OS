/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Lattice platform used by ad738x_fmcz
 *           project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Axel Haslam (ahaslam@baylibre.com)
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <sys_platform.h>
#include "no_os_init.h"
#include "riscvrx_cache.h"
#include "riscvrx_ip_uart.h"

#include "axi_pwm_extra.h"
#include "spi_engine.h"

#define UART_DEVICE_ID      0
#define UART_IRQ_ID         UART0_INST_IRQ
#define UART_IRQ_PRIORITY   1
#define UART_BAUDRATE	    115200
#define UART_EXTRA		    &uart_extra_ip
#define UART_OPS		    &latt_ip_uart_ops
#define UART_ASYNC_RX       true

#define DCACHE_INVALIDATE		lattice_dcache_invalidate_range

#define LATTICE_UART_BASE_ADDR  UART0_INST_BASE_ADDR
#define DMA_BASEADDR		DMAC0_INST_BASE_ADDR
#define SPI_ENGINE_BASEADDR		AXI_SPI0_INST_BASE_ADDR
#define AXI_PWMGEN_BASEADDR		PWM0_INST_BASE_ADDR

#define SAMPLES_PER_CHANNEL_PLATFORM        1000
#define NUMBER_OF_CHANNELS                  1
#define BYTES_PER_SAMPLE		            2
#define MAX_SIZE_BASE_ADDR (SAMPLES_PER_CHANNEL_PLATFORM * NUMBER_OF_CHANNELS * BYTES_PER_SAMPLE)

#define CLOCK_OS                10000000
#define REFCLK_RATE			    160000000
#define UART_REFCLK_RATE        100000000
#define AXI_PWM_REFCLK_RATE     REFCLK_RATE
#define SPI_ENG_REFCLK_RATE     REFCLK_RATE

#define SPI_DEVICE_ID		0
#define SPI_OPS				&spi_eng_platform_ops
#define SPI_EXTRA			&spi_eng_init_param
#define SPI_CS				0
#define SPI_BAUDRATE	    50000000

#define TRIGGER_PWM_ID		0
#define PWM_OPS				&axi_pwm_ops
#define TRIGGER_PWM_EXTRA	&axi_pwm_init_param
#define TRIGGER_PERIOD_NS	500
#define TRIGGER_DUTY_NS		10

#define OFFLOAD_INIT        &spi_engine_offload_init_param

extern struct latt_ip_uart_init_param uart_extra_ip;
extern struct spi_engine_init_param spi_eng_init_param;
extern struct axi_pwm_init_param axi_pwm_init_param;
extern struct spi_engine_offload_init_param spi_engine_offload_init_param;
#endif /* __PARAMETERS_H__ */
