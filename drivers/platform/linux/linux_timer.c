/***************************************************************************//**
 *   @file   linux_timer.c
 *   @brief  Source file for Linux timer platform driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "no_os_error.h"
#include "no_os_timer.h"
#include "no_os_alloc.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct linux_timer_desc
 * @brief Linux platform specific timer descriptor
 */
struct linux_timer_desc {
	timer_t		timer_id;
	bool		enable;
	struct timespec	start_time;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Timer driver init function
 * @param desc - timer descriptor to be initialized
 * @param param - initialization parameter for the desc
 * @return 0 in case of success, negative errno error codes otherwise.
 */
int linux_timer_init(struct no_os_timer_desc **desc,
		     const struct no_os_timer_init_param *param)
{
	struct no_os_timer_desc *descriptor;
	struct linux_timer_desc *linux_desc;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	linux_desc = no_os_calloc(1, sizeof(*linux_desc));
	if (!linux_desc)
		goto free_desc;

	descriptor->extra = linux_desc;

	descriptor->id = param->id;
	descriptor->freq_hz = param->freq_hz;
	descriptor->ticks_count = param->ticks_count;

	*desc = descriptor;

	return 0;

free_desc:
	no_os_free(descriptor);

	return -ENOMEM;
}

/**
 * @brief Timer driver remove function
 * @param desc - timer descriptor
 * @return 0 in case of success, -EINVAL otherwise.
 */
int linux_timer_remove(struct no_os_timer_desc *desc)
{
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Timer count start function
 * @param desc - timer descriptor
 * @return 0 in case of success, -EINVAL otherwise.
 */
int linux_timer_start(struct no_os_timer_desc *desc)
{
	struct linux_timer_desc *linux_desc;

	linux_desc = desc->extra;

	clock_gettime(CLOCK_REALTIME, &linux_desc->start_time);
	linux_desc->enable = true;

	return 0;
}

/**
 * @brief Timer count stop function
 * @param desc - timer descriptor
 * @return 0 in case of success, -EINVAL otherwise.
 */
int linux_timer_stop(struct no_os_timer_desc *desc)
{
	struct linux_timer_desc *linux_desc;

	linux_desc = desc->extra;

	linux_desc->enable = false;

	return 0;
}

/**
 * @brief Function to get the current timer counter value
 * @param desc - timer descriptor
 * @param counter - the timer counter value
 * @return 0 in case of success, -EINVAL otherwise.
 */
int linux_timer_counter_get(struct no_os_timer_desc *desc,
			    uint32_t *counter)
{
	struct linux_timer_desc *linux_desc;
	struct timespec curr_value;

	linux_desc = desc->extra;

	clock_gettime(CLOCK_REALTIME, &curr_value);

	curr_value.tv_sec -= linux_desc->start_time.tv_sec;

	if (curr_value.tv_sec) {
		curr_value.tv_nsec += 1000000000 -
				      linux_desc->start_time.tv_nsec;
		if (curr_value.tv_nsec >= 1000000000)
			curr_value.tv_nsec -= 1000000000;
		else
			curr_value.tv_sec--;
	} else {
		curr_value.tv_nsec -= linux_desc->start_time.tv_nsec;
	}

	*counter = curr_value.tv_sec * 1000 + curr_value.tv_nsec / 1000000;

	return 0;
}

/**
 * @brief Function to set the timer counter value
 * @param desc - timer descriptor
 * @param new_val - timer counter value to be set
 * @return 0 in case of success, -EINVAL otherwise.
 */
int linux_timer_counter_set(struct no_os_timer_desc *desc,
			    uint32_t new_val)
{
	struct linux_timer_desc *linux_desc;
	struct timespec curr_value;

	linux_desc = desc->extra;

	clock_gettime(CLOCK_REALTIME, &curr_value);

	linux_desc->start_time.tv_sec = curr_value.tv_sec + new_val / 1000;
	linux_desc->start_time.tv_nsec = curr_value.tv_nsec +
					 (new_val % 1000) * 1000000;
	if (linux_desc->start_time.tv_nsec >= 1000000000) {
		linux_desc->start_time.tv_nsec -= 1000000000;
		linux_desc->start_time.tv_sec++;
	}

	return 0;
}

/**
 * @brief Function to get the timer frequency
 * @param desc - timer descriptor
 * @param freq_hz - the timer frequency value
 * @return 0 in case of success, -EINVAL otherwise.
 */
int linux_timer_count_clk_get(struct no_os_timer_desc *desc,
			      uint32_t *freq_hz)
{
	*freq_hz = 1000;

	return 0;
}

/**
 * @brief Function to set the timer frequency.
 * @param desc - timer descriptor.
 * @param freq_hz - the timer frequency value to be set.
 * @return 0 in case of success, negative errno error codes otherwise.
 */
int linux_timer_count_clk_set(struct no_os_timer_desc *desc,
			      uint32_t freq_hz)
{
	return -ENOSYS;
}

/**
 * @brief Not implemented
 * @param desc - timer descriptor
 * @param elapsed_time - time in nanoseconds
 * @return 0 in case of success, negative errno error codes otherwise.
 */
int linux_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
				      uint64_t *elapsed_time)
{
	struct linux_timer_desc *linux_desc;
	struct timespec curr_value;

	linux_desc = desc->extra;

	clock_gettime(CLOCK_REALTIME, &curr_value);

	*elapsed_time = curr_value.tv_sec * 1000000000 + curr_value.tv_nsec -
			linux_desc->start_time.tv_sec * 1000000000 -
			linux_desc->start_time.tv_nsec;

	return 0;
}

/**
 * @brief linux platform specific timer platform ops structure
 */
const struct no_os_timer_platform_ops linux_timer_ops = {
	.init = (int32_t (*)())linux_timer_init,
	.start = (int32_t (*)())linux_timer_start,
	.stop = (int32_t (*)())linux_timer_stop,
	.counter_get = (int32_t (*)())linux_timer_counter_get,
	.counter_set = (int32_t (*)())linux_timer_counter_set,
	.count_clk_get = (int32_t (*)())linux_timer_count_clk_get,
	.count_clk_set = (int32_t (*)())linux_timer_count_clk_set,
	.get_elapsed_time_nsec =
	(int32_t (*)())linux_timer_get_elapsed_time_nsec,
	.remove = (int32_t (*)())linux_timer_remove
};

