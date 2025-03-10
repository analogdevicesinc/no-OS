/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Mbed platform data used by ad74416h project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include "parameters.h"

struct mbed_uart_init_param ad74416h_uart_extra_ip = {
	.uart_tx_pin = UART_TX_PIN,
	.uart_rx_pin = UART_RX_PIN
};

struct mbed_i2c_init_param ad74416h_i2c_extra = {
	.i2c_sda_pin = I2C_SDA,
	.i2c_scl_pin = I2C_SCL
};

extern struct mbed_spi_init_param ad74416h_spi_extra = {
	.spi_miso_pin = ARDUINO_UNO_D12,
	.spi_mosi_pin = ARDUINO_UNO_D11,
	.spi_clk_pin = ARDUINO_UNO_D13,
	.use_sw_csb = false
};

/* ADC_RDY GPIO parameters*/
struct no_os_gpio_init_param adc_rdy_gpio_ip = {
	.port = 0,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_ADC_RDY,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

/* ADC_RDY GPIO IRQ parameters */
struct no_os_irq_init_param adc_rdy_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ADC_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_ADC_EXTRA
};

struct mbed_gpio_init_param ad74416h_gpio_extra = {
	.pin_mode = 0 //NA
};

/*ADC_RDY interrupt Mbed platform specific parameters */
struct mbed_gpio_irq_init_param mbed_adc_rdy_gpio_irq_extra = {
	.gpio_irq_pin = ARDUINO_UNO_D2,
};
