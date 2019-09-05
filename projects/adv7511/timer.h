/***************************************************************************//**
*   @file   timer.h
*   @brief  Timer and GIC control module header.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "config.h"

#if (CURRENT_PLATFORM == PLATFORM_ZC702) || \
		(CURRENT_PLATFORM == PLATFORM_ZC706) || \
		(CURRENT_PLATFORM == PLATFORM_ZED)
#include <xscutimer.h>
#include <xscugic.h>
#include <xuartps.h>
#elif (CURRENT_PLATFORM == PLATFORM_KC705) || \
	(CURRENT_PLATFORM == PLATFORM_AC701) || (CURRENT_PLATFORM == PLATFORM_VC707)
#include <xtmrctr.h>
#include <xintc.h>
#include <xuartlite.h>
#include <xuartlite_l.h>
#endif
#include <stdbool.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#if defined(_XPARAMETERS_PS_H_)
#define CORE_PRIVATE_TIMER_CLOCK (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2)
#elif defined(XTMRCTR_H)
#define MB_AXI_TIMER_CLOCK XPAR_AXI_TIMER_CLOCK_FREQ_HZ
#endif

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct gic_desc {
#if defined(XSCUGIC_H)
	XScuGic_Config *config;
	XScuGic instance;
#elif defined(XINTC_H)
	XIntc instance;
#endif
	uint8_t id;
};

struct gic_init_param {
	uint8_t id;
};

struct timer_desc {
#if defined(XSCUTIMER_H)
	XScuTimer instance;
	XScuTimer_Config *config;
#elif defined(XTMRCTR_H)
	uint8_t active_tmr;
	XTmrCtr instance;
#endif
	uint8_t id;
	uint32_t load;
	uint32_t freq;
	bool auto_reload;
};

struct timer_init_param {
#if defined(XTMRCTR_H)
	uint8_t active_tmr;
#endif
	uint8_t id;
	uint32_t start_freq;
	bool auto_reload;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Timer ISR. */
#if defined(XSCUTIMER_H)
void timer_isr(void *instance);
#elif defined(XTMRCTR_H)
void timer_isr(void *instance, uint8_t timer_nr);
#endif

/* Initialize hardware timer and the handler structure associated with it. */
int32_t timer_setup(struct timer_desc **desc,
		    struct timer_init_param *init_param);

/* Free the memory allocated by timer_setup(). */
int32_t timer_remove(struct timer_desc *desc);

/* Start a timer. */
void timer_start(struct timer_desc *desc);

/* Stop a timer from counting. */
void timer_stop(struct timer_desc *desc);

/* Enable timer interrupt. */
void timer_enable_interrupt(struct timer_desc *desc);

/* Disable timer interrupt. */
void timer_disable_interrupt(struct timer_desc *desc);

/* Initialize the Generic Interrupt Controller (GIC) and get the pointer to its
 * device handler. */
int32_t gic_setup(struct gic_desc **desc, struct gic_init_param *init_param);

/* Free memory allocated by gic_setup(). */
int32_t gic_remove(struct gic_desc *desc);

/* Connect an interrupt ID and an ISR to be called by the GIC. */
int32_t gic_connect_handler(struct gic_desc *desc, uint32_t int_id,
			    Xil_ExceptionHandler isr_ptr, void *instance_ptr);

/* Disconnect a connected ISR from an interrupt ID in the GIC. */
void gic_disconnect_handler(struct gic_desc *desc, uint32_t int_id);

/* Enable GIC interrupt for a specific ID. */
void gic_enable_interrupt(struct gic_desc *desc, uint32_t int_id);

/* Disable GIC interrupt for a specific ID. */
void gic_disable_interrupt(struct gic_desc *desc, uint32_t int_id);

#endif /* SRC_TIMER_H_ */
