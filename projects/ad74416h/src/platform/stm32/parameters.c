/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-ad74416h project.
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

struct stm32_uart_init_param ad74416h_uart_extra_ip = {
	.huart = &huart5,
};

struct stm32_spi_init_param ad74416h_spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
};

/* ADC_RDY interrupt STM32 platform specific parameters */
struct stm32_gpio_irq_init_param ad74416h_gpio_irq_extra_ip = {
	.port_nb = GPIO_ADC_RDY_PORT, /* Port A */
};

/* ADC_RDY GPIO parameters */
struct no_os_gpio_init_param adc_rdy_gpio_ip = {
	.port = GPIO_ADC_RDY_PORT,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_ADC_RDY_PIN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

/* ADC_RDY GPIO IRQ parameters */
struct no_os_irq_init_param adc_rdy_gpio_irq_ip = {
	.irq_ctrl_id = GPIO_IRQ_ID1,
	.platform_ops = GPIO_IRQ_OPS,
	.extra = GPIO_IRQ_ADC_EXTRA,
};
