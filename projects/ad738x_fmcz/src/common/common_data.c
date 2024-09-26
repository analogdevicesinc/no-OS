/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-ad738x examples.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
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
#include "spi_engine.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_pwm.h"
/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
uint8_t in_buff[MAX_SIZE_BASE_ADDR] = {0};

struct no_os_uart_init_param ad738x_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_spi_init_param ad738x_spi_init_param = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_1,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

struct no_os_pwm_init_param trigger_pwm_init_param = {
	.id = TRIGGER_PWM_ID,
	.period_ns = TRIGGER_PERIOD_NS,
	.duty_cycle_ns = TRIGGER_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = TRIGGER_PWM_EXTRA,
};

struct ad738x_init_param ad738x_init_param = {
	.spi_param = &ad738x_spi_init_param,
	.clkgen_init = CLKGEN_INIT,
	.axi_clkgen_rate = 100000000,
	.offload_init_param = OFFLOAD_INIT,
	.dcache_invalidate_range =
	(void (*)(uint32_t, uint32_t))DCACHE_INVALIDATE,
	.pwm_init = &trigger_pwm_init_param,
	.conv_mode = ONE_WIRE_MODE,
	.ref_sel = INT_REF,
#ifdef USE_STANDARD_SPI
	.flags = AD738X_FLAG_STANDARD_SPI_DMA,
#else
	.flags = AD738X_FLAG_OFFLOAD,
#endif
};
