/***************************************************************************//**
 * @file common_data.h
 * @brief Common data header file for capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "capi_uart.h"
#include "parameters.h"

struct test_framework_config;

/**
 * @brief Console UART configuration used by the test framework output path.
 */
extern const struct capi_uart_config uart_config;
/**
 * @brief Short platform label printed in the test run header.
 */
extern const char platform_name[];

#ifdef GPIO_OUTPUT_OPS
#include "capi_gpio.h"

/* If it's not declared, assume the port loopback test is on by default. */
#ifndef GPIO_HAS_PORT_LOOPBACK
#define GPIO_HAS_PORT_LOOPBACK 1
#endif /* GPIO_HAS_PORT_LOOPBACK */

/* If it's not declared, assume the pin loopback test is off by default. */
#ifndef GPIO_HAS_PIN_LOOPBACK
#define GPIO_HAS_PIN_LOOPBACK 0
#endif /* GPIO_HAS_PIN_LOOPBACK */

/* Toggle is an optional CAPI op; assume supported unless a platform opts out. */
#ifndef GPIO_HAS_TOGGLE
#define GPIO_HAS_TOGGLE 1
#endif /* GPIO_HAS_TOGGLE */

/**
 * @brief CAPI GPIO config for the output/readback test port.
 */
extern const struct capi_gpio_port_config gpio_output_config;
/**
 * @brief CAPI GPIO config for the input sampling test port.
 */
extern const struct capi_gpio_port_config gpio_input_config;

#if GPIO_HAS_PIN_LOOPBACK
/**
 * @brief Pin number mappings for loopback testing.
 */
extern const uint32_t gpio_output_pin_numbers[];
extern const uint32_t gpio_input_pin_numbers[];
extern const uint32_t gpio_num_output_pins;
extern const uint32_t gpio_num_input_pins;
#endif /* GPIO_HAS_PIN_LOOPBACK */
#endif /* GPIO_OUTPUT_OPS */

/**
 * @brief Fill a test framework configuration for the selected platform.
 * @param config - Destination framework configuration.
 * @param uart - UART handle used as the framework write context.
 */
void get_test_framework_config(struct test_framework_config *config,
			       struct capi_uart_handle *uart);

#endif /* __COMMON_DATA_H__ */
