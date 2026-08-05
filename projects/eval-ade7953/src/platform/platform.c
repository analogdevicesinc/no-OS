/***************************************************************************//**
 *   @file   platform.c
 *   @brief  Defines platform data to be used by ADE7953 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "platform.h"

struct max_uart_init_param max_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_gpio_init_param gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param ade7953_spi_extra_ip  = {
	.num_slaves = SPI_SLAVE_NUM,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param ade7953_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEV_ID,
	.baud_rate = UART_BAUD,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = &max_uart_ops,
	.extra = &max_uart_extra_ip,
};

/* user led pin */
struct no_os_gpio_init_param gpio_led1_ip = {
	.port = GPIO_LED_PORT,
	.number = GPIO_LED_PIN,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

/* reset pin */
struct no_os_gpio_init_param gpio_reset_ip = {
	.port = GPIO_RESET_PORT,
	.number = GPIO_RESET_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

/* irqn pin */
struct no_os_gpio_init_param ade7953_gpio_irq_ip = {
	.port = GPIO_IRQ_PORT,
	.number = GPIO_IRQ_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

/* zx pin */
struct no_os_gpio_init_param ade7953_gpio_zx_ip = {
	.port = GPIO_ZX_PORT,
	.number = GPIO_ZX_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

struct no_os_spi_init_param ade7953_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = SPI_CS,
	.extra = &ade7953_spi_extra_ip,
};

struct no_os_irq_init_param ade7953_gpio_int_ip = {
	.platform_ops = GPIO_IRQ_OPS,
	.irq_ctrl_id = GPIO_CTRL_IRQ_ID,
	.extra = GPIO_IRQ_EXTRA,
};

/**
 * @brief Toggle Led
 * @param gpio_led_desc - led descriptor
 * @return 0 in case of success, negative error code otherwise.
 */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc)
{
	uint8_t val;
	int ret;

	ret = no_os_gpio_get_value(gpio_led_desc, &val);
	if (ret)
		return ret;

	return no_os_gpio_set_value(gpio_led_desc, !val);
}
