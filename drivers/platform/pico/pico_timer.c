/***************************************************************************//**
 *   @file   pico/pico_timer.c
 *   @brief  Implementation of pico timer driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_timer.h"
#include "no_os_alloc.h"
#include "pico_timer.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define PICO_ALARM_MAX_NB 4

/******************************************************************************/
/************************* Variables Definitions ******************************/
/******************************************************************************/

struct no_os_timer_desc *pico_alarm_desc[PICO_ALARM_MAX_NB] = {NULL};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the timer peripheral.
 * @param desc  - The timer descriptor.
 * @param param - The structure that contains the timer parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_timer_init(struct no_os_timer_desc **desc,
			const struct no_os_timer_init_param *param)
{
	int32_t ret;
	struct no_os_timer_desc	*no_os_desc;
	struct pico_timer_desc *pico_timer;

	if (!desc || !param)
		return -EINVAL;

	/* Only 4 alarms available */
	if (param->id > PICO_ALARM_MAX_NB)
		return -EINVAL;

	/* Verify if alarm is already claimed */
	if (hardware_alarm_is_claimed(param->id))
		return -EINVAL;

	no_os_desc = (struct no_os_timer_desc *)no_os_calloc(1, sizeof(*no_os_desc));
	if (!no_os_desc)
		return -ENOMEM;

	pico_timer = (struct pico_timer_desc*)no_os_calloc(1,sizeof(*pico_timer));
	if (!pico_timer) {
		ret = -ENOMEM;
		goto error;
	}

	no_os_desc->extra = pico_timer;

	/* Get period of timer in microseconds */
	uint64_t period = param->ticks_count*1000000/param->freq_hz;

	/* Make sure the period is range */
	if (!period) {
		ret = -EINVAL;
		goto error;
	}

	pico_timer->period = period;

	/* Claim alarm for given id */
	hardware_alarm_claim(param->id);

	/* Copy settings to device descriptor */
	no_os_desc->extra = pico_timer;
	no_os_desc->id = param->id;
	no_os_desc->freq_hz = param->freq_hz;
	no_os_desc->ticks_count = param->ticks_count;
	*desc = no_os_desc;
	pico_alarm_desc[param->id] = no_os_desc;

	return 0;

error:
	no_os_free(no_os_desc);
	return ret;
}

/**
 * @brief Free the resources allocated by no_os_timer_init().
 * @param desc - The timer descriptor.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_timer_remove(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	/* Unclaim alarm */
	hardware_alarm_unclaim(desc->id);
	pico_alarm_desc[desc->id] = NULL;
	no_os_free(desc);

	return 0;
}

/**
 * @brief Start a timer.
 * @param desc - Pointer to the device handler.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_timer_start(struct no_os_timer_desc *desc)
{
	struct pico_timer_desc *pico_timer;

	if (!desc || !desc->extra)
		return -EINVAL;

	pico_timer = desc->extra;

	/* For pico platform, the timer starts counting from the boot-up and it cannot be stopped.
	An alarm shall be used to trigger interrupts at specific periods of time. */
	absolute_time_t target = {._private_us_since_boot = time_us_64() + pico_timer->period};
	hardware_alarm_set_target(desc->id, target);

	return 0;
}

/**
 * @brief Stop a timer from counting.
 * @param desc - Pointer to the device handler.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_timer_stop(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	/* For pico platform, the timer cannot be stopped from counting.
	It will continue counting, however, an alarm interrupt will no longer
	be generated, simulating the timer stop behavior. */
	/* Cancel alarm */
	hardware_alarm_cancel(desc->id);

	return 0;
}

/**
 * @brief Get the value of the counter register for the timer.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] counter - Pointer to the counter value.
 * @return -ENOSYS
 */
int32_t pico_timer_counter_get(struct no_os_timer_desc *desc,
			       uint32_t *counter)
{
	/* This function is not be implemented for pico platform due to the
	fact that the timer counter value is on 64 bits. The timer
	will never overflow and is always incrementing once per microsecond.
	The counter value can be obtained using pico_timer_get_elapsed_time_nsec */
	return -ENOSYS;
}

/**
 * @brief Set the timer counter register value.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] new_val - The new value of the counter register.
 * @return -ENOSYS
 */
int32_t pico_timer_counter_set(struct no_os_timer_desc *desc, uint32_t new_val)
{
	/* This function cannot be implemented for pico platform due to the
	fact that the timer counter value cannot be modified */
	return -ENOSYS;
}

/**
 * @brief Get the timer clock frequency.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] freq_hz - The value in Hz of the timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_timer_count_clk_get(struct no_os_timer_desc *desc,
				 uint32_t *freq_hz)
{
	if (!desc || !freq_hz)
		return -EINVAL;

	*freq_hz = desc->freq_hz;
	return 0;
}

/**
 * @brief Set the timer clock frequency.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] freq_hz - The value in Hz of the new timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_timer_count_clk_set(struct no_os_timer_desc *desc,
				 uint32_t freq_hz)
{
	struct pico_timer_desc *pico_timer;

	if (!desc || !desc->extra)
		return -EINVAL;

	pico_timer = desc->extra;

	/* For pico platform the clock source frequency of the timer is
	always the same. The used timer (system timer) is incrementing once per
	microseconds. When the user requests the change of the clock source
	frequency, it translates to a period change, where the period is the
	time interval at which an alarm is activated, generating an alarm interrupt. */

	/* Get period of timer in microseconds */
	uint64_t period = desc->ticks_count*1000000/freq_hz;

	if (!period)
		return -EINVAL;

	pico_timer->period = period;
	desc->freq_hz = freq_hz;

	return 0;
}

/**
 * @brief Get the elapsed time in nsec for the timer.
 * @param [in] desc         - Pointer to the device handler.
 * @param [in] elapsed_time - The elapsed time in nsec.
 * @return 0 in case of success, error code otherwise.
 */
int32_t pico_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
		uint64_t *elapsed_time)
{
	/* Returns the elapsed time in nanoseconds since the boot-up */
	*elapsed_time = time_us_64();
	return 0;
}

/**
 * @brief pico platform specific timer platform ops structure
 */
const struct no_os_timer_platform_ops pico_timer_ops = {
	.init = &pico_timer_init,
	.start = &pico_timer_start,
	.stop = &pico_timer_stop,
	.counter_get = &pico_timer_counter_get,
	.counter_set = &pico_timer_counter_set,
	.count_clk_get = &pico_timer_count_clk_get,
	.count_clk_set = &pico_timer_count_clk_set,
	.get_elapsed_time_nsec = &pico_timer_get_elapsed_time_nsec,
	.remove = &pico_timer_remove
};
