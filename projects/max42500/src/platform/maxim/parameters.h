/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions used by the MAX42500 project.
 *   @author Joshua Maniti (Joshua.Maniti@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/* MAX42500 parameters */
#define MAX42500_I2C_DEVICE_ID  (0)
#define MAX42500_I2C_CLK_SPEED  (400000)
#define MAX42500_I2C_ADDR       (MAX42500_ADDR(0))
#define MAX42500_EN0_PORT       (2)
#define MAX42500_EN0_PIN        (26)
#define MAX42500_EN1_PORT       (2)
#define MAX42500_EN1_PIN        (29)
#define MAX42500_ADDR_PORT      (2)
#define MAX42500_ADDR_PIN       (9)
#define MAX42500_ADDR_SEL       (0)
#define MAX42500_PECE           (NO_OS_BIT(0))
#define MAX42500_VMON_EN        (NO_OS_BIT(MAX42500_VM1))
#define MAX42500_VMON_VMPD      (NO_OS_BIT(7))
#define MAX42500_RESET_MAP      (NO_OS_BIT(MAX42500_VM1) | \
                                 NO_OS_BIT(7))

extern struct max_uart_init_param uart_extra_ip;
extern const struct max_i2c_init_param max42500_i2c_ip;
extern const struct max_gpio_init_param max42500_gpio_extra;

#endif /* __PARAMETERS_H__ */
