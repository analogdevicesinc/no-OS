/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by rmspow example project
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "maxim_i2c.h"
#include "maxim_gpio.h"
#include "no_os_init.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/


struct max_uart_init_param max_uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct max_i2c_init_param max_i2c_extra_ip = {
	//.vssel = MXC_GPIO_VSSEL_VDDIOH,
	.sda_pinctrl = MAX_GPIO_PINCTRL(2, 7, 1, NO_OS_PULL_NONE, MXC_GPIO_VSSEL_VDDIOH),
	.scl_pinctrl = MAX_GPIO_PINCTRL(2, 8, 1, NO_OS_PULL_NONE, MXC_GPIO_VSSEL_VDDIOH),
};

struct max_gpio_init_param gpio_extra = {
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

//i2c init
struct no_os_i2c_init_param i2c_ip = {
	.device_id = 0,
	.max_speed_hz = 100000,
	.slave_address = 0x9,
	.platform_ops = &max_i2c_ops,
	.extra = &max_i2c_extra_ip,
};

// user led pin
struct no_os_gpio_init_param gpio_led1_ip = {
	.port = GPIO_LED_PORT,
	.number = GPIO_LED_PIN,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

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

	if (val == NO_OS_GPIO_LOW)
		val = NO_OS_GPIO_HIGH;
	else
		val = NO_OS_GPIO_LOW;

	return no_os_gpio_set_value(gpio_led_desc, val);
}


int rmspow_i2c_reg_read(struct no_os_i2c_desc *desc, uint16_t reg, uint32_t *val)
{
	uint8_t bytes_number;
	uint32_t mask = 0;
	uint8_t data[4];
	int ret;

	//data[0] = 0x12;
	data[0] = 0xff;
    data[1] = 0x9f;

	ret = no_os_i2c_write(desc, data, 2, 1);
    printf("ACK ret %d \n", ret);
    if (ret)
		return ret;

	// ret = no_os_i2c_read(desc, data, bytes_number, 1);
	// if (ret)
	// 	return ret;

// 	*val = no_os_get_unaligned_be32(data);

// 	if (mask)
// 		*val = no_os_field_get(*val, mask);

	return 0;
}