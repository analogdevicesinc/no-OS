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
/*
 * The polled CAPI transmit fills the UART TX FIFO in one shot; a request
 * larger than the FIFO leaves bytes undrained and wedges the console. Send in
 * FIFO-sized chunks so every call completes cleanly.
 */
#define TEST_UART_TX_CHUNK	32U

static int test_uart_write(void *context, const uint8_t *data, uint32_t length)
{
	struct capi_uart_handle *uart = (struct capi_uart_handle *)context;

	for (uint32_t off = 0U; off < length; off += TEST_UART_TX_CHUNK) {
		uint32_t chunk = length - off;

		if (chunk > TEST_UART_TX_CHUNK)
			chunk = TEST_UART_TX_CHUNK;

		int ret = capi_uart_transmit(uart, (uint8_t *)data + off, chunk);
		if (ret != 0)
			return ret;
	}

	return 0;
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

#ifdef UART_EXTRA_TYPE
static UART_EXTRA_TYPE uart_extra = UART_EXTRA_INIT;
#endif

/**
 * @brief CAPI UART configuration used as the test report transport.
 */
const struct capi_uart_config uart_config = {
	.identifier = UART_IDENTIFIER,
	.dma_handle = NULL,
	.clk_freq_hz = 0U,
	.line_config = &uart_line_config,
#ifdef UART_EXTRA_TYPE
	.extra = &uart_extra,
#else
	.extra = NULL,
#endif
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

/**
 * @brief Platform-specific private data for the input GPIO port.
 */
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
 * @brief CAPI IRQ controller configuration used before IRQ-backed async tests.
 */
struct capi_irq_config irq_config = {
	.irq_ctrl_id = IRQ_CTRL_IDENTIFIER,
	.extra = NULL,
};

/**
 * @brief Platform-specific extra data for the SPI controller.
 */
static SPI_EXTRA_TYPE spi_extra = SPI_EXTRA_INIT;

/**
 * @brief CAPI configuration for the SPI controller.
 */
const struct capi_spi_config spi_controller_config = {
	.ops = SPI_OPS,
	.identifier = SPI_IDENTIFIER,
	.dma_handle = NULL,
	.three_pin_mode = false,
	.loopback = false,
	.clk_freq_hz = SPI_CLK_FREQ,
	.extra = &spi_extra,
};

#ifdef TIMER_OPS
/*
 * Timer configuration: uses the same EXTRA_TYPE / EXTRA_INIT pattern as the
 * other peripherals.  The timer_config is consumed by test_timer.c.
 */
#ifdef TIMER_EXTRA_TYPE
static TIMER_EXTRA_TYPE timer_extra = TIMER_EXTRA_INIT;
#endif

/**
 * @brief CAPI timer configuration for the counter/compare/IRQ tests.
 */
const struct capi_timer_config timer_config = {
	.ops = TIMER_OPS,
	.identifier = TIMER_IDENTIFIER,
	.input_clock_identifier = 0U,
	.input_clock_hz = 0U,
	.output_freq_hz = 1000000U,
#ifdef TIMER_EXTRA_TYPE
	.extra = &timer_extra,
#else
	.extra = NULL,
#endif
};
#endif /* TIMER_OPS */

#ifdef DMA_OPS
DMA_XFER_EXTRA_TYPE dma_xfer_extra = DMA_XFER_EXTRA_INIT;

const struct capi_dma_config dma_config = {
	.id = DMA_IDENTIFIER,
	.num_chans = DMA_NUM_CHANS,
	.ops = DMA_OPS,
	.irq_handle = NULL,
	.extra = NULL,
};
#endif /* DMA_OPS */

#ifdef I2C_OPS
static I2C_EXTRA_TYPE i2c_extra = I2C_EXTRA_INIT;

const struct capi_i2c_config i2c_master_config = {
	.identifier = I2C_IDENTIFIER,
	.clk_freq_hz = 0U,
	.initiator = true,
	.address = 0U,
	.device = NULL,
	.dma_handle = NULL,
	.extra = &i2c_extra,
	.ops = I2C_OPS,
};

struct capi_i2c_device i2c_dev = {
	.controller = NULL,
	.address = I2C_TARGET_ADDR,
	.b10addr = false,
	.speed = CAPI_I2C_SPEED_STANDARD,
	.duty_cycle = 0U,
	.clk_stretch = 0,
	.extra = NULL,
};
#endif /* I2C_OPS */

#ifdef I2C_TARGET_OPS
static I2C_TARGET_EXTRA_TYPE i2c_target_extra = I2C_TARGET_EXTRA_INIT;

const struct capi_i2c_config i2c_target_config = {
	.identifier = I2C_TARGET_IDENTIFIER,
	.clk_freq_hz = 0U,
	.initiator = false,
	.address = I2C_TARGET_ADDR,
	.device = NULL,
	.dma_handle = NULL,
	.extra = &i2c_target_extra,
	.ops = I2C_TARGET_OPS,
};

struct capi_i2c_device i2c_target_dev = {
	.controller = NULL,
	.address = I2C_TARGET_ADDR,
	.b10addr = false,
	.speed = CAPI_I2C_SPEED_STANDARD,
	.duty_cycle = 0U,
	.clk_stretch = 0,
	.extra = NULL,
};
#endif /* I2C_TARGET_OPS */

/**
 * @brief CAPI SPI device descriptor for the external loopback test.
 *
 * The controller field is assigned after capi_spi_init().
 */
struct capi_spi_device spi_dev = {
	.controller = NULL,
	.max_speed_hz = SPI_DEVICE_SPEED_HZ,
	.mode = SPI_DEVICE_MODE,
	.native_cs = SPI_DEVICE_NATIVE_CS,
	.cs_gpio = NULL,
	.cs_gpio_num = 0U,
	.flow_ctl_param = {
		.mode = CAPI_SPI_FLOW_CTL_DISABLE,
	},
	.non_continuous_mode = false,
	.lsb_first = false,
	.extra = NULL,
};
