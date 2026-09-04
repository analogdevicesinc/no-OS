/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by MAX20362 examples.
 *   @author Wilbert Jethro R. Limjoco (Wilbertjethro.Limjoco@analog.com)
 *   @author Wilford Abilay (Wilford.Abilay@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "parameters.h"

/** UART initialization parameters for console communication */
struct no_os_uart_init_param uart_ip = {
	.device_id             = UART_DEVICE_ID,
	.asynchronous_rx       = UART_ASYNC_RX,
	.baud_rate             = UART_BAUDRATE,
	.size                  = NO_OS_UART_CS_8,
	.parity                = NO_OS_UART_PAR_NO,
	.stop                  = NO_OS_UART_STOP_1_BIT,
	.extra                 = &uart_extra_ip,
	.platform_ops          = UART_OPS,
};

/** MAX20362 initialization parameters with default settings */
struct max20362_init_param max20362_ip = {
	.i2c_init = {
		.device_id = MAX20362_I2C_DEVICE_ID,
		.max_speed_hz = MAX20362_I2C_CLK_SPEED,
		.slave_address = MAX20362_PMIC_I2C_ADDR,
		.platform_ops = I2C_OPS,
		.extra = (void *)&max20362_i2c_extra,
	},
	.buck_boost_voltage_uv = MAX20362_BBOUT_DEFAULT_UV,
	.cap_voltage_uv = MAX20362_VCAP_DEFAULT_UV,
	.ldo_voltage_uv = MAX20362_LDO_DEFAULT_UV,
	.input_current_limit_ma = MAX20362_ILIM_DEFAULT_MA,
	.bbat_vdrop = MAX20362_BBAT_VDROP_100MV,
	.buck_boost_enable = MAX20362_BBOUT_ENABLE_DEFAULT,
	.ldo_enable = MAX20362_LDO_ENABLE_DEFAULT,
};
