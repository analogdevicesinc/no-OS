/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Linux platform used by eval-ad7191
 *           project.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "linux_spi.h"
#include "linux_gpio.h"
#include "linux_uart.h"

/*
 * The values below describe how an EVAL-AD7191 board is wired to a Raspberry
 * Pi 5. Adjust them to match your own harness before building.
 *
 * SPI: the AD7191 DOUT/RDY line is the SPI MISO. /dev/spidevB.C is selected by
 * SPI_DEVICE_ID (B) and SPI_CS (C). The AD7191 samples on the SCLK falling
 * edge with an idle-high clock, i.e. SPI mode 3.
 *
 * GPIO: every control pin lives on the same gpiochip (GPIO_CHIP -> the RP1
 * bank exposed as /dev/gpiochipN, typically 4 on Raspberry Pi 5). The numbers
 * are BCM offsets on the 40-pin header.
 */
#define UART_DEVICE_ID		"ttyAMA0"
#define UART_BAUDRATE		115200
#define UART_OPS		&linux_uart_ops
#define UART_EXTRA		&ad7191_uart_extra_ip

#define SPI_DEVICE_ID		0
#define SPI_CS			0
#define SPI_MAX_SPEED_HZ	1000000
#define SPI_OPS			&linux_spi_ops

#define GPIO_CHIP		4
#define GPIO_OPS		&linux_gpio_ops

#define GPIO_PGA1_PIN		5
#define GPIO_PGA2_PIN		6
#define GPIO_ODR1_PIN		13
#define GPIO_TEMP_PIN		16
#define GPIO_ODR2_PIN		19
#define GPIO_CLKSEL_PIN		20
#define GPIO_PDOWN_PIN		21
#define GPIO_RDY_PIN		25
#define GPIO_CHAN_PIN		26

/* Reference voltage present on the AD7191 REFIN pins, in millivolts. */
#define AD7191_VREF_MV		5000

extern struct linux_uart_init_param ad7191_uart_extra_ip;

#endif /* __PARAMETERS_H__ */
