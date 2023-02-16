/***************************************************************************//**
 *   @file   stm32/stm32_tdm.h
 *   @brief  Header file for the stm32 tdm driver.
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
