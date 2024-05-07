/***************************************************************************//**
 *   @file   aducm3029/aducm3029_timer.c
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
#include <drivers/pwr/adi_pwr.h>
#include "aducm3029_timer.h"
#include "no_os_timer.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** With this load we get an interrupt each millisecond */
#define NO_OS_HFOSC_LOAD	26000u
/** 1 KHz value */
#define NO_OS_FREQ_1KHZ	1000u
/** 1 MHz value */
#define NO_OS_FREQ_1MHZ	1000000u
/** Converts the timer value into microseconds */
#define NO_OS_MHZ26_TO_US(count)	((NO_OS_HFOSC_LOAD - count) / (NO_OS_HFOSC_LOAD / 1000u))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** Incremented each millisecond by \ref aducm3029_tmr_callback() */
static volatile uint64_t	g_count;
/** Counts the number of instances created */
static uint32_t			nb_instances;
/** Counts the number of started timers */
static uint32_t			nb_enables;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * Called each millisecond and used to increment a global counter
 * @param param - Unused
 * @param tmr_event - Unused
 * @param arg - Unused
 */
static void aducm3029_tmr_callback(void *param, uint32_t tmr_event, void *arg)
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
 * The timer used is defined by the \ref no_os_timer_init_param.id .\n
 * The driver will use the timer selected for the first instance for all the new
 * ones. If it is needed to change the hardware timer, the user must remove all
 * the instances and initialize a new one for the desired hardware timer.
 * @param desc - Pointer to descriptor of the instance that will be created.
 * @param param - Parameter used to configure the new instance. The extra field
 * is unused and should be NULL.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_timer_init(struct no_os_timer_desc **desc,
			     const struct no_os_timer_init_param *param)
{
	struct no_os_timer_desc *ldesc;
	struct aducm_timer_desc *aducm_desc;
	struct aducm_timer_init_param *aducm_timer_ip;
	ADI_TMR_CONFIG		tmr_conf;
	uint32_t freq;
	uint32_t prescaler;

	if (!desc || !param || param->freq_hz > NO_OS_FREQ_1MHZ)
		return -1;

	if (!(ldesc = no_os_calloc(1, sizeof(*ldesc))))
		return -1;

	if (!(aducm_desc = no_os_calloc(1, sizeof(*aducm_desc)))) {
		no_os_free(ldesc);
		*desc = NULL;
		return -1;
	}
	aducm_timer_ip = param->extra;

	ldesc->extra = aducm_desc;
	ldesc->ticks_count = param->ticks_count;
	ldesc->freq_hz = param->freq_hz;

	if (param->id == 0) {
		/* Timer id 0 will be used for delays.
		   It can be instantiated multiple times */
		ldesc->id = 0;
		if (nb_instances == 0) {
			adi_tmr_Init(ldesc->id, aducm3029_tmr_callback, NULL, true);
			/* Set the timer configuration */
			tmr_conf.bCountingUp = false;
			tmr_conf.bPeriodic = true;
			tmr_conf.ePrescaler = ADI_TMR_PRESCALER_1;
			tmr_conf.eClockSource = ADI_TMR_CLOCK_HFOSC;
			tmr_conf.nLoad = NO_OS_HFOSC_LOAD;
			tmr_conf.nAsyncLoad = NO_OS_HFOSC_LOAD;
			tmr_conf.bReloading = true;
			tmr_conf.bSyncBypass = true;
			while (ADI_TMR_DEVICE_BUSY == adi_tmr_ConfigTimer(ldesc->id,
					&tmr_conf));
		}

		nb_instances++;
	} else {
		/* Timer id 1 will be used for time triggered interrupts.
			Every time this function is called the timer will be re-initialized */
		ldesc->id = 1;
		adi_tmr_Init(ldesc->id, NULL, NULL, false);
		/* Set the timer configuration */
		tmr_conf.bCountingUp = false;
		tmr_conf.bPeriodic = true;
		tmr_conf.ePrescaler = aducm_timer_ip->source_freq % 4;
		tmr_conf.eClockSource = aducm_timer_ip->source_freq / 4;

		if (aducm_timer_ip->source_freq < HFOSC_DIV1)
			adi_pwr_GetClockFrequency(ADI_CLOCK_PCLK, &freq);
		else if (aducm_timer_ip->source_freq < LFOSC_DIV1)
			adi_pwr_GetClockFrequency(ADI_CLOCK_HCLK, &freq);
		else
			/* 32 KHz clock frequency */
			freq = 32000;

		switch (tmr_conf.ePrescaler) {
		case ADI_TMR_PRESCALER_1:
			prescaler = 1;
			break;
		case ADI_TMR_PRESCALER_16:
			prescaler = 16;
			break;
		case ADI_TMR_PRESCALER_64:
			prescaler = 64;
			break;
		case ADI_TMR_PRESCALER_256:
			prescaler = 256;
			break;
		default:
			no_os_free(aducm_desc);
			no_os_free(ldesc);
			return -EINVAL;
			break;
		}

		freq /= prescaler;
		tmr_conf.nLoad = param->ticks_count;
		tmr_conf.nAsyncLoad = param->ticks_count;
		tmr_conf.bReloading = true;
		tmr_conf.bSyncBypass = true;
		aducm_desc->tmr_conf = tmr_conf;
		ldesc->freq_hz = freq;
	}

	*desc = ldesc;

	return 0;
}

/**
 * @brief Remove a instance of a timer.
 *
 * If there are no more instances it stops the hardware timer.
 * @param desc - Descriptor of the timer instance.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_timer_remove(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -1;
	no_os_free(desc->extra);
	no_os_free(desc);
	if (desc->id == 0) {
		nb_instances--;
		if (nb_instances == 0) {
			while (ADI_TMR_DEVICE_BUSY == adi_tmr_Enable(0, false));
			g_count = 0;
			nb_enables = 0;
		}
	} else {
		while (ADI_TMR_DEVICE_BUSY == adi_tmr_Enable(desc->id, false));
	}

	return 0;
}

static inline uint64_t aducm3029_get_current_time(struct no_os_timer_desc *desc)
{
	uint16_t		count_us = 0;

	if (desc->freq_hz > NO_OS_FREQ_1KHZ) {
		adi_tmr_GetCurrentCount(desc->id, &count_us);
		return g_count * 1000u + NO_OS_MHZ26_TO_US(count_us);
	} else {
		return g_count;
	}
}

/**
 * @brief Enable counting in the timer instance.
 *
 * The timer starts counting from the load value used in \ref no_os_timer_init(), set
 * by \ref no_os_timer_counter_set() or from the value the timer stopped at.
 * @param desc - Descriptor of the timer instance.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_timer_start(struct no_os_timer_desc *desc)
{
	struct aducm_timer_desc *tmr_desc;

	if (!desc)
		return -1;

	tmr_desc = desc->extra;
	if (tmr_desc->started)
		return -1;
	if (desc->id == 0) {
		if (nb_enables == 0)
			while (ADI_TMR_DEVICE_BUSY == adi_tmr_Enable(0, true));
		tmr_desc->old_time = aducm3029_get_current_time(desc);
		tmr_desc->started = true;
		nb_enables++;
	} else {
		while (ADI_TMR_DEVICE_BUSY == adi_tmr_Enable(desc->id, true));
		tmr_desc->started = true;
	}

	return 0;
}

/**
 * @brief Stop counting the timer instance.
 *
 * If there are no more timer instance started it also stops the hardware timer.
 * @param desc - Descriptor of the timer instance.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_timer_stop(struct no_os_timer_desc *desc)
{
	uint32_t		counter;
	struct aducm_timer_desc *tmr_desc;

	if (!desc)
		return -1;

	tmr_desc = desc->extra;
	if (!tmr_desc->started)
		return -1;
	if (desc->id == 0) {
		no_os_timer_counter_get(desc, &counter);
		desc->ticks_count = counter;
		if (nb_enables == 1)
			while (ADI_TMR_DEVICE_BUSY == adi_tmr_Enable(0, false));
		nb_enables--;
		tmr_desc->started = false;
	} else {
		while (ADI_TMR_DEVICE_BUSY == adi_tmr_Enable(desc->id, false));
		tmr_desc->started = false;
	}

	return 0;
}

/**
 * @brief Get the value the timer is at.
 * @param desc - Descriptor of the timer instance.
 * @param counter - Pointer were the timer counter is stored.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_timer_counter_get(struct no_os_timer_desc *desc,
				    uint32_t *counter)
{
	struct aducm_timer_desc	*tmr_desc;
	uint16_t		count_us;
	uint64_t		local_count;
	uint64_t		new_time;
	uint64_t		big_counter;

	if (!desc || !counter)
		return -1;

	tmr_desc = desc->extra;
	if (tmr_desc->started == 0) {
		*counter = desc->ticks_count;
		return 0;
	}
	/*
	 * Save the global count to local variable because it can suffer
	 * modifications during calculations.
	 */
	local_count = g_count;
	if (desc->freq_hz > NO_OS_FREQ_1KHZ) {
		adi_tmr_GetCurrentCount(desc->id, &count_us);
		count_us = NO_OS_MHZ26_TO_US(count_us);
		new_time = local_count * 1000u + count_us;
		if (new_time < tmr_desc->old_time)
			new_time += 1000;
		big_counter = new_time - tmr_desc->old_time;
		/*
		 * Transform from the base counting frequency (1MHz) to the
		 * frequency requested by the application
		 */
		*counter = (big_counter * desc->freq_hz) / NO_OS_FREQ_1MHZ;
	} else {
		big_counter = local_count - tmr_desc->old_time;
		/*
		 * Transform from the base counting frequency (1KHz) to the
		 * frequency requested by the application
		 */
		*counter = (big_counter * desc->freq_hz) / NO_OS_FREQ_1KHZ;
	}
	*counter += desc->ticks_count;

	return 0;
}

/**
 * @brief Set a new value for the timer.
 * @note  This function can be called only when the timer is stopped.
 * @param desc - Descriptor of the timer instance.
 * @param new_val - Value of the new counter to be set
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_timer_counter_set(struct no_os_timer_desc *desc,
				    uint32_t new_val)
{
	struct aducm_timer_desc	*tmr_desc;

	if (!desc || !desc->extra)
		return -1;

	tmr_desc = (struct aducm_timer_desc *)desc->extra;
	tmr_desc->old_time = aducm3029_get_current_time(desc);
	desc->ticks_count = new_val;

	return 0;
}

/**
 * @brief Get the frequency the timer counts with.
 * @param desc - Descriptor of the timer instance.
 * @param freq_hz - Pointer where the frequency value will be stored
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_timer_count_clk_get(struct no_os_timer_desc *desc,
				      uint32_t *freq_hz)
{
	if (!desc || !freq_hz)
		return -1;

	*freq_hz = desc->freq_hz;

	return 0;
}

/**
 * @brief Set the frequency the timer counts with.
 * @note This function can be called only when the timer is stopped.
 * @param desc - Descriptor of the timer instance.
 * @param freq_hz - Value of the new frequency to be set
 * @return 0 in case of success, -1 otherwise.
 */
int32_t aducm3029_timer_count_clk_set(struct no_os_timer_desc *desc,
				      uint32_t freq_hz)
{
	if (!desc || ((struct aducm_timer_desc*)desc->extra)->started)
		return -1;

	desc->freq_hz = freq_hz;

	return 0;
}

/**
 * @brief Get the elapsed time in nsec for the timer.
 * @param [in] desc         - Pointer to the device handler.
 * @param [in] elapsed_time - The elapsed time in nsec.
 * @return 0 in case of success, error code otherwise.
 */
int32_t aducm3029_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
		uint64_t *elapsed_time)
{
	/* Function not implemented because it is not needed at the moment */
	return -ENOSYS;
}

/**
 * @brief aducm3029 platform specific timer platform ops structure
 */
const struct no_os_timer_platform_ops aducm_timer_ops = {
	.init = &aducm3029_timer_init,
	.start = &aducm3029_timer_start,
	.stop = &aducm3029_timer_stop,
	.counter_get = &aducm3029_timer_counter_get,
	.counter_set = &aducm3029_timer_counter_set,
	.count_clk_get = &aducm3029_timer_count_clk_get,
	.count_clk_set = &aducm3029_timer_count_clk_set,
	.get_elapsed_time_nsec = &aducm3029_timer_get_elapsed_time_nsec,
	.remove = &aducm3029_timer_remove
};
