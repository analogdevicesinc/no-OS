/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition header of Maxim platform data used by LTC3208 project
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_i2c.h"
#include "maxim_gpio.h"

#define DEV_I2C_ADDRESS     0x1B

#if (TARGET_NUM == 32655)
#define I2C_DEVICE_ID    2
#elif (TARGET_NUM == 32690)
#define I2C_DEVICE_ID    0
#endif

#define I2C_OPS         &max_i2c_ops

#define GPIO_OPS        &max_gpio_ops

#if (TARGET_NUM == 32690)
#define GPIO_ENRGBS_PIN_NUM    14
#define GPIO_ENRGBS_PIN_PORT   0
#elif (TARGET_NUM == 32655)
#define GPIO_ENRGBS_PIN_NUM    9
#define GPIO_ENRGBS_PIN_PORT   1
#endif

extern struct max_gpio_init_param ltc3208_gpio_extra;
extern struct max_i2c_init_param ltc3208_i2c_extra;

#endif /* __PARAMETERS_H__ */
