/***************************************************************************//**
*   @file   no_os_timer.h
*   @brief  Timer control module header.
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

#ifndef _NO_OS_SRC_TIMER_H_
#define _NO_OS_SRC_TIMER_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define TIMER_MAX_TABLE 4

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct no_os_timer_desc
 * @brief Structure holding timer descriptor
 */
struct no_os_timer_desc {
	/** Timer mutex*/
	void *mutex;
	/** timer ID */
	uint16_t id;
	/** timer count frequency (Hz) */
	uint32_t freq_hz;
	/** the number of ticks the timer counts until it resets */
	uint32_t ticks_count;
	/** Timer platform operations */
	const struct no_os_timer_platform_ops *platform_ops;
	/** timer extra parameters (device specific) */
	void *extra;
};

/**
 * @struct no_os_timer_platform_ops
 * @brief Structure holding timer function pointers that point to the platform
 * specific function
 */
struct no_os_timer_platform_ops ;

/**
 * @struct no_os_timer_init_param
 * @brief  Structure holding the parameters for timer initialization
 */
struct no_os_timer_init_param {
	/** timer ID */
	uint16_t id;
	/** timer count frequency (Hz) */
	uint32_t freq_hz;
	/** the number of ticks the timer counts until it resets */
	uint32_t ticks_count;
	/** Timer platform operations */
	const struct no_os_timer_platform_ops *platform_ops;
	/** timer extra parameters (device specific) */
	void *extra;
};

/**
 * @struct no_os_timer_platform_ops
 * @brief Structure holding timer function pointers that point to the platform
 * specific function
 */
struct no_os_timer_platform_ops {
	/** timer initialization function pointer */
	int32_t (*init)(struct no_os_timer_desc **,
			const struct no_os_timer_init_param *);
	/** timer start function pointer */
	int32_t (*start)(struct no_os_timer_desc *);
	/** timer stop function pointer */
	int32_t (*stop)(struct no_os_timer_desc *);
	/** timer get counter function pointer */
	int32_t (*counter_get)(struct no_os_timer_desc *, uint32_t *counter);
	/** timer set counter function pointer */
	int32_t (*counter_set)(struct no_os_timer_desc *, uint32_t new_val);
	/** timer get clock frequency function pointer */
	int32_t (*count_clk_get)(struct no_os_timer_desc *, uint32_t *freq_hz);
	/** timer set clock frequency function pointer */
	int32_t (*count_clk_set)(struct no_os_timer_desc *, uint32_t freq_hz);
	/** timer get elapsed time in nsec */
	int32_t (*get_elapsed_time_nsec)(struct no_os_timer_desc *,
					 uint64_t *elapsed_time);
	/** timer remove function pointer */
	int32_t (*remove)(struct no_os_timer_desc *);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize hardware timer and the handler structure associated with it. */
int32_t no_os_timer_init(struct no_os_timer_desc **desc,
			 const struct no_os_timer_init_param *param);

/* Free the memory allocated by timer_setup(). */
int32_t no_os_timer_remove(struct no_os_timer_desc *desc);

/* Start a timer. */
int32_t no_os_timer_start(struct no_os_timer_desc *desc);

/* Stop a timer from counting. */
int32_t no_os_timer_stop(struct no_os_timer_desc *desc);

/* Get the value of the counter register for the timer. */
int32_t no_os_timer_counter_get(struct no_os_timer_desc *desc,
				uint32_t *counter);

/* Set the timer counter register value. */
int32_t no_os_timer_counter_set(struct no_os_timer_desc *desc,
				uint32_t new_val);

/* Get the timer clock frequency. */
int32_t no_os_timer_count_clk_get(struct no_os_timer_desc *desc,
				  uint32_t *freq_hz);

/* Set the timer clock frequency. */
int32_t no_os_timer_count_clk_set(struct no_os_timer_desc *desc,
				  uint32_t freq_hz);

/* Get the elapsed time in nsec for the timer. */
int32_t no_os_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
		uint64_t *elapsed_time);

#endif // _NO_OS_SRC_TIMER_H_
