/********************************************************************************
 *   @file   common_data.c
 *   @brief  Defines data common to all examples.
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#if defined(PQM_CONN_USB)
struct no_os_uart_init_param iio_demo_usb_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = &max_usb_uart_ops,
}; // USB initialization parameter for iio connection
#elif defined(PQM_CONN_SERIAL) || defined(PQM_CONN_T1L)

struct no_os_uart_init_param iio_demo_serial_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = &max_uart_ops,
}; // SERIAL initialization parameter for iio connection

#if defined(PQM_CONN_T1L)

const struct no_os_gpio_init_param adin1110_int_ip = {
	.port = 2,
	.number = 6,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param adin1110_rst_gpio_ip = {
	.port = 2,
	.number = 1,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param adin1110_swpd_ip = {
	.port = 2,
	.number = 25,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param adin1110_tx2p4_ip = {
	.port = 2,
	.number = 10,
	.pull = NO_OS_PULL_DOWN,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param adin1110_mssel_ip = {
	.port = 2,
	.number = 9,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param adin1110_cfg0_ip = {
	.port = 2,
	.number = 3,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param adin1110_cfg1_ip = {
	.port = 2,
	.number = 0,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};
const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = 2,
	.max_speed_hz = 15000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = 0,
	.extra = ADIN_SPI_EXTRA,
};
struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.comm_param = adin1110_spi_ip,
	.reset_param = adin1110_rst_gpio_ip,
	.append_crc = false,
};
struct lwip_network_param lwip_ip = {
	.platform_ops = &adin1110_lwip_ops,
	.mac_param = &adin1110_ip,
};
#endif
#endif

IIO_BUFF_TYPE iio_data_buffer_loc[MAX_SIZE_BASE_ADDR] = {0};

struct pqm_init_para pqm_ip = {
	.ext_buff_len = SAMPLES_PER_CHANNEL_PLATFORM * TOTAL_PQM_CHANNELS,
	.ext_buff = iio_data_buffer_loc,
	.dev_global_attr = {
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		VCONSEL_4W_WYE,
		ADI_PQLIB_NOMINAL_FREQUENCY_50HZ,
		ADI_PQLIB_FLICKER_MODEL_230V_50HZ
	},
	.dev_ch_attr = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	}
}; // pqm init param

struct max_i2c_init_param i2c_extra = {.vssel = MXC_GPIO_VSSEL_VDDIOH}; // I2C

struct no_os_i2c_init_param i2c_ip = {
	.device_id = 1,
	.max_speed_hz = 400000,
	.slave_address = 0x51,
	.platform_ops = &max_i2c_ops,
	.extra = &i2c_extra
}; // I2C Initialization Parameters

struct no_os_uart_init_param uart_ip_stdio = {
	.device_id = 0,
	.asynchronous_rx = false,
	.baud_rate = 115200,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_STDIO_EXT,
	.platform_ops = &max_uart_ops
}; // UART init param

struct no_os_spi_init_param spi_egy_ip = {
	.device_id = SPI_PQM_DEVICE_ID,
	.max_speed_hz = SPI_PQM_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = 0,
	.extra = SPI_PQM_EXTRA,
}; // SPI init param for ade9430

struct no_os_gpio_init_param reset_gpio_ip = {
	.port = RESET_GPIO_PORT_NUM,
	.number = RESET_GPIO_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = RESET_GPIO_EXTRA,
	.pull = NO_OS_PULL_UP
}; // Software reset gpio pin

struct no_os_gpio_init_param intr_gpio_ip = {
	.port = INTR_GPIO_PORT_NUM,
	.number = INTR_GPIO_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = INTR_GPIO_EXTRA,
	.pull = NO_OS_PULL_NONE,
}; // ADE9430 interrupt gpio pin

// Configure IRQ controller
struct no_os_irq_init_param afe_callback_ctrl_ip = {
	.platform_ops = INTR_OPS,
	.irq_ctrl_id = INTR_GPIO_PORT_NUM,
	.extra = NULL,
};

// Configure callback function
struct no_os_callback_desc afe0_callback_desc = {
	.event = NO_OS_EVT_GPIO,
	.peripheral = NO_OS_GPIO_IRQ,
	.ctx = NULL,
	.handle = NULL,
};
