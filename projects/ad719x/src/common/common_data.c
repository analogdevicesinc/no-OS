/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad719x examples.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

const struct no_os_spi_init_param spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 100000,
	.mode = NO_OS_SPI_MODE_3,
	.chip_select = 0U,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = &spi_extra
};

const struct no_os_gpio_init_param miso_ip = {
	.number = RDY_PIN,
	.platform_ops = GPIO_OPS,
	.extra = &gpio_extra
};

const struct no_os_gpio_init_param sync_ip = {
	.number = SYNC_PIN,
	.platform_ops = GPIO_OPS,
	.extra = &gpio_extra
};

struct ad719x_init_param ad7190_dev_ip = {
	.spi_init = &spi_ip,
	.gpio_miso = &miso_ip,
	.sync_pin = NULL,
	.current_polarity = 1,
	.current_gain = AD719X_ADC_GAIN_64,
	.operating_mode = AD719X_MODE_SINGLE,
	.data_rate_code = 480U,
	.clock_source = AD719X_INT_CLK_4_92_MHZ,
	.input_mode = 0,
	.buffer = 0,
	.bpdsw_mode = 0,
	.chip_id = AD7190
};
