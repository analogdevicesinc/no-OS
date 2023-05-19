/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adxl355-pmdz examples.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct no_os_uart_init_param adxl355_uart_ip = {
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

#ifdef IIO_TRIGGER_EXAMPLE
/* GPIO trigger */
struct no_os_irq_init_param adxl355_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

struct iio_hw_trig_init_param adxl355_gpio_trig_ip = {
	.irq_id = ADXL355_GPIO_TRIG_IRQ_ID,
	.irq_trig_lvl = NO_OS_IRQ_EDGE_RISING,
	.cb_info = {
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = ADXL355_GPIO_CB_HANDLE,
	},
	.name = ADXL355_GPIO_TRIG_NAME,
};
#endif

struct no_os_spi_init_param adxl355_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

struct adxl355_init_param adxl355_ip = {
	.comm_type = ADXL355_SPI_COMM,
#ifdef ADXL359_DEV
	.dev_type = ID_ADXL359,
#elif defined ADXL357_DEV
	.dev_type = ID_ADXL357,
#else
	.dev_type = ID_ADXL355,
#endif
};

#ifdef IIO_LWIP_EXAMPLE
uint8_t adin1110_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80};

const struct no_os_gpio_init_param adin1110_reset_ip = {
	.port = ADIN1110_GPIO_RESET_PORT,
	.number = ADIN1110_GPIO_RESET_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = &adin1110_reset_gpio_extra_ip,
};

const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = ADIN1110_SPI_DEVICE_ID,
	.max_speed_hz = ADIN1110_SPI_CLK_RATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = ADIN1110_SPI_CS,
	.extra = &adin1110_spi_extra_ip,
};

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.comm_param = adin1110_spi_ip,
	.reset_param = adin1110_reset_ip,
	.append_crc = false,
};
#endif
