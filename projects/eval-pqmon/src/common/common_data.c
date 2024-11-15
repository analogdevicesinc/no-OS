/********************************************************************************
 *   @file   common_data.c
 *   @brief  Defines data common to all examples.
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

struct no_os_uart_init_param iio_demo_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = &max_usb_uart_ops,
}; // UART initialization parameter for iio connection

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
