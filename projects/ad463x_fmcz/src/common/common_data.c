/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad463x_fmcz examples.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "no_os_pwm.h"
#include "no_os_spi.h"
#include "no_os_uart.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct no_os_uart_init_param ad463x_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS
};

struct spi_engine_offload_init_param spi_engine_offload_init_param = {
	.offload_config = OFFLOAD_RX_EN,
	.rx_dma_baseaddr = DMA_BASEADDR,
};

struct axi_clkgen_init clkgen_init = {
	.name = "rx_clkgen",
	.base = RX_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

struct no_os_pwm_init_param trigger_pwm_init = {
	.period_ns = 500,	/* 2Mhz */
	.duty_cycle_ns = AD463X_TRIGGER_PULSE_WIDTH_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = PWM_EXTRA,
};

struct no_os_gpio_init_param ad463x_resetn = {
	.number = GPIO_RESETN_1,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

#if ADAQ4224_DEV
/* PGIA gain control inputs */
struct no_os_gpio_init_param ad463x_pgia_a0 = {
	.number = GPIO_PGIA_0,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};
struct no_os_gpio_init_param ad463x_pgia_a1 = {
	.number = GPIO_PGIA_1,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};
#endif

struct no_os_spi_init_param spi_init = {
	.chip_select = SPI_CS,
	.max_speed_hz = 80000000,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.extra = (void*)SPI_EXTRA,
};

struct ad463x_init_param ad463x_init_param = {
	.spi_init = &spi_init,
	.offload_init_param = &spi_engine_offload_init_param,
	.trigger_pwm_init = &trigger_pwm_init,
	.gpio_resetn = &ad463x_resetn,
	.clkgen_init = &clkgen_init,
	.axi_clkgen_rate = 160000000,
	.reg_access_speed = 20000000,
	.reg_data_width = 8,
	.output_mode = AD463X_24_DIFF,
	.lane_mode = AD463X_ONE_LANE_PER_CH,
	.clock_mode = AD463X_SPI_COMPATIBLE_MODE,
	.data_rate = AD463X_SDR_MODE,
#if ADAQ4224_DEV
	.vref = 4096 * 1000UL,
	.device_id = ID_ADAQ4224, /* dev_id */
	.gpio_pgia_a0 = &ad463x_pgia_a0,
	.gpio_pgia_a1 = &ad463x_pgia_a1,
#elif AD4030_DEV
	.vref = 5000 * 1000UL,
	.device_id = ID_AD4030, /* dev_id */
#else
	.vref = 5000 * 1000UL,
	.device_id = ID_AD4630_24, /* dev_id */
#endif
	.dcache_invalidate_range =
	(void (*)(uint32_t, uint32_t))DCACHE_INVALIDATE,
};

