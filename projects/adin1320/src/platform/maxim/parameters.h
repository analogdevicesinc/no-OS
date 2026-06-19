/***************************************************************************//**
*   @file   parameters.h
*   @brief  Definitions used by the ADIN1320 project.
*   @author John Roed Retuya (Johnroed.Retuya@analog.com)
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

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_gpio.h"
#include "maxim_gpio_irq.h"
#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_uart.h"

/* Platform operation structures (from maxim driver headers) */
#define SPI_OPS		&max_spi_ops
#define GPIO_OPS	&max_gpio_ops
#define IRQ_OPS		&max_irq_ops
#define GPIO_IRQ_OPS	&max_gpio_irq_ops
#define UART_OPS	&max_uart_ops

/* Platform extra initialization parameters (project specific) */
#define UART_EXTRA_IP	&max_uart_extra_ip
#define SPI_EXTRA_IP	&max_spi_extra_ip
#define GPIO_EXTRA_IP	&max_gpio_extra_ip

extern struct max_uart_init_param max_uart_extra_ip;
extern struct max_spi_init_param max_spi_extra_ip;
extern struct max_gpio_init_param max_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */
