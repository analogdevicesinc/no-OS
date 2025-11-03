/***************************************************************************//**
 *   @file   platform.c
 *   @brief  Platform configuration for eval-ublox-gnss project.
 *   @author Radu Etz (radu.etz@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "platform.h"
#include "common_data.h"
#include "no_os_gpio.h"
#include "no_os_error.h"

struct max_gpio_init_param max_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_uart_init_param max_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

/* IRQ initialization parameters */
struct no_os_irq_init_param gnss_nvic_ip = {
	.platform_ops = &max_irq_ops
};

/* UART initialization parameters for console output (UART0) */
struct no_os_uart_init_param uart_console_ip = {
	.device_id = CONSOLE_UART_DEVICE_ID,
	.baud_rate = CONSOLE_UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = CONSOLE_UART_PARITY,
	.stop = CONSOLE_UART_STOP,
	.extra = &max_uart_extra_ip,
	.platform_ops = &max_uart_ops
};

/* UART initialization parameters for GNSS communication (UART1) */
struct no_os_uart_init_param uart_gnss_ip = {
	.device_id = GNSS_UART_DEVICE_ID,
	.baud_rate = GNSS_UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = GNSS_UART_PARITY,
	.stop = GNSS_UART_STOP,
	.extra = &max_uart_extra_ip,
	.platform_ops = &max_uart_ops
};

/* GPIO initialization parameters for GNSS reset */
struct no_os_gpio_init_param gpio_gnss_reset_ip = {
	.port = GNSS_RESET_PORT,
	.number = GNSS_RESET_PIN,
	.pull = NO_OS_PULL_NONE,
	.extra = &max_gpio_extra_ip,
	.platform_ops = &max_gpio_ops
};

/* Status LED GPIO parameters */
struct no_os_gpio_init_param gpio_led_ip = {
	.port = GNSS_LED_PORT,
	.number = GNSS_LED_PIN,
	.pull = NO_OS_PULL_NONE,
	.extra = &max_gpio_extra_ip,
	.platform_ops = &max_gpio_ops
};

/* irqn pin */
struct no_os_gpio_init_param gnss_gpio_irq_ip = {
	.port = GPIO_IRQ_PORT,
	.number = GPIO_IRQ_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &max_gpio_extra_ip,
};

struct no_os_irq_init_param gnss_gpio_int_ip = {
	.platform_ops = GPIO_IRQ_OPS,
	.irq_ctrl_id = GPIO_CTRL_IRQ_ID,
	.extra = GPIO_IRQ_EXTRA,
};

/**
 * @brief Toggle LED GPIO
 * @param gpio_desc - GPIO descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int platform_toggle_led(struct no_os_gpio_desc *gpio_desc)
{
	static uint8_t led_state = 0;
	int ret;

	if (!gpio_desc)
		return -EINVAL;

	led_state = !led_state;
	ret = no_os_gpio_set_value(gpio_desc, led_state);
	if (ret)
		return ret;

	return 0;
}
