/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad405x examples.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "common_data.h"
#include "ad405x.h"

#ifdef AD405X_I3C
#define LENGTH_I3C_DEVS sizeof(i3c1_devs_param)/sizeof(void *)
#endif

struct no_os_uart_init_param ad405x_uart_ip = {
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

const struct no_os_i3c_init_param eeprom_i2c_ip = {
	.is_i3c = EEPROM_IS_I2C,
	.addr = EEPROM_STATIC_ADDR,
};

#ifdef AD405X_I3C
const struct no_os_i3c_init_param *i3c1_devs_param[] = {
	&ad405x_i3c_ip, &eeprom_i2c_ip
};

struct no_os_i3c_bus_init_param i3c1_ip = {
	.device_id = I3C_DEVICE_ID,
	.platform_ops = I3C_OPS,
	.devs = i3c1_devs_param,
	.num_devs = LENGTH_I3C_DEVS,
	.extra = &ad405x_i3c_extra_ip
};

struct no_os_i3c_init_param ad405x_i3c_ip = {
	.bus = &i3c1_ip,
	.pid = AD405X_I3C_PID(AD405X_INSTANCE_ID),
	.is_i3c = AD405X_IS_I3C,
	.addr = AD405X_DYN_ADDR,
	.is_static = AD405X_NO_STATIC_ADDR,
};
#endif

#ifdef AD405X_SPI
struct no_os_spi_init_param ad405x_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 17000000,
	.mode = NO_OS_SPI_MODE_0,
	.chip_select = GPIO_CS_PIN,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = &ad405x_spi_extra_ip
};

struct no_os_gpio_init_param gpio_cnv_param = {
	.port = GPIO_CNV_PORT,
	.number = GPIO_CNV_PIN,
	.platform_ops = GPIO_OPS,
	.extra = &gpio_init
};
#endif

struct ad405x_init_param ad405x_ip = {
	.comm_type = AD405X_COMM,
#ifdef AD405X_I3C
	.comm_init.i3c_init = &ad405x_i3c_ip,
#endif
#ifdef AD405X_SPI
	.comm_init.spi_init = &ad405x_spi_ip,
#endif
	.dev_type = AD405X_DEV_TYPE,
	.gpio_cnv = AD405X_GPIO_CNV,
	.gpio_gpio0 = &gpio_gpio0_param,
	.gpio_gpio1 = &gpio_gpio1_param
};

struct no_os_gpio_init_param gpio_gpio0_param = {
	.port = GPIO_GPIO0_PORT,
	.number = GPIO_GPIO0_PIN,
	.platform_ops = GPIO_OPS,
	.extra = &gpio_init
};

struct no_os_gpio_init_param gpio_gpio1_param = {
	.port = GPIO_GPIO1_PORT,
	.number = GPIO_GPIO1_PIN,
	.platform_ops = GPIO_OPS,
	.extra = &gpio_init
};
