/*******************************************************************************
 *   @file   riscvrx_irq.h
 *   @brief  Header containing types used in the IRQ driver for RISC-V RX.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef LATTICE_IRQ_H_
#define LATTICE_IRQ_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_irq.h"
#include "hal.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct latt_irq_init_param
 * @brief Structure holding the initialization parameters for Lattice platform
 * specific IRQ parameters.
 */
struct latt_irq_init_param {
	/** Lattice interrupt mode */
	priv_e mode;
};

/**
 * @struct latt_irq_desc
 * @brief Lattice platform specific IRQ descriptor
 */
struct latt_irq_desc {
	/** Lattice interrupt mode */
	priv_e mode;
	/** Lattice IRQ Instance */
	void				*instance;
};

/**
 * @brief Lattice specific IRQ platform ops structure
 */
extern const struct no_os_irq_platform_ops latt_irq_ops;

#endif
