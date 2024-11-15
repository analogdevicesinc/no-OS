/*******************************************************************************
 *   @file   aducm3029/aducm3029_gpio.h
 *   @brief  Header containing no_os_gpio_platform_ops used by the GPIO driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ADUCM3029_GPIO_H_
#define ADUCM3029_GPIO_H_


/** Get GPIO pin from GPIO number */
#define PIN(nb)		(1u << ((nb) & 0x0F))
/** Get GPIO port from GPIO number */
#define PORT(nb)	(((nb) & 0xF0) >> 4)

/**
 * @brief ADuCM3029 specific GPIO platform ops structure
 */
extern const struct no_os_gpio_platform_ops aducm_gpio_ops;

#endif // ADUCM3029_GPIO_H_
