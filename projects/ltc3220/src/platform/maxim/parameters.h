/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition header of Maxim platform data used by LTC3220 project
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
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
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_i2c.h"
#include "maxim_gpio.h"

#ifdef DEVICE_VARIANT_1
#define DEV_I2C_ADDRESS     0x1D
#else
#define DEV_I2C_ADDRESS     0x1C
#endif

#if (TARGET_NUM == 32650) || (TARGET_NUM == 78000)
#define I2C_DEVICE_ID    1
#elif (TARGET_NUM == 32655)
#define I2C_DEVICE_ID    2
#elif (TARGET_NUM == 32665) || (TARGET_NUM == 32660) || (TARGET_NUM == 32690)
#define I2C_DEVICE_ID    0
#endif

#define I2C_OPS         &max_i2c_ops

#define GPIO_OPS        &max_gpio_ops

#if (TARGET_NUM == 32690)
#define GPIO_RST_PIN_NUM    14
#define GPIO_RST_PIN_PORT   0
#elif (TARGET_NUM == 32655)
#define GPIO_RST_PIN_NUM    9
#define GPIO_RST_PIN_PORT   1
#endif

extern struct max_gpio_init_param ltc3220_gpio_extra;

extern struct max_i2c_init_param ltc3220_i2c_extra;

#endif /* __PARAMETERS_H__ */
