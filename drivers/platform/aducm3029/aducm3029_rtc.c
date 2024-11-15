/***************************************************************************//**
 *   @file   aducm3029_rtc.c
 *   @brief  Implementation of RTC driver for ADuCM302x
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_rtc.h"
#include "aducm3029_rtc.h"
#include "no_os_irq.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the RTC peripheral.
 * @param device - The RTC descriptor.
 * @param init_param - The structure that contains the RTC initialization.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_rtc_init(struct no_os_rtc_desc **device,
		       struct no_os_rtc_init_param *init_param)
{
	int32_t ret;
	struct no_os_rtc_desc *dev;
	struct aducm_rtc_desc *adev;

	dev = (struct no_os_rtc_desc *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	adev = (struct aducm_rtc_desc *)no_os_calloc(1, sizeof(*adev));
	if (!adev)
		goto error_dev;

	adev->memory = no_os_calloc(1, ADI_RTC_MEMORY_SIZE);
	if(!adev->memory)
		goto error_extra;

	dev->id = init_param->id;
	dev->freq = init_param->freq;
	dev->load = init_param->load;
	dev->extra = adev;

	ret = adi_rtc_Open(dev->id, adev->memory, ADI_RTC_MEMORY_SIZE,
			   &adev->instance);
	if(ret != 0)
		goto error_mem;

	if ((dev->freq >= AUDCM_32768HZ) && (dev->freq <= AUDCM_1HZ)) {
		ret = adi_rtc_SetPreScale(adev->instance, dev->freq);
		if(ret != 0)
			goto error_open;
	} else {
		goto error_open;
	}

	ret = adi_rtc_SetCount(adev->instance, dev->load);
	if (ret != 0)
		goto error_open;

	*device = dev;

	return 0;

error_open:
	adi_rtc_Close(adev->instance);
error_mem:
	no_os_free(adev->memory);
error_extra:
	no_os_free(adev);
error_dev:
	no_os_free(dev);

	return -1;
}

/**
 * @brief Free the resources allocated by no_os_rtc_init().
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_rtc_remove(struct no_os_rtc_desc *dev)
{
	int32_t ret;
	struct aducm_rtc_desc *adev = dev->extra;

	ret = adi_rtc_Close(adev->instance);
	if(ret != 0)
		return -1;

	no_os_free(adev->memory);
	no_os_free(adev);
	no_os_free(dev);

	return 0;
}

/**
 * @brief Start the real time clock.
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_rtc_start(struct no_os_rtc_desc *dev)
{
	struct aducm_rtc_desc *adev;

	if (!dev)
		return -1;
	adev = dev->extra;

	return adi_rtc_Enable(adev->instance, true);
}

/**
 * @brief Stop the real time clock.
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_rtc_stop(struct no_os_rtc_desc *dev)
{
	struct aducm_rtc_desc *adev;

	if (!dev)
		return -1;
	adev = dev->extra;

	return adi_rtc_Enable(adev->instance, false);
}

/**
 * @brief Get the current count for the real time clock.
 * @param dev - The RTC descriptor.
 * @param tmr_cnt - Pointer where the read counter will be stored.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_rtc_get_cnt(struct no_os_rtc_desc *dev, uint32_t *tmr_cnt)
{
	struct aducm_rtc_desc *adev = dev->extra;

	return adi_rtc_GetCount(adev->instance, tmr_cnt);
}

/**
 * @brief Set the current count for the real time clock.
 * @param dev - The RTC descriptor.
 * @param tmr_cnt - New value of the timer counter.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_rtc_set_cnt(struct no_os_rtc_desc *dev, uint32_t tmr_cnt)
{
	struct aducm_rtc_desc *adev = dev->extra;

	return adi_rtc_SetCount(adev->instance, tmr_cnt);
}
