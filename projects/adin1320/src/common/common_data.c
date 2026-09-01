/***************************************************************************//**
*   @file   common_data.c
*   @brief  Defines common data to be used by ADIN1320 examples.
*   @author John Roed Retuya (Johnroed.Retuya@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEV_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA_IP
};

struct no_os_spi_init_param spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA_IP
};

struct no_os_gpio_init_param gpio_uc_cfg0_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = CFG0_NUMBER,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_uc_cfg1_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = CFG1_NUMBER,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_uc_cfg2_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = CFG2_NUMBER,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_uc_cfg3_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = CFG3_NUMBER,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_led_green_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = LED3_NUMBER,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_led_red_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = LED2_NUMBER,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_led_yellow_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = LED1_NUMBER,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_led_blue_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = LED0_NUMBER,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_adin1320_0_reset_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = ADIN1320_0_RESET_BUTTON,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_adin1320_8_reset_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = ADIN1320_8_RESET_BUTTON,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_adin1320_0_int_n_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = ADIN1320_0_INT_N_PIN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_gpio_init_param gpio_adin1320_8_int_n_ip = {
	.port = PORT_NUMBER,
	.pull = NO_OS_PULL_NONE,
	.number = ADIN1320_8_INT_N_PIN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA_IP
};

struct no_os_irq_init_param irq_nvic_ip = {
	.platform_ops = IRQ_OPS
};

struct no_os_irq_init_param irq_int_n_ip = {
	.irq_ctrl_id = PORT_NUMBER,
	.platform_ops = GPIO_IRQ_OPS
};
