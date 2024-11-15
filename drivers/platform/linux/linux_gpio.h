/*******************************************************************************
 *   @file   linux/linux_gpio.h
 *   @brief  Header containing no_os_gpio_platform_ops used by the GPIO driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef LINUX_GPIO_H_
#define LINUX_GPIO_H_

/**
 * @brief Linux specific GPIO platform ops structure
 */
extern const struct no_os_gpio_platform_ops linux_gpio_ops;

#endif // LINUX_GPIO_H_
