/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad469x examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
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

struct no_os_uart_init_param ad4692_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_spi_init_param ad4692_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.extra = SPI_EXTRA,
	.max_speed_hz = SPI_BAUDRATE,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_3,
};

struct no_os_gpio_init_param ad4692_gpio0_ip = {
	.port = GPIO_DREADY_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_DREADY_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ad4692_reset_ip = {
	.port = GPIO_RESET_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_RESET_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_pwm_init_param ad4692_pwm_ip = {
	.id = PWM_ID,
	.polarity = NO_OS_PWM_POLARITY_LOW,
	.platform_ops = PWM_OPS,
	.period_ns = 28000,
	.duty_cycle_ns = 210,
	.extra = PWM_EXTRA,
};

/* GPIO trigger */
struct no_os_irq_init_param ad4692_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_EXTRA,
};

struct ad4692_init_param ad4692_ip = {
	.comm_param = &ad4692_spi_ip,
	.conv_param = &ad4692_pwm_ip,
	.gpio0_param = &ad4692_gpio0_ip,
	.reset_param = &ad4692_reset_ip,
	.id = ID_AD4692,
	.mode = AD4692_MANUAL_MODE,
	.vref = 5000000,
};
