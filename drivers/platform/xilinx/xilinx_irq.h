/*******************************************************************************
 *   @file   xilinx_irq.h
 *   @brief  Header containing types used in the irq driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef XILINX_IRQ_H_
#define XILINX_IRQ_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_irq.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

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
