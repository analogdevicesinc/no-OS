/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines data common to all examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include "common_data.h"
#include "maxim_gpio.h"
#include "maxim_spi.h"
#include "adin1110.h"
#include "lwip_socket.h"
#include "lwip_adin1110.h"

struct max_uart_init_param uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct no_os_uart_init_param uart_ip = {
	.device_id = 0,
	.asynchronous_rx = false,
	.baud_rate = 115200,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &uart_extra_ip,
	.platform_ops = &max_uart_ops,
};

const struct max_spi_init_param maxq1065_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,	
};

const struct no_os_spi_init_param maxq1065_spi_ip = {
	.device_id = 2,
	.max_speed_hz = 10000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = 0,
	.extra = &maxq1065_spi_extra,
};

const struct no_os_gpio_init_param maxq1065_reset_ip = {
	.port = 3,
	.number = 8,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param){
		.vssel = 1
	},
};

// const struct no_os_gpio_init_param maxq1065_rdy_ip = {
// 	.port = 2,
// 	.number = 16,
// 	.pull = NO_OS_PULL_NONE,
// 	.platform_ops = &max_gpio_ops,
// 	.extra = &adin1110_rst_gpio_extra,
// };

const struct maxq1065_init_param maxq1065_ip = {
	.device_id = 0,
	.comm_param = maxq1065_spi_ip,
	.reset_gpio_param = maxq1065_reset_ip,
	.rdy_gpio_param = NULL,
};

const struct no_os_gpio_init_param adin1110_rst_gpio_ip = {
	.port = 0,
	.number = 15,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param){
		.vssel = 1
	},
};

const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = 3,
	.max_speed_hz = 25000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = 0,
	.extra = &maxq1065_spi_extra,
};

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.comm_param = adin1110_spi_ip,
	.reset_param = adin1110_rst_gpio_ip,
	// .mssel_param = adin1110_mssel_ip,
	// .int_param = adin1110_int_gpio_ip,
	.mac_address = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80},
	.append_crc = false,
};

struct lwip_network_param lwip_ip = {
	.platform_ops = &adin1110_lwip_ops,
	.mac_param = &adin1110_ip,
};
