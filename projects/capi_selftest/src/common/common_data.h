/***************************************************************************//**
 * @file common_data.h
 * @brief Common data header file for capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include <stdbool.h>
#include <stdint.h>
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

/*
 * Platform hooks for the GPIO-driven IRQ integration test (test_irq.c). The
 * loopback input pin doubles as an interrupt source: arm() routes that pin to
 * an IRQ line and returns the CAPI IRQ number, ack() clears the pin's latched
 * source from inside the ISR, disarm() masks it again. Implemented per platform
 * in a later commit; arm() returns -ENOTSUP where the board has no GPIO-IRQ
 * path, causing the IRQ test to skip.
 */
/**
 * @brief Route the loopback input pin to an interrupt line.
 * @param irq_line - Out: CAPI IRQ number to connect/enable.
 * @return 0 on success, -ENOTSUP if the board has no GPIO interrupt path,
 *         negative error code otherwise.
 */
int platform_gpio_irq_arm(uint32_t *irq_line);
/**
 * @brief Clear the loopback input pin as the interrupt source.
 * @return true if the input pin was the source and was cleared.
 */
bool platform_gpio_irq_ack(void);
/**
 * @brief Mask the loopback input pin's interrupt again.
 */
void platform_gpio_irq_disarm(void);

/*
 * Interrupts one low->high pulse on the loopback pin raises. A platform whose
 * input is a true single-edge detector leaves this alone; one whose input is
 * change-triggered (interrupting on both halves of the pulse) publishes 2 from
 * its parameters.h. The IRQ suite's exact-count case scales by it.
 */
#ifndef GPIO_IRQ_EVENTS_PER_EDGE
#define GPIO_IRQ_EVENTS_PER_EDGE	1U
#endif /* GPIO_IRQ_EVENTS_PER_EDGE */
#endif /* GPIO_OUTPUT_OPS */

#ifdef SPI_OPS
#include "capi_spi.h"
#include "capi_irq.h"

/**
 * @brief CAPI SPI config for external loopback.
 */
extern const struct capi_spi_config spi_controller_config;
/**
 * @brief CAPI SPI device descriptor for the external loopback test.
 */
extern struct capi_spi_device spi_dev;
#endif /* SPI_OPS */

#ifdef IRQ_CTRL_IDENTIFIER
/**
 * @brief CAPI IRQ controller config used before IRQ-backed async tests.
 */
extern struct capi_irq_config irq_config;
#endif /* IRQ_CTRL_IDENTIFIER */

/**
 * @brief Fill a test framework configuration for the selected platform.
 * @param config - Destination framework configuration.
 * @param uart - UART handle used as the framework write context.
 */
void get_test_framework_config(struct test_framework_config *config,
			       struct capi_uart_handle *uart);

#endif /* __COMMON_DATA_H__ */
