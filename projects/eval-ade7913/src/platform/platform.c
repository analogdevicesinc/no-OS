/***************************************************************************//**
 *   @file   platform.c
 *   @brief  Defines platform data to be used by ADE7913 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#include "platform.h"

struct max_uart_init_param max_uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct max_gpio_init_param gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param ade7913_spi_extra_ip  = {
	.num_slaves = SPI_SLAVE_NUM,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param ade7913_gpio_extra_ip = {
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

// user led pin
struct no_os_gpio_init_param gpio_led1_ip = {
	.port = GPIO_LED_PORT,
	.number = GPIO_LED_PIN,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

// data ready pin
struct no_os_gpio_init_param ade7913_gpio_rdy_ip = {
	.port = GPIO_RDY_PORT,
	.number = GPIO_RDY_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

// device 1 spi
struct no_os_spi_init_param ade7913_spi_ip0 = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = &max_spi_ops,
	.chip_select = SPI_CS0,
	.extra = &ade7913_spi_extra_ip,
};

// device 2 spi
struct no_os_spi_init_param ade7913_spi_ip1 = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = &max_spi_ops,
	.chip_select = SPI_CS1,
	.extra = &ade7913_spi_extra_ip,
};

// device 3 spi
struct no_os_spi_init_param ade7913_spi_ip2 = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = &max_spi_ops,
	.chip_select = SPI_CS2,
	.extra = &ade7913_spi_extra_ip,
};

struct no_os_irq_init_param ade7913_gpio_irq_ip = {
	.platform_ops = GPIO_IRQ_OPS,
	.irq_ctrl_id = GPIO_CTRL_IRQ_ID,
	.extra = GPIO_IRQ_EXTRA,
};


/**
 * @brief Init NVIC
 * @param ade7913_nvic_desc - irq nvic descriptor
 * @return 0 in case of success, negative error code otherwise.
 */
int init_nvic(struct no_os_irq_ctrl_desc *ade7913_nvic_desc)
{
	int ret;

	if (!ade7913_nvic_desc)
		return -EINVAL;

	/* Initialize NVIC IRQ controller in order
	to be able to enable GPIO IRQ interrupt */
	struct no_os_irq_init_param ade7913_nvic_ip = {
		.platform_ops = &max_irq_ops,
	};
	/* Initialize GPIO IRQ controller */
	ret = no_os_irq_ctrl_init(&ade7913_nvic_desc,
				  &ade7913_nvic_ip);
	if (ret)
		goto error;
	ret = no_os_irq_set_priority(ade7913_nvic_desc,
				     GPIO2_IRQn, 1);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_enable(ade7913_nvic_desc, GPIO2_IRQn);
	if (ret)
		goto remove_irq;

	return 0;

remove_irq:
	no_os_irq_ctrl_remove(ade7913_nvic_desc);

error:
	pr_err("ERROR\n");
	return ret;
}
