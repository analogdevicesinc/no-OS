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

/**
 * @brief CAPI GPIO config for the output/readback test port.
 */
extern const struct capi_gpio_port_config gpio_output_config;
/**
 * @brief CAPI GPIO config for the input sampling test port.
 */
extern const struct capi_gpio_port_config gpio_input_config;
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
/**
 * @brief CAPI IRQ controller config used before IRQ-backed async tests.
 */
extern struct capi_irq_config irq_config;
#endif /* SPI_OPS */

#ifdef TIMER_OPS
#include "capi_timer.h"

/**
 * @brief CAPI timer config for the counter/compare/IRQ tests.
 */
extern const struct capi_timer_config timer_config;
#endif /* TIMER_OPS */

#ifdef I2C_OPS
#include "capi_i2c.h"

/**
 * @brief CAPI I2C initiator configuration for the loopback tests.
 */
extern const struct capi_i2c_config i2c_master_config;
/**
 * @brief CAPI I2C device descriptor used by the initiator to address the target.
 */
extern struct capi_i2c_device i2c_dev;
#endif /* I2C_OPS */

#ifdef I2C_TARGET_OPS
/**
 * @brief CAPI I2C target configuration for the loopback tests.
 */
extern const struct capi_i2c_config i2c_target_config;
/**
 * @brief CAPI I2C device descriptor for the target side.
 */
extern struct capi_i2c_device i2c_target_dev;
#endif /* I2C_TARGET_OPS */

/**
 * @brief Fill a test framework configuration for the selected platform.
 * @param config - Destination framework configuration.
 * @param uart - UART handle used as the framework write context.
 */
void get_test_framework_config(struct test_framework_config *config,
			       struct capi_uart_handle *uart);

#endif /* __COMMON_DATA_H__ */
