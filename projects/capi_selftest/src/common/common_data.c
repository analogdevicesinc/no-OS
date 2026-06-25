/***************************************************************************//**
 * @file common_data.c
 * @brief Common data source file for capi_selftest project.
 * @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
 ********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <stdint.h>
#include "common_data.h"
#include "parameters.h"
#include "test_framework.h"

/**
 * @brief Write test-framework bytes through the configured CAPI UART.
 * @param context - CAPI UART handle.
 * @param data - Bytes to transmit.
 * @param length - Number of bytes to transmit.
 * @return 0 in case of success, negative error code otherwise.
 */
static int test_uart_write(void *context, const uint8_t *data, uint32_t length)
{
	return capi_uart_transmit((struct capi_uart_handle *)context,
				  (uint8_t *)data, length);
}

/**
 * @brief Default console UART line configuration.
 */
static struct capi_uart_line_config uart_line_config = {
	.baudrate = UART_BAUDRATE,
	.size = CAPI_UART_DATA_BITS_8,
	.parity = CAPI_UART_PARITY_NONE,
	.stop_bits = CAPI_UART_STOP_1_BIT,
	.flow_control = CAPI_UART_FLOW_CONTROL_NONE,
	.address_mode = CAPI_UART_ADDRESS_MODE_DISABLED,
	.device_address = 0U,
	.sticky_parity = false,
	.loopback = false,
};

/**
 * @brief CAPI UART configuration used as the test report transport.
 */
const struct capi_uart_config uart_config = {
	.identifier = UART_IDENTIFIER,
	.dma_handle = NULL,
	.clk_freq_hz = 0U,
	.line_config = &uart_line_config,
	.extra = NULL,
	.ops = UART_OPS,
};

/**
 * @brief Short platform label printed by TEST_RUN_START().
 */
const char platform_name[] = PLATFORM_NAME;

void get_test_framework_config(struct test_framework_config *config,
			       struct capi_uart_handle *uart)
{
	*config = (struct test_framework_config) {
		.run_label = platform_name,
		.write = test_uart_write,
		.write_context = uart,
	};
}

/**
 * @brief Platform-specific private data for the output/readback GPIO port.
 */
static GPIO_OUTPUT_EXTRA gpio_output_extra = GPIO_OUTPUT_EXTRA_INIT;
static GPIO_INPUT_EXTRA gpio_input_extra = GPIO_INPUT_EXTRA_INIT;


/**
 * @brief CAPI configuration for the output/readback GPIO port.
 */
const struct capi_gpio_port_config gpio_output_config = {
	.ops = GPIO_OUTPUT_OPS,
	.identifier = GPIO_OUTPUT_IDENTIFIER,
	.num_pins = GPIO_OUTPUT_NUM_PINS,
	.flags = NULL,
	.extra = &gpio_output_extra,
};

/**
 * @brief CAPI configuration for the input GPIO port.
 */
const struct capi_gpio_port_config gpio_input_config = {
	.ops = GPIO_INPUT_OPS,
	.identifier = GPIO_INPUT_IDENTIFIER,
	.num_pins = GPIO_INPUT_NUM_PINS,
	.flags = NULL,
	.extra = &gpio_input_extra,
};

/**
 * @brief Platform-specific extra data for the SPI peripheral
 */
static SPI_EXTRA_TYPE spi_extra = SPI_EXTRA_INIT;

/**
 * @brief CAPI configuration for the SPI controller.
 */
const struct capi_spi_config spi_controller_config = {
	.ops = SPI_OPS,
	.identifier = SPI_IDENTIFIER,
	.three_pin_mode = false,
	.clk_freq_hz = SPI_CLK_FREQ,
	.loopback = false,
	.dma_handle = NULL,
	.extra = &spi_extra,
};

/**
 * @brief CAPI SPI device descriptor for the external loopback test.
 *
 * Platform fields are set here; the .controller field must be assigned
 * after capi_spi_init() before use.
 */
struct capi_spi_device spi_dev = {
	.native_cs    = SPI_DEVICE_NATIVE_CS,
	.mode         = SPI_DEVICE_MODE,
	.max_speed_hz = SPI_DEVICE_SPEED_HZ,
};

