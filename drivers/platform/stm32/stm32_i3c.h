/***************************************************************************//**
 *   @file   stm32/stm32_i3c.h
 *   @brief  Header file for the stm32 I3C driver.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef STM32_I3C_H_
#define STM32_I3C_H_

#include <stdint.h>
#include "no_os_i3c.h"
#include "stm32_hal.h"

#define STM32_I3C_TP_PID(x)	(x >> 16)

/**
 * @struct stm32_i3c_dma_desc
 * @brief STM32 I3C DMA descriptor for controlling the
 * I3C DMA transactions.
 */
struct stm32_i3c_dma_desc {
	/** I3C DMA Descriptor */
	struct no_os_dma_desc *dma_desc;
	/** Control DMA Channel Descriptor */
	struct no_os_dma_ch *crdma_ch;
	/** TX DMA Channel Descriptor */
	struct no_os_dma_ch *txdma_ch;
	/** RX DMA Channel Descriptor */
	struct no_os_dma_ch *rxdma_ch;
	/** Status DMA Channel Descriptor */
	struct no_os_dma_ch *srdma_ch;
	/** Private data */
	void *priv;
};

/**
 * @struct stm32_i3c_bus_desc
 * @brief STM32 platform specific I3C bus descriptor.
 */
struct stm32_i3c_bus_desc {
	/** I3C instance */
	I3C_HandleTypeDef *hi3c;
	/** I3C context buffer */
	I3C_XferTypeDef xfer;
	/** I3C CCC info */
	I3C_CCCInfoTypeDef ccc_info;
	/** I3C Interrupt ID */
	uint32_t irq_id;
	/** I3C DMA descriptor */
	struct stm32_i3c_dma_desc *i3c_dma_desc;
};

/**
 * @struct stm32_i3c_device_desc
 * @brief STM32 platform specific I3C device descriptor.
 */
struct stm32_i3c_device_desc {
	/** I3C device configuration */
	I3C_DeviceConfTypeDef dev_conf;
};

/**
 * @struct stm32_i3c_init_param
 * @brief Structure holding the initialization parameters I3C bus.
 */
struct stm32_i3c_init_param {
	/** I3C instance */
	I3C_HandleTypeDef *hi3c;
	/** I3C Interrupt ID */
	uint32_t irq_id;
	/** I3C DMA descriptor */
	struct stm32_i3c_dma_desc *i3c_dma_desc;
};

/**
 * @brief i3c_table contains the pointers towards the I3C buses.
 */
extern struct no_os_i3c_bus_desc *i3c_table [];

/**
 * @brief STM32 specific I3C platform ops structure.
 */
extern const struct no_os_i3c_platform_ops stm32_i3c_ops;

#endif // STM32_I3C_H_
