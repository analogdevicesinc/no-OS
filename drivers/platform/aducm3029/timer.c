/***************************************************************************//**
 *   @file   aducm3029/timer.c
 *   @brief  Implementation of TIMER driver for ADuCM302x.
 *
 *   This driver enables the user to create multiple instance of a
 *   timer using only one hardware timer of the ADuCM3029.
 *
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include <stdlib.h>
#include <drivers/tmr/adi_tmr.h>
#include "timer_extra.h"
#include "timer.h"
#include "error.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** With this load we get an interrupt each millisecond */
#define HFOSC_LOAD	26000u
/** 1 KHz value */
#define FREQ_1KHZ	1000u
/** 1 MHz value */
#define FREQ_1MHZ	1000000u
/** Converts the timer value into microseconds */
#define MHZ26_TO_US(count)	((HFOSC_LOAD - count) / (HFOSC_LOAD / 1000u))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** Incremented each millisecond by \ref tmr_callback() */
static volatile uint64_t	g_count;
/** Counts the number of instances created */
static uint32_t			nb_instances;
/** Counts the number of started timers */
static uint32_t			nb_enables;
/** Current hardware timer used */
static uint32_t			timer_id;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * Called each millisecond and used to increment a global counter
 * @param param - Unused
 * @param tmr_event - Unused
 * @param arg - Unused
 */
static void tmr_callback(void *param, uint32_t tmr_event, void *arg)
{
	g_count++;
}

/**
 * @brief Initialize a instance of a timer.
 *
 * The created descriptor must be used in the next driver calls to identify the
 * timer instance.\n
 * The user can create multiple instances but only one hardware timer will be
 * used.\n
 * The timer used is defined by the \ref timer_init_param.id .\n
 * The driver will use the timer selected for the first instance for all the new
 * ones. If it is needed to change the hardware timer, the user must remove all
 * the instances and initialize a new one for the desired hardware timer.
 * @param desc - Pointer to descriptor of the instance that will be created.
 * @param param - Parameter used to configure the new instance. The extra field
 * is unused and should be NULL.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t timer_init(struct timer_desc **desc, struct timer_init_param *param)
{
	struct timer_desc *ldesc;
	struct aducm_timer_desc *aducm_desc;
	ADI_TMR_CONFIG		tmr_conf;

	if (!desc || !param || param->freq_hz > FREQ_1MHZ)
		return FAILURE;

	if (!(ldesc = calloc(1, sizeof(*ldesc))))
		return FAILURE;

	if (!(aducm_desc = calloc(1, sizeof(*aducm_desc)))) {
		free(ldesc);
		*desc = NULL;
		return FAILURE;
	}

	if (nb_instances == 0) {
		timer_id = param->id;
		adi_tmr_Init(timer_id, tmr_callback, NULL, true);
		/* Set the timer configuration */
		tmr_conf.bCountingUp = false;
		tmr_conf.bPeriodic = true;
		tmr_conf.ePrescaler = ADI_TMR_PRESCALER_1;
		tmr_conf.eClockSource = ADI_TMR_CLOCK_HFOSC;
		tmr_conf.nLoad = HFOSC_LOAD;
		tmr_conf.nAsyncLoad = HFOSC_LOAD;
		tmr_conf.bReloading = true;
		tmr_conf.bSyncBypass = true;
		while (ADI_TMR_DEVICE_BUSY == adi_tmr_ConfigTimer(timer_id,
				&tmr_conf));
	}

	nb_instances++;
	ldesc->id = timer_id;
	ldesc->extra = aducm_desc;
	ldesc->freq_hz = param->freq_hz;
	ldesc->load_value = param->load_value;

	*desc = ldesc;

	return SUCCESS;
}

/**
 * @brief Remove a instance of a timer.
 *
 * If there are no more instances it stops the hardware timer.
 * @param desc - Descriptor of the timer instance.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t timer_remove(struct timer_desc *desc)
{
	if (!desc)
		return FAILURE;
	free(desc->extra);
	free(desc);
	nb_instances--;
	if (nb_instances == 0) {
		while (ADI_TMR_DEVICE_BUSY == adi_tmr_Enable(timer_id, false));
		g_count = 0;
		nb_enables = 0;

	}

	return SUCCESS;
}

static inline uint64_t get_current_time(struct timer_desc *desc)
{
	uint16_t		count_us = 0;

	if (desc->freq_hz > FREQ_1KHZ) {
		adi_tmr_GetCurrentCount(timer_id, &count_us);
		return g_count * 1000u + MHZ26_TO_US(count_us);
	} else {
		return g_count;
	}
}

/**
 * @brief Enable counting in the timer instance.
 *
 * The timer starts counting from the load value used in \ref timer_init(), set
 * by \ref timer_counter_set() or from the value the timer stopped at.
 * @param desc - Descriptor of the timer instance.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t timer_start(struct timer_desc *desc)
{
	struct aducm_timer_desc *tmr_desc;

	if (!desc)
		return FAILURE;

	tmr_desc = desc->extra;
	if (tmr_desc->started)
		return FAILURE;
	if (nb_enables == 0)
		while (ADI_TMR_DEVICE_BUSY == adi_tmr_Enable(timer_id, true));
	tmr_desc->old_time = get_current_time(desc);
	tmr_desc->started = true;
	nb_enables++;

	return SUCCESS;
}

/**
 * @brief Stop counting the timer instance.
 *
 * If there are no more timer instance started it also stops the hardware timer.
 * @param desc - Descriptor of the timer instance.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t timer_stop(struct timer_desc *desc)
{
	uint32_t		counter;
	struct aducm_timer_desc *tmr_desc;

	if (!desc)
		return FAILURE;

	tmr_desc = desc->extra;
	if (!tmr_desc->started)
		return FAILURE;

	timer_counter_get(desc, &counter);
	desc->load_value = counter;
	if (nb_enables == 1)
		while (ADI_TMR_DEVICE_BUSY == adi_tmr_Enable(timer_id, false));
	nb_enables--;
	tmr_desc->started = false;

	return SUCCESS;
}

/**
 * @brief Get the value the timer is at.
 * @param desc - Descriptor of the timer instance.
 * @param counter - Pointer were the timer counter is stored.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t timer_counter_get(struct timer_desc *desc, uint32_t *counter)
{
	struct aducm_timer_desc	*tmr_desc;
	uint16_t		count_us;
	uint64_t		local_count;
	uint64_t		new_time;
	uint64_t		big_counter;

	if (!desc || !counter)
		return FAILURE;

	tmr_desc = desc->extra;
	if (tmr_desc->started == 0) {
		*counter = desc->load_value;
		return SUCCESS;
	}
	/*
	 * Save the global count to local variable because it can suffer
	 * modifications during calculations.
	 */
	local_count = g_count;
	if (desc->freq_hz > FREQ_1KHZ) {
		adi_tmr_GetCurrentCount(timer_id, &count_us);
		count_us = MHZ26_TO_US(count_us);
		new_time = local_count * 1000u + count_us;
		if (new_time < tmr_desc->old_time)
			new_time += 1000;
		big_counter = new_time - tmr_desc->old_time;
		/*
		 * Transform from the base counting frequency (1MHz) to the
		 * frequency requested by the application
		 */
		*counter = (big_counter * desc->freq_hz) / FREQ_1MHZ;
	} else {
		big_counter = local_count - tmr_desc->old_time;
		/*
		 * Transform from the base counting frequency (1KHz) to the
		 * frequency requested by the application
		 */
		*counter = (big_counter * desc->freq_hz) / FREQ_1KHZ;
	}
	*counter += desc->load_value;

	return SUCCESS;
}

/**
 * @brief Set a new value for the timer.
 * @note  This function can be called only when the timer is stopped.
 * @param desc - Descriptor of the timer instance.
 * @param new_val - Value of the new counter to be set
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t timer_counter_set(struct timer_desc *desc, uint32_t new_val)
{
	struct aducm_timer_desc	*tmr_desc;

	if (!desc || !desc->extra)
		return FAILURE;

	tmr_desc = (struct aducm_timer_desc *)desc->extra;
	tmr_desc->old_time = get_current_time(desc);
	desc->load_value = new_val;

	return SUCCESS;
}

/**
 * @brief Get the frequency the timer counts with.
 * @param desc - Descriptor of the timer instance.
 * @param freq_hz - Pointer where the frequency value will be stored
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t timer_count_clk_get(struct timer_desc *desc, uint32_t *freq_hz)
{
	if (!desc || !freq_hz)
		return FAILURE;

	*freq_hz = desc->freq_hz;

	return SUCCESS;
}

/**
 * @brief Set the frequency the timer counts with.
 * @note This function can be called only when the timer is stopped.
 * @param desc - Descriptor of the timer instance.
 * @param freq_hz - Value of the new frequency to be set
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t timer_count_clk_set(struct timer_desc *desc, uint32_t freq_hz)
{
	if (!desc || ((struct aducm_timer_desc*)desc->extra)->started)
		return FAILURE;

	desc->freq_hz = freq_hz;

	return SUCCESS;
}
