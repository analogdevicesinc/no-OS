/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by MAX20362 examples.
 *   @author Wilbert Jethro R. Limjoco (Wilbertjethro.Limjoco@analog.com)
 *   @author Wilford Abilay (Wilford.Abilay@analog.com)
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
#include "parameters.h"

/** UART initialization parameters for console communication */
struct no_os_uart_init_param uart_ip = {
	.device_id             = 3,
	.asynchronous_rx       = false,
	.baud_rate             = 115200,
	.size                  = NO_OS_UART_CS_8,
	.parity                = NO_OS_UART_PAR_NO,
	.stop                  = NO_OS_UART_STOP_1_BIT,
	.extra                 = &uart_extra_ip,
	.platform_ops          = UART_OPS,
};

/** MAX20362 initialization parameters with default settings */
struct max20362_init_param max20362_ip = {
	.i2c_init = {
		.device_id = 0,
		.max_speed_hz = MAX20362_I2C_CLK_SPEED,
		.slave_address = MAX20362_PMIC_I2C_ADDR,
		.platform_ops = I2C_OPS,
		.extra = (void *)&max20362_i2c_extra,
	},
	.buck_boost_voltage_uv = 3300000,     /* 3.3V default */
	.cap_voltage_uv = 5000000,            /* 5V default */
	.ldo_voltage_uv = 1800000,            /* 1.8V default */
	.input_current_limit_ma = 20,         /* 20mA default */
	.bbat_vdrop = MAX20362_BBAT_VDROP_100MV,
	.buck_boost_enable = false,           /* Start disabled for testing */
	.ldo_enable = false,                  /* Start disabled for testing */
};
