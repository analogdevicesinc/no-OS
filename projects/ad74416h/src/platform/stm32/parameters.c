/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-ad74416h project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
