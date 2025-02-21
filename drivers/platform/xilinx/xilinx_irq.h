/*******************************************************************************
 *   @file   xilinx_irq.h
 *   @brief  Header containing types used in the irq driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef XILINX_IRQ_H_
#define XILINX_IRQ_H_

#include <stdint.h>
#include "no_os_irq.h"

/**
 * @enum xil_irq_type
 * @brief Xilinx platform architecture sections
 */
enum xil_irq_type {
	/** Programmable Logic */
	IRQ_PL,
	/** Processing System */
	IRQ_PS
};

/**
 * @struct xil_irq_init_param
 * @brief Structure holding the initialization parameters for Xilinx platform
 * specific IRQ parameters.
 */
struct xil_irq_init_param {
	/** Xilinx architecture */
	enum xil_irq_type	type;
};

/**
 * @struct xil_irq_desc
 * @brief Xilinx platform specific IRQ descriptor
 */
struct xil_irq_desc {
	/** Xilinx architecture */
	enum xil_irq_type	type;
	/** Xilinx IRQ Instance */
	void				*instance;
};

/**
 * @brief Xilinx specific IRQ platform ops structure
 */
extern const struct no_os_irq_platform_ops xil_irq_ops;

#endif
