/**************************************************************************//**
 *   @file   mbed_timer.cpp
 *   @brief  Implementation of Timer Mbed Platform Driver Interfaces
*******************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
