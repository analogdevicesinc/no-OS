/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Mbed platform data used by admt4000 project.
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 * 		     Louijie Compo  (louijie.compo@analog.com)
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

#include "parameters.h"
#include "admt4000.h"

struct mbed_uart_init_param uart_extra_ip = {
	.uart_tx_pin = UART_TX_PIN,
	.uart_rx_pin = UART_RX_PIN
};

struct no_os_uart_init_param uart_ip = {
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

struct mbed_spi_init_param spi_extra_ip = {
	.spi_miso_pin = SPI_MISO,
	.spi_mosi_pin = SPI_MOSI,
	.spi_clk_pin = SPI_SCK,
	.use_sw_csb = false,
};

struct mbed_spi_init_param spi_sel_b_extra_ip = {
	.spi_miso_pin = SPI_MISO,
	.spi_mosi_pin = SPI_MOSI,
	.spi_clk_pin = SPI_SCK,
	.use_sw_csb = true,
};

struct mbed_gpio_init_param gpio_extra_ip = {
	.pin_mode = 1,
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

const struct no_os_spi_init_param admt_spi_ip = {
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

const struct no_os_gpio_init_param gpio_gpio0_busy_ip = {
	.port = GPIO_SPI_SEL_PORT,
	.number = GPIO_GPIO0_BUSY,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param gpio_shdn_n_ip = {
	.port = GPIO_SPI_SEL_PORT,
	.number = GPIO_SHDN_N,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct admt4000_init_param admt_ip = {
	.spi_init = admt_spi_ip,
	.gpio_coil_rs = gpio_coil_rs_ip,
	.gpio_gpio0_busy = gpio_gpio0_busy_ip,
	.gpio_shdn_n = gpio_shdn_n_ip,
	.dev_vdd = ADMT4000_3P3V,
};
