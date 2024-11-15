/***************************************************************************//**
*   @file   generic/generic_timer.c
*   @brief  Timer control module source.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_timer.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize hardware timer and the handler structure associated with
 *        it.
 * @param [out] desc - Pointer to the reference of the device handler.
 * @param [in] param - Initialization structure.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t generic_timer_init(struct no_os_timer_desc **desc,
			   const struct no_os_timer_init_param *param)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(param);

	return 0;
}

/**
 * @brief Free the memory allocated by timer_setup().
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t generic_timer_remove(struct no_os_timer_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);

	return 0;
}

/**
 * @brief Start a timer.
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t generic_timer_start(struct no_os_timer_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);

	return 0;
}

/**
 * @brief Stop a timer from counting.
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t generic_timer_stop(struct no_os_timer_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);

	return 0;
}

/**
 * @brief Get the value of the counter register for the timer.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] counter - Pointer to the counter value.
 * @return 0 in case of success, error code otherwise.
 */
int32_t generic_timer_counter_get(struct no_os_timer_desc *desc,
				  uint32_t *counter)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(counter);

	return 0;
}

/**
 * @brief Set the timer counter register value.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] new_val - The new value of the counter register.
 * @return 0 in case of success, error code otherwise.
 */
int32_t generic_timer_counter_set(struct no_os_timer_desc *desc,
				  uint32_t new_val)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(new_val);

	return 0;
}

/**
 * @brief Get the timer clock frequency.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] freq_hz - The value in Hz of the timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t generic_timer_count_clk_get(struct no_os_timer_desc *desc,
				    uint32_t *freq_hz)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(freq_hz);

	return 0;
}

/**
 * @brief Set the timer clock frequency.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] freq_hz - The value in Hz of the new timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t generic_timer_count_clk_set(struct no_os_timer_desc *desc,
				    uint32_t freq_hz)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(freq_hz);

	return 0;
}

/**
 * @brief Get the elapsed time in nsec for the timer.
 * @param [in] desc         - Pointer to the device handler.
 * @param [in] elapsed_time - The elapsed time in nsec.
 * @return 0 in case of success, error code otherwise.
 */
int32_t generic_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
		uint64_t *elapsed_time)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(elapsed_time);

	return 0;
}

/**
 * @brief generic platform specific timer platform ops structure
 */
const struct no_os_timer_platform_ops generic_timer_ops = {
	.init = &generic_timer_init,
	.start = &generic_timer_start,
	.stop = &generic_timer_stop,
	.counter_get = &generic_timer_counter_get,
	.counter_set = &generic_timer_counter_set,
	.count_clk_get = &generic_timer_count_clk_get,
	.count_clk_set = &generic_timer_count_clk_set,
	.get_elapsed_time_nsec = &generic_timer_get_elapsed_time_nsec,
	.remove = &generic_timer_remove
};
