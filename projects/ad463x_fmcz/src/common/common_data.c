/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad463x_fmcz examples.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
uint8_t in_buff[MAX_SIZE_BASE_ADDR] = {0};

struct no_os_uart_init_param ad463x_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS
};

#if defined(USE_STANDARD_SPI)
struct no_os_gpio_init_param ad463x_cnv = {
	.number = GPIO_CNV,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
	.port = GPIO_CNV_PORT,
};

struct no_os_gpio_init_param cnv_pwm_gpio_params = {
	.port = GPIO_CNV_PORT,
	.number = GPIO_CNV,
	.platform_ops = GPIO_OPS,
	.extra = PWM_GPIO_EXTRA,
};
#else
struct spi_engine_offload_init_param spi_engine_offload_init_param = {
	.offload_config = OFFLOAD_RX_EN,
	.rx_dma_baseaddr = DMA_BASEADDR,
};

struct axi_clkgen_init clkgen_init = {
	.name = "rx_clkgen",
	.base = RX_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

#endif

struct no_os_pwm_init_param trigger_pwm_init = {
	.id = NO_OS_PWM_ID,
	.period_ns = TRIGGER_PERIOD_NS,
	.duty_cycle_ns = TRIGGER_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
#if defined(USE_STANDARD_SPI)
	.pwm_gpio = &cnv_pwm_gpio_params,
#endif
	.platform_ops = PWM_OPS,
	.extra = PWM_EXTRA,
};

struct no_os_gpio_init_param ad463x_resetn = {
	.number = GPIO_RESETN_1,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
	.port = GPIO_RESETN_PORT,
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
	.device_id = SPI_DEVICE_ID,
	.chip_select = SPI_CS,
	.max_speed_hz = SPI_BAUDRATE,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.extra = (void*)SPI_EXTRA,
};

struct ad463x_init_param ad463x_init_param = {
	.spi_init = &spi_init,
#if defined(USE_STANDARD_SPI)
	.gpio_cnv = &ad463x_cnv,
	.lane_mode = AD463X_SHARED_TWO_CH,
	.spi_dma_enable = true,
#else
	.offload_init_param = &spi_engine_offload_init_param,
	.axi_clkgen_rate = 160000000,
	.clkgen_init = &clkgen_init,
	.lane_mode = AD463X_ONE_LANE_PER_CH,
	.offload_enable = true,
#endif
	.trigger_pwm_init = &trigger_pwm_init,
	.gpio_resetn = &ad463x_resetn,
	.reg_access_speed = 20000000,
	.reg_data_width = 8,
	.output_mode = AD463X_24_DIFF,
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
