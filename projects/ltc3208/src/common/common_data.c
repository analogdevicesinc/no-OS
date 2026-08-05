/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by LTC3208 examples.
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

const struct no_os_i2c_init_param iip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 400000,
	.slave_address = DEV_I2C_ADDRESS,
	.platform_ops = I2C_OPS,
	.extra = &ltc3208_i2c_extra,
};

const struct no_os_gpio_init_param gip = {
	.port = GPIO_ENRGBS_PIN_PORT,
	.number = GPIO_ENRGBS_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = &ltc3208_gpio_extra,
};

struct ltc3208_init_param ltc3208_user_init = {
	.i2c_init_param = iip,
	.gpio_init_param = gip
};
