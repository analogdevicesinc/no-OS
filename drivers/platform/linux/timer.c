/***************************************************************************//**
*   @file   generic/timer.c
*   @brief  Timer control module source.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "error.h"
#include "timer.h"

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
int32_t timer_init(struct timer_desc **desc,
		   struct timer_init_param *param)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}
	if (param) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Free the memory allocated by timer_setup().
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t timer_remove(struct timer_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Start a timer.
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t timer_start(struct timer_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}
}

/**
 * @brief Stop a timer from counting.
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t timer_stop(struct timer_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}
}

/**
 * @brief Get the value of the counter register for the timer.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] counter - Pointer to the counter value.
 * @return 0 in case of success, error code otherwise.
 */
int32_t timer_counter_get(struct timer_desc *desc, uint32_t *counter)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (counter) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Set the timer counter register value.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] new_val - The new value of the counter register.
 * @return 0 in case of success, error code otherwise.
 */
int32_t timer_counter_set(struct timer_desc *desc, uint32_t new_val)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (new_val) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Get the timer clock frequency.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] freq_hz - The value in Hz of the timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t timer_count_clk_get(struct timer_desc *desc, uint32_t *freq_hz)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (freq_hz) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Set the timer clock frequency.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] freq_hz - The value in Hz of the new timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t timer_count_clk_set(struct timer_desc *desc, uint32_t freq_hz)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (freq_hz) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

