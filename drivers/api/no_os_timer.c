/***************************************************************************//**
*   @file   no_os_timer.c
*   @brief  Implementation of the timer Interface
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stddef.h>
#include "no_os_error.h"
#include "no_os_timer.h"
#include "no_os_mutex.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * @brief - TIMER mutex
*/
static void *timer_mutex_table[TIMER_MAX_TABLE + 1];

/**
 * @brief Initialize hardware timer and the handler structure associated with
 *        it.
 * @param [out] desc - Pointer to the reference of the device handler.
 * @param [in] param - Initialization structure.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t no_os_timer_init(struct no_os_timer_desc **desc,
			 const struct no_os_timer_init_param *param)
{
	int32_t ret;

	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->init)
		return -ENOSYS;

	ret = param->platform_ops->init(desc, param);
	if (ret)
		return ret;

	(*desc)->platform_ops = param->platform_ops;

	no_os_mutex_init(&timer_mutex_table[param->id]);
	(*desc)->mutex = timer_mutex_table[param->id];

	return 0;
}

/**
 * @brief Free the memory allocated by timer_init().
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t no_os_timer_remove(struct no_os_timer_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	no_os_mutex_remove(desc->mutex);
	timer_mutex_table[desc->id] = NULL;

	return desc->platform_ops->remove(desc);
}

/**
 * @brief Start a timer.
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t no_os_timer_start(struct no_os_timer_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->start)
		return -ENOSYS;

	return desc->platform_ops->start(desc);
}

/**
 * @brief Stop a timer from counting.
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t no_os_timer_stop(struct no_os_timer_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->stop)
		return -ENOSYS;

	no_os_mutex_unlock(desc->mutex);
	return desc->platform_ops->stop(desc);
}

/**
 * @brief Get the value of the counter register for the timer.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] counter - Pointer to the counter value.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_timer_counter_get(struct no_os_timer_desc *desc,
				uint32_t *counter)
{
	if (!desc || !desc->platform_ops || !counter)
		return -EINVAL;

	if (!desc->platform_ops->counter_get)
		return -ENOSYS;

	return desc->platform_ops->counter_get(desc, counter);
}

/**
 * @brief Set the timer counter register value.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] new_val - The new value of the counter register.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_timer_counter_set(struct no_os_timer_desc *desc, uint32_t new_val)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->counter_set)
		return -ENOSYS;

	return desc->platform_ops->counter_set(desc, new_val);
}

/**
 * @brief Get the timer clock frequency.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] freq_hz - The value in Hz of the timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_timer_count_clk_get(struct no_os_timer_desc *desc,
				  uint32_t *freq_hz)
{
	if (!desc || !desc->platform_ops || !freq_hz)
		return -EINVAL;

	if (!desc->platform_ops->count_clk_get)
		return -ENOSYS;

	return desc->platform_ops->count_clk_get(desc, freq_hz);
}

/**
 * @brief Set the timer clock frequency.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] freq_hz - The value in Hz of the new timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_timer_count_clk_set(struct no_os_timer_desc *desc,
				  uint32_t freq_hz)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->count_clk_set)
		return -ENOSYS;

	return desc->platform_ops->count_clk_set(desc, freq_hz);
}

/**
 * @brief Get the elapsed time in nsec for the timer.
 * @param [in] desc         - Pointer to the device handler.
 * @param [in] elapsed_time - The elapsed time in nsec.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
		uint64_t *elapsed_time)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->get_elapsed_time_nsec)
		return -ENOSYS;

	return desc->platform_ops->get_elapsed_time_nsec(desc, elapsed_time);
}
