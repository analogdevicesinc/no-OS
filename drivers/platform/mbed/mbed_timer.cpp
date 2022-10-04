/**************************************************************************//**
 *   @file   mbed_timer.cpp
 *   @brief  Implementation of Timer Mbed Platform Driver Interfaces
*******************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
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

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include <stdio.h>
#include <mbed.h>

//Platform Drivers need to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif	//__cplusplus

#include "no_os_timer.h"
#include "no_os_error.h"
#include "mbed_timer.h"

using namespace std::chrono;

/*****************************************************************************/
/************************** Functions Definitions ****************************/
/*****************************************************************************/

/**
 * @brief Initialize the timer peripheral.
 * @param desc[in, out] - The Timer descriptor.
 * @param init_param[in] - Structure that contains Timer Initialization Parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_timer_init(struct no_os_timer_desc **desc,
			const struct no_os_timer_init_param *param)
{
	struct no_os_timer_desc *timer_desc; 		// Pointer to timer descriptor
	struct mbed_timer_desc *mbed_timer_desc;	// Pointer to mbed timer descriptor
	mbed::Timer *timer;					// Pointer to new Timer instance

	if (!desc || !param)
		return -EINVAL;

	timer_desc = (struct no_os_timer_desc *)calloc(1, sizeof(*timer_desc));
	if (!timer_desc)
		return -ENOMEM;

	mbed_timer_desc = (struct mbed_timer_desc *)calloc(1, sizeof(*mbed_timer_desc));
	if (!mbed_timer_desc)
		goto err_mbed_timer_desc;

	timer = new Timer();
	if (!timer)
		goto err_timer;

	mbed_timer_desc->timer = timer;
	timer_desc->extra = mbed_timer_desc;

	*desc = timer_desc;
	return 0;

err_timer:
	free(mbed_timer_desc);
err_mbed_timer_desc:
	free(timer_desc);

	return -ENOMEM;
}

/**
* @brief Start the Timer.
* @param desc[in] - The Timer Descriptor.
* @return 0 in case of success, negative error code otherwise.
*/
int32_t mbed_timer_start(struct no_os_timer_desc *desc)
{
	mbed::Timer *timer;

	if (!desc || !desc->extra)
		return -EINVAL;

	timer = (Timer *)(((struct mbed_timer_desc *)(desc->extra))->timer);
	if (!timer)
		return -EINVAL;

	timer->start();

	return 0;
}

/**
* @brief Stop the Timer.
* @param desc[in] - The Timer Descriptor.
* @return 0 in case of success, negative error code otherwise.
*/
int32_t mbed_timer_stop(struct no_os_timer_desc *desc)
{
	mbed::Timer *timer;

	if (!desc || !desc->extra)
		return -EINVAL;

	timer = (Timer *)(((struct mbed_timer_desc *)(desc->extra))->timer);
	if (!timer)
		return -EINVAL;

	timer->stop();

	return 0;
}

/**
* @brief Release all the resources allocated by Timer.
* @param desc[in] - The Timer Descriptor.
* @return 0 in case of success, negative error code otherwise.
*/
int32_t mbed_timer_remove(struct no_os_timer_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	/* Free the timer object */
	if ((Timer *)(((struct mbed_timer_desc *)(desc->extra))->timer))
		delete((Timer *)(((struct mbed_timer_desc *)(desc->extra))->timer));

	/* Free the Timer descriptors */
	free(desc->extra);
	free(desc);

	return 0;
}

/**
* @brief Get the elapsed time in nanoseconds.
* @param desc[in] - The Timer descriptor.
* @param elapsed_time[in, out] - Pointer where the elapsed time value is stored.
* @return 0 in case of success, negative error code otherwise.
*/
int32_t mbed_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
		uint64_t *elapsed_time)
{
	mbed::Timer *timer;

	if (!desc || !desc->extra || !elapsed_time)
		return -EINVAL;

	timer = (Timer *)(((struct mbed_timer_desc *)(desc->extra))->timer);
	if (!timer)
		return -EINVAL;

	*elapsed_time = duration_cast<nanoseconds>(timer->elapsed_time()).count();

	return 0;
}

/**
* @brief Mbed platform specific timer platform ops structure
*/
const struct no_os_timer_platform_ops mbed_timer_ops = {
	.init = &mbed_timer_init,
	.start = &mbed_timer_start,
	.stop = &mbed_timer_stop,
	.get_elapsed_time_nsec = &mbed_timer_get_elapsed_time_nsec,
	.remove = &mbed_timer_remove,
};

#ifdef __cplusplus
}
#endif // __cplusplus
