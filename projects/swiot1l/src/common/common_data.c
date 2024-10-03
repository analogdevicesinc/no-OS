/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by the SWIOT1L firmware.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include "common_data.h"

struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.platform_ops = &max_uart_ops,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = 2,
	.max_speed_hz = 15000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = 0,
	.extra = SPI_EXTRA,
};

const struct no_os_i2c_init_param adt75_comm_param = {
	.device_id = 1,
	.max_speed_hz = 400000,
	.slave_address = ADT75_ADDR(0),
	.platform_ops = &max_i2c_ops,
	.extra = I2C_EXTRA,
};

struct adt75_init_param adt75_ip = {
	.comm_param = adt75_comm_param,
};

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

const struct no_os_gpio_init_param tx_perf_gpio_ip = {
	.port = 2,
	.number = 16,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param ad74413r_reset_ip = {
	.port = 0,
	.number = 26,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param ad74413r_ldac_ip = {
	.port = 0,
	.number = 27,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param ad74413r_irq_ip = {
	.port = 1,
	.number = 22,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param max14906_en_ip = {
	.port = 1,
	.number = 25,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param max14906_d1_ip = {
	.port = 3,
	.number = 7,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param max14906_d2_ip = {
	.port = 3,
	.number = 4,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param max14906_d3_ip = {
	.port = 3,
	.number = 8,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param max14906_d4_ip = {
	.port = 3,
	.number = 5,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param max14906_synch_ip = {
	.port = 3,
	.number = 6,
	.pull = NO_OS_PULL_NONE,
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

const struct no_os_gpio_init_param swiot_led1_ip = {
	.port = 2,
	.number = 15,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param swiot_led2_ip = {
	.port = 2,
	.number = 14,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = GPIO_EXTRA,
};

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.comm_param = adin1110_spi_ip,
	.reset_param = adin1110_rst_gpio_ip,
	.append_crc = false,
};

const struct no_os_platform_spi_delays ad74413r_spi_delays = {
	.cs_delay_first = 1000,
	.cs_delay_last = 0,
};

const struct no_os_spi_init_param ad74413r_spi_ip = {
	.device_id = 3,
	.max_speed_hz = 1000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_2,
	.platform_ops = &max_spi_ops,
	.chip_select = 3,
	.platform_delays = ad74413r_spi_delays,
	.extra = SPI_EXTRA,
};

struct no_os_spi_init_param max14906_spi_ip = {
	.device_id = 1,
	.max_speed_hz = 10000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = 3,
	.extra = SPI_EXTRA,
};

const struct no_os_spi_init_param pmod_spi_ip = {
	.device_id = 1,
	.max_speed_hz = 10000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = 2,
	.extra = SPI_EXTRA,
};

const struct no_os_gpio_init_param psu_gpio_ip = {
	.port = 3,
	.number = 9,
	.platform_ops = &max_gpio_ops,
	.pull = NO_OS_PULL_UP,
	.extra = GPIO_EXTRA
};

struct max149x6_init_param max14906_ip = {
	.chip_address = 0,
	.comm_param = &max14906_spi_ip,
	.crc_en = true,
};

const struct no_os_irq_init_param ad74413r_nvic_ip = {
	.irq_ctrl_id = 0,
	.platform_ops = &max_irq_ops,
	.extra = NULL,
};

struct ad74413r_init_param ad74413r_ip = {
	.chip_id = AD74413R,
	.comm_param = ad74413r_spi_ip,
};

#ifdef SWIOT1L_DEFAULT_FW
/* GPIO trigger */
struct no_os_irq_init_param ad74413r_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID,
	.platform_ops = &max_gpio_irq_ops,
	.extra = GPIO_IRQ_EXTRA,
};

const struct iio_hw_trig_cb_info gpio_cb_info = {
	.event = NO_OS_EVT_GPIO,
	.peripheral = NO_OS_GPIO_IRQ,
	.handle = AD74413R_GPIO_CB_HANDLE,
};

struct swiot_iio_desc_init_param swiot_ip = {
	.psu_gpio_param = psu_gpio_ip,
	.identify_gpio_param = swiot_led2_ip,
	.mode = SWIOT_CONFIG,
};

struct iio_hw_trig_init_param ad74413r_gpio_trig_ip = {
	.irq_id = AD74413R_GPIO_TRIG_IRQ_ID,
	.irq_trig_lvl = NO_OS_IRQ_EDGE_FALLING,
	.cb_info = gpio_cb_info,
	.name = AD74413R_GPIO_TRIG_NAME,
};

struct iio_trigger ad74413r_iio_trig_desc = {
	.is_synchronous = true,
	.enable = iio_trig_enable,
	.disable = iio_trig_disable
};
#endif
