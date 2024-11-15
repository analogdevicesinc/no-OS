/***************************************************************************//**
 *   @file   stm32/stm32_spi.h
 *   @brief  Header file for the stm32 spi driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STM32_SPI_H_
#define STM32_SPI_H_

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_dma.h"
#include "no_os_gpio.h"
#include "stm32_hal.h"
#include "stm32_gpio.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "stm32_pwm.h"
#endif

/**
 * @struct stm32_spi_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 * specific SPI parameters.
 */
struct stm32_spi_init_param {
	/** Chip select port */
	uint32_t chip_select_port;
	/** Get perihperal source clock function */
	uint32_t (*get_input_clock)(void);
	/** Chip select alternate function */
	uint32_t alternate;
	/** DMA Initialization Parameters */
	struct no_os_dma_init_param* dma_init;
	/** RX DMA Channel Descriptor */
	struct stm32_dma_channel* rxdma_ch;
	/** TX DMA Channel Descriptor */
	struct stm32_dma_channel* txdma_ch;
	uint32_t irq_num;
#ifdef HAL_TIM_MODULE_ENABLED
	/** CS PWM Initialization Parameters */
	const struct no_os_pwm_init_param *pwm_init;
	const struct no_os_pwm_init_param *tx_pwm_init;
#endif
};

/**
 * @struct stm32_spi_desc
 * @brief stm32 platform specific SPI descriptor
 */
struct stm32_spi_desc {
	/** SPI instance */
	SPI_HandleTypeDef hspi;
	/** SPI input clock */
	uint32_t input_clock;
	/** Chip select alternate function */
	uint32_t alternate;
	/** Chip select gpio descriptor */
	struct no_os_gpio_desc *chip_select;
	/** DMA Descriptor */
	struct no_os_dma_desc* dma_desc;
	/** RX DMA Channel Descriptor */
	struct no_os_dma_ch* rxdma_ch;
	/** TX DMA Channel Descriptor */
	struct no_os_dma_ch* txdma_ch;
	struct stm32_gpio_init_param csip_extra;
	struct no_os_gpio_init_param csip;
	bool stm32_spi_dma_done;
	void (*stm32_spi_dma_user_cb)(void *ctx);
	void *stm32_spi_dma_user_ctx;
	struct stm32_dma_channel* rx_ch_xfer;
	struct stm32_dma_channel* tx_ch_xfer;
#ifdef HAL_TIM_MODULE_ENABLED
	/** CS PWM descriptor */
	struct no_os_pwm_desc* pwm_desc;
	struct no_os_pwm_desc* tx_pwm_desc;
#endif
};

/**
 * @brief stm32 specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops stm32_spi_ops;

#endif // STM32_SPI_H_
