/***************************************************************************//**
 * @file i2c_platform_init.c
 * @brief STM32 I2C2 platform setup for single-board loopback tests.
 *
 * CubeMX only generates MSP code for I2C1. This file configures I2C2
 * (clock source, GPIO, NVIC) so that the CAPI driver can initialise I2C2
 * as a target device. It also re-configures I2C1 pins with internal
 * pull-ups required for the loopback wiring.
 *
 * The I2C2 IRQ handlers dispatch through the CAPI isr function. The test
 * must call i2c_platform_set_target_handle() after capi_i2c_init() so that
 * the IRQ vectors can reach the correct controller handle.
 *
 * Wiring: PB10 (I2C2_SCL) <-> PB6 (I2C1_SCL)
 *         PB11 (I2C2_SDA) <-> PB9 (I2C1_SDA)
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stddef.h>
#include "stm32_hal.h"
#include "capi_i2c.h"

static struct capi_i2c_controller_handle *i2c2_handle;

int i2c_platform_init(void)
{
	RCC_PeriphCLKInitTypeDef clk_init = { 0 };
	GPIO_InitTypeDef gpio_init;

	clk_init.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
	clk_init.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&clk_init) != HAL_OK)
		return -1;

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C2_CLK_ENABLE();

	gpio_init.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	gpio_init.Mode = GPIO_MODE_AF_OD;
	gpio_init.Pull = GPIO_PULLUP;
	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_init.Alternate = GPIO_AF4_I2C2;
	HAL_GPIO_Init(GPIOB, &gpio_init);

	gpio_init.Pin = GPIO_PIN_6 | GPIO_PIN_9;
	gpio_init.Mode = GPIO_MODE_AF_OD;
	gpio_init.Pull = GPIO_PULLUP;
	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_init.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &gpio_init);

	HAL_NVIC_SetPriority(I2C2_EV_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
	HAL_NVIC_SetPriority(I2C2_ER_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);

	i2c2_handle = NULL;

	return 0;
}

void i2c_platform_deinit(void)
{
	HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
	HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);
	i2c2_handle = NULL;
}

void i2c_platform_set_target_handle(struct capi_i2c_controller_handle *handle)
{
	i2c2_handle = handle;
}

void I2C2_EV_IRQHandler(void)
{
	if (i2c2_handle)
		capi_i2c_isr(i2c2_handle);
}

void I2C2_ER_IRQHandler(void)
{
	if (i2c2_handle)
		capi_i2c_isr(i2c2_handle);
}
