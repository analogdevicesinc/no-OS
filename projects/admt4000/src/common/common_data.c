/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADMT4000 examples.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "common_data.h"
#include "no_os_gpio.h"

struct no_os_uart_init_param admt4000_uart_ip = {
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

const struct no_os_spi_init_param admt4000_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
};

const struct no_os_gpio_init_param gpio_coil_rs_ip = {
	.port = GPIO_SPI_SEL_PORT,
	.number = GPIO_COIL_RS,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param gpio_busy_ip = {
	.port = GPIO_SPI_SEL_PORT,
	.number = GPIO_GPIO0_BUSY,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param gpio_shdn_n_ip = {
	.port = GPIO_SPI_SEL_PORT,
	.number = GPIO_SHDN_N,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param gpio_cnv_ip = {
	.port = GPIO_SPI_SEL_PORT,
	.number = GPIO_CNV,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param gpio_acalc_ip = {
	.port = GPIO_ACALC_PORT,
	.number = GPIO_ACALC,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct admt4000_init_param admt4000_ip = {
	.spi_init_param = admt4000_spi_ip,
	.gpio_acalc = gpio_acalc_ip,
	.gpio_busy = gpio_busy_ip,
	.dev_vdd = ADMT4000_3P3V,
};

struct no_os_gpio_init_param spi_sel_b_ip = {
	.port = GPIO_SPI_SEL_PORT,
	.number = GPIO_SPI_SEL_PIN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_spi_init_param spi_sel_b_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.chip_select = GPIO_SPI_SEL_PIN,
	.mode = NO_OS_SPI_MODE_3,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_SEL_EXTRA,
};

#ifdef TMC
const struct no_os_spi_init_param tmc_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.chip_select = TMC_CS_PIN,
	.mode = NO_OS_SPI_MODE_3,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
};

struct tmc5240_init_param tmc5240_ip = {
	.spi_init_param = tmc_spi_ip,
};

/* Check implementation in TMC-EVAL-SYSTEM */
uint8_t reset()
{
	return 1;
}

/* Check implementation in TMC-EVAL-SYSTEM */
uint8_t restore()
{
	return 1;
}
#endif
