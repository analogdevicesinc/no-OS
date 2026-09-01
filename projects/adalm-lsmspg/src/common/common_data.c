/***************************************************************************//**
 *   @file   common_data.c
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#include "common_data.h"
#include "parameters.h"

struct no_os_spi_init_param ad5592r_spi_ip = {
	.device_id = AD5592R_SPI_DEVICE_ID,
	.max_speed_hz = AD5592R_SPI_SPEED,
	.chip_select = AD5592R_SPI_CS,
	.mode = AD5592R_SPI_MODE,
	.bit_order = AD5592R_SPI_BIT_ORDER,
	.lanes = AD5592R_SPI_LANES,
	.platform_ops = AD5592R_SPI_OPS,
	.extra = AD5592R_SPI_EXTRA,
};

struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = UART_ASYNC_RX,
	.baud_rate = UART_BAUD_RATE,
	.size = UART_SIZE,
	.parity	= UART_PARITY,
	.stop = UART_STOP_BITS,
	.platform_ops = UART_PLATFORM_OPS,
	.extra = UART_EXTRA,
};

struct no_os_irq_init_param timer_irq_ip = {
	.irq_ctrl_id = TIMER_IRQ_ID,
	.platform_ops = TIMER_IRQ_OPS,
};

struct no_os_timer_init_param timer_ip = {
	.id = TIMER_ID,
	.freq_hz = TIMER_FREQ_HZ,
	.ticks_count = TIMER_TICKS_COUNT,
	.platform_ops = TIMER_OPS,
};

struct no_os_gpio_init_param led_gpio_ip = {
	.port = LED_GPIO_PORT,
	.number = LED_GPIO_NUMBER,
	.pull = LED_GPIO_PULL,
	.platform_ops = LED_GPIO_OPS,
	.extra = LED_GPIO_EXTRA,
};

struct no_os_i2c_init_param ad5593r_i2c_ip = {
	.device_id = AD5593R_I2C_DEVICE_ID,
	.max_speed_hz = AD5593R_I2C_SPEED_HZ,
	.slave_address = AD5593R_I2C_SLAVE_ADDR,
	.platform_ops = AD5593R_I2C_OPS,
	.extra = AD5593R_I2C_EXTRA,
};

struct no_os_i2c_init_param lm75_i2c_ip = {
	.device_id = LM75_I2C_DEVICE_ID,
	.max_speed_hz = LM75_I2C_SPEED_HZ,
	.slave_address = LM75_I2C_SLAVE_ADDR,
	.platform_ops = LM75_I2C_OPS,
	.extra = LM75_I2C_EXTRA,
};
