/***************************************************************************//**
 *   @file   stm32/stm32_spi.h
 *   @brief  Header file for the stm32 spi driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef STM32_SPI_H_
#define STM32_SPI_H_

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_dma.h"
#include "stm32_hal.h"
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
	/** DMA Initialization Parameters */
	struct no_os_dma_init_param* dma_init;
	/** RX DMA Channel Descriptor */
	struct no_os_dma_ch* rxdma_ch;
	/** TX DMA Channel Descriptor */
	struct no_os_dma_ch* txdma_ch;
#ifdef HAL_TIM_MODULE_ENABLED
	/** CS PWM Initialization Parameters */
	const struct no_os_pwm_init_param *pwm_init;
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
	/** Chip select gpio descriptor */
	struct no_os_gpio_desc *chip_select;
	/** DMA Descriptor */
	struct no_os_dma_desc* dma_desc;
	/** RX DMA Channel Descriptor */
	struct no_os_dma_ch* rxdma_ch;
	/** TX DMA Channel Descriptor */
	struct no_os_dma_ch* txdma_ch;
#ifdef HAL_TIM_MODULE_ENABLED
	/** CS PWM descriptor */
	struct no_os_pwm_desc* pwm_desc;
#endif
};

/**
 * @brief stm32 specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops stm32_spi_ops;

#endif // STM32_SPI_H_
