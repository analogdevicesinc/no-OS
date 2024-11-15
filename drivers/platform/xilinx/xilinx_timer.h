/***************************************************************************//**
*   @file   xilinx_timer.h
*   @brief  Timer control module header.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef XILINX_TIMER_H_
#define XILINX_TIMER_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <xparameters.h>
#ifdef XPAR_XSCUTIMER_NUM_INSTANCES
#include <xscutimer.h>
#endif
#ifdef XPAR_XTMRCTR_NUM_INSTANCES
#include <xtmrctr.h>
#endif

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum xil_timer_type
 * @brief Xilinx platform architecture sections
 */
enum xil_timer_type {
	/** Programmable Logic */
	TIMER_PL,
	/** Processing System */
	TIMER_PS
};

/**
 * @struct xil_timer_desc
 * @brief Xilinx specific timer descriptor
 */
struct xil_timer_desc {
	/** Pointer to the BSP driver handler */
	void *instance;
	/** Pointer to the BSP driver configuration handler */
	void *config;
	/** Timer used from the PL 3-timer core */
	uint8_t active_tmr;
	/** Platform selection parameter */
	enum xil_timer_type type;
};

/**
 * @struct xil_timer_init_param
 * @brief Xilinx specific timer initialization structure
 */
struct xil_timer_init_param {
	/** Timer used from the PL 3-timer core */
	uint8_t active_tmr;
	/** Platform selection parameter */
	enum xil_timer_type type;
};

/**
 * @brief Xilinx platform specific timer platform ops structure
 */
extern const struct no_os_timer_platform_ops xil_timer_ops;

#endif /* XILINX_TIMER_H_ */
