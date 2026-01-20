/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADMT4000 examples.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_eeprom.h"

struct no_os_i2c_init_param no_os_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.platform_ops = I2C_OPS,
	.max_speed_hz = 100000,
	.extra = I2C_EXTRA,
};

struct eeprom_24xx32a_init_param eeprom_extra_ip = {
	.i2c_init = &no_os_i2c_ip
};

struct no_os_eeprom_init_param eeprom_ip = {
	.device_id = 0,
	.platform_ops = &eeprom_24xx32a_ops,
	.extra = &eeprom_extra_ip
};

struct no_os_uart_init_param admt4000_uart_ip = {
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

const struct no_os_spi_init_param admt4000_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
};

struct no_os_gpio_init_param gpio_coil_rs_ip = {
	.port = GPIO_COIL_RS_PORT,
	.number = GPIO_COIL_RS,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_PP_EXTRA,
};

struct no_os_gpio_init_param gpio_shdn_n_ip = {
	.port = GPIO_SHDN_N_PORT,
	.number = GPIO_SHDN_N,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_PP_EXTRA,
};

struct no_os_gpio_init_param gpio_v_en_ip = {
	.port = GPIO_V_EN_PORT,
	.number = GPIO_V_EN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param gpio_busy_ip = {
	.port = GPIO_BUSY_PORT,
	.number = GPIO_BUSY,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param gpio_cnv_ip = {
	.port = GPIO_CNV_PORT,
	.number = GPIO_CNV,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param gpio_acalc_ip = {
	.port = GPIO_ACALC_PORT,
	.number = GPIO_ACALC,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param gpio_fault_ip = {
	.port = GPIO_FAULT_PORT,
	.number = GPIO_FAULT,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

const struct no_os_gpio_init_param gpio_rstb_ip = {
	.port = GPIO_RSTB_PORT,
	.number = GPIO_RSTB,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_PP_EXTRA,
};

struct admt4000_init_param admt4000_ip = {
	.spi_init_param = admt4000_spi_ip,
	.gpio_reset_ip = gpio_rstb_ip,
	.admt4000_gpio_ip = {
		{.alt_mode = false, .output_en = false, .init_state = false},  /* GPIO0: BUSY disabled */
		{.alt_mode = false, .output_en = false, .init_state = false},  /* GPIO1: CNV via register */
		{.alt_mode = false, .output_en = false, .init_state = false},  /* GPIO2: Reserved */
		{.alt_mode = true,  .output_en = false, .init_state = false},  /* GPIO3: ACALC enabled */
		{.alt_mode = true,  .output_en = true, .init_state = false},  /* GPIO4: FAULT mode */
		{.alt_mode = false, .output_en = false, .init_state = false},  /* GPIO5: BOOTLOAD disabled */
	},
};

#if(TMC)
const struct no_os_spi_init_param tmc_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.chip_select = TMC_CS_PIN,
	.mode = NO_OS_SPI_MODE_3,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_TMC_EXTRA,
};

struct tmc5240_init_param tmc5240_ip = {
	.spi_init_param = tmc_spi_ip,
	.vstart = 0,
	.v1 = 0,
	.a1 = 0,
	.v2 = 0,
	.a2 = 0,
	.d1 = 0,
	.d2 = 0,
	.vstop = 0,
	.amax = 131072,
	.vmax = 254876,
	.dmax = 131072,
	.clock = 12500000, /* 12.5 MHz internal clock*/
	.step_angle_millidegrees = 900, /* 0.9 degrees */
	.microsteps_res = 0, /* Native 256 microsteps */
	.irundelay = 4, /* 4*512 clocks */
	.iholddelay = 4, /* 4*2^18 clocks */
	.current_run = 16, /* Motor Current = 16/32 */
	.current_hold = 0, /* Stand Still Current = 0/32 */
	.toff = 3, /* 24 + 3*32 clocks */
	.tbl = 2, /* 36 clocks default blank time */
	.global_scaler = 0xFB, /* Default global scaler */
	.slope_control = 0, /* 100V/us */
	.current_range = 3, /*3A */

};
#endif
