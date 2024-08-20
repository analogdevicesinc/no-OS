/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Lattice platform used by ad738x_fmcz
 *           project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Axel Haslam (ahaslam@baylibre.com)
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
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
#include <sys_platform.h>
#include "no_os_init.h"
#include "lattice_cache.h"
#include "lattice_uart.h"

#include "axi_pwm_extra.h"
#include "spi_engine.h"

#define INPUT_BUFFER        SYSMEM_DMAC0_INST_BASE_ADDR

#define UART_DEVICE_ID      0
#define UART_IRQ_ID         0
#define UART_BAUDRATE	    115200
#define UART_EXTRA			&uart_extra_ip
#define UART_OPS			&latt_uart_ops

#define DCACHE_INVALIDATE		lattice_dcache_invalidate_range

#define LATTICE_UART_BASE_ADDR  LOCAL_UART_INST_BASE_ADDR
#define DMA_BASEADDR			DMAC0_INST_BASE_ADDR
#define SPI_ENGINE_BASEADDR		AXI_SPI0_INST_BASE_ADDR
#define AXI_PWMGEN_BASEADDR		PWM0_INST_BASE_ADDR

#define SAMPLES_PER_CHANNEL_PLATFORM        1000
#define NUMBER_OF_CHANNELS                  4
#define MAX_SIZE_BASE_ADDR (SAMPLES_PER_CHANNEL_PLATFORM * NUMBER_OF_CHANNELS * sizeof(uint32_t))

#define SPI_ENG_REF_CLK_FREQ_HZ		XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ

#define REFCLK_RATE			    100000000
#define UART_REFCLK_RATE        REFCLK_RATE
#define AXI_PWM_REFCLK_RATE     REFCLK_RATE
#define SPI_ENG_REFCLK_RATE     REFCLK_RATE
#define AXI_CLK_GEN_REFCLK_RATE 100000000

#define SPI_DEVICE_ID			0
#define SPI_OPS				&spi_eng_platform_ops
#define SPI_EXTRA			&spi_eng_init_param
#define SPI_CS				0
#define SPI_BAUDRATE	    50000000

#define NO_OS_PWM_ID			0
#define PWM_OPS				&axi_pwm_ops
#define PWM_EXTRA			&ad738x_axi_pwm_init
#define TRIGGER_PERIOD_NS		500
#define TRIGGER_DUTY_NS			10

extern struct latt_uart_init_param uart_extra_ip;
extern struct spi_engine_init_param spi_eng_init_param;
extern struct axi_pwm_init_param ad738x_axi_pwm_init;
#endif /* __PARAMETERS_H__ */
