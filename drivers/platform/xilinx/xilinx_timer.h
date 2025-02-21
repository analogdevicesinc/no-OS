/***************************************************************************//**
*   @file   xilinx_timer.h
*   @brief  Timer control module header.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2020(c) Analog Devices, Inc.
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

#ifndef XILINX_TIMER_H_
#define XILINX_TIMER_H_

#include <xparameters.h>
#ifdef XPAR_XSCUTIMER_NUM_INSTANCES
#include <xscutimer.h>
#endif
#ifdef XPAR_XTMRCTR_NUM_INSTANCES
#include <xtmrctr.h>
#endif

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
