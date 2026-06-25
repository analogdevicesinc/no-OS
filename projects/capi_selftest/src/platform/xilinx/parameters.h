/***************************************************************************//**
 * @file parameters.h
 * @brief Definitions specific to Xilinx platform used by capi_selftest project.
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

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <xparameters.h>
#include "xilinx_capi_gpio.h"
#include "xilinx_capi_spi.h"
#include "capi_uart.h"

extern struct capi_uart_ops capi_uart_xilinx_ps_ops;

#ifdef SDT
#define UART_IDENTIFIER		XPAR_XUARTPS_0_BASEADDR
#else
#define UART_IDENTIFIER		XPAR_XUARTPS_0_DEVICE_ID
#endif

#define UART_OPS		&capi_uart_xilinx_ps_ops
#define UART_BAUDRATE		115200U
#define PLATFORM_NAME		"XILINX"

/*
 * PS GPIO EMIO on ZedBoard JA PMOD:
 *   EMIO 0 = JA1 (input), EMIO 1 = JA2 (output) — wired together
 *   EMIO 2 = JA7 (input), EMIO 3 = JA8 (output) — wired together
 * Use the JA1/JA2 pair: drive JA2 (global pin 55), read back on JA1 (global pin 54).
 */
#define GPIO_OUTPUT_IDENTIFIER		XPAR_XGPIOPS_0_BASEADDR
#define GPIO_OUTPUT_NUM_PINS		1U
#define GPIO_OUTPUT_OPS			&capi_gpio_xilinx_ps_ops
#define GPIO_OUTPUT_EXTRA		struct capi_gpio_xilinx_ps_config
#define GPIO_OUTPUT_EXTRA_INIT		{ .base_pin = 55U }

#define GPIO_INPUT_IDENTIFIER		XPAR_XGPIOPS_0_BASEADDR
#define GPIO_INPUT_NUM_PINS		1U
#define GPIO_INPUT_OPS			&capi_gpio_xilinx_ps_ops
#define GPIO_INPUT_EXTRA		struct capi_gpio_xilinx_ps_config
#define GPIO_INPUT_EXTRA_INIT		{ .base_pin = 54U }

/*
   * PS SPI0 (EMIO) on ZedBoard JC PMOD:
   *   JC1 = SCLK, JC2 = MOSI, JC3 = MISO, JC4 = SS0 (CS0)
   *   Loopback test requires MOSI (JC2) physically wired to MISO (JC3).
   * 3 chip-selects available (CS0/CS1/CS2);
   */
#define SPI_IDENTIFIER		XPAR_XSPIPS_0_BASEADDR
#define SPI_OPS			&capi_spi_xilinx_ps_ops
#define SPI_EXTRA_TYPE		struct capi_spi_xilinx_config
#define SPI_EXTRA_INIT		{ .use_irq = false, .irq_id = 0 }
#define SPI_CLK_FREQ		1000000U

#define SPI_DEVICE_NATIVE_CS	0x01U
#define SPI_DEVICE_MODE		CAPI_SPI_MODE_0
#define SPI_DEVICE_SPEED_HZ	1000000U
#endif /* __PARAMETERS_H__ */
