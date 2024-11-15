/***************************************************************************//**
 *   @file   stm32/stm32_tdm.h
 *   @brief  Header file for the stm32 tdm driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STM32_TDM_H_
#define STM32_TDM_H_

#include <stdint.h>
#include "no_os_tdm.h"
#include "stm32_hal.h"
#include "no_os_irq.h"

/**
 * @struct stm32_tdm_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 * specific TDM parameters.
 */
struct stm32_tdm_init_param {
	/** Device ID */
	SAI_Block_TypeDef *base;
	/** Interrupt controller descriptor */
	//struct no_os_irq_ctrl_desc *nvic;
};

/**
 * @struct stm32_tdm_desc
 * @brief stm32 platform specific TDM descriptor
 */
struct stm32_tdm_desc {
	/** TDM instance */
	SAI_HandleTypeDef hsai;
	/** Interrupt controller descriptor - Rx Half complete */
	struct no_os_irq_ctrl_desc *nvic_rx_halfcplt;
	/** Interrupt controller descriptor -Rx complete */
	struct no_os_irq_ctrl_desc *nvic_rxcplt;
	/** Rx half complete callback */
	struct no_os_callback_desc rx_half_callback;
	/** Rx complete callback */
	struct no_os_callback_desc rx_callback;
};

/**
 * @brief stm32 specific TDM platform ops structure
 */
extern const struct no_os_tdm_platform_ops stm32_tdm_platform_ops;


/* Initialize the TDM communication peripheral. */
int32_t stm32_tdm_init(struct no_os_tdm_desc **desc,
		       const struct no_os_tdm_init_param *param);

/* Free the resources allocated by stm32_tdm_init(). */
int32_t stm32_tdm_remove(struct no_os_tdm_desc *desc);

/* Write and read data to/from TDM. */
int32_t stm32_tdm_read(struct no_os_tdm_desc *desc, void *data,
		       uint16_t bytes_number);

/* Stop TDM DMA Data transfer */
int32_t stm32_stop_tdm_transfer(struct no_os_tdm_desc *desc);

/* Pause TDM DMA Data transfer */
int32_t stm32_pause_tdm_transfer(struct no_os_tdm_desc *desc);

/* Resume TDM DMA Data transfer */
int32_t stm32_resume_tdm_transfer(struct no_os_tdm_desc *desc);


#endif // STM32_TDM_H_
