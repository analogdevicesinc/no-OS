/***************************************************************************//**
 *   @file   no_os_rtc.c
 *   @brief  Implementation of the RTC
 *   @author Francis Roi Manabat (francisroi.manabat@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <inttypes.h>
#include "no_os_rtc.h"
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_alloc.h"

/*
 * @brief Initialize the RTC communication peripheral.
 * @param desc - The RTC descriptor.
 * @param param - The structure that contains the RTC parameters.
 * @return 0 in case of success, -ERROR otherwise.
 */
int32_t no_os_rtc_init(struct no_os_rtc_desc **device,
		       struct no_os_rtc_init_param *init_param)
{
	int ret;

	/* Check if device pointer is NULL */
	if (!device) {
		return -EINVAL;
	}

	/* Check if Platform Ops is not NULL */
	if (!init_param || !init_param->platform_ops) {
		return -EINVAL;
	}
	/* Check if platform specific init function exists */
	if (!init_param->platform_ops->init)
		return -ENOSYS;

	/* Initilize RTC descriptor */
	ret = init_param->platform_ops->init(device, init_param);

	if (ret)
		return ret;

	(*device)->id = init_param->id;
	(*device)->extra = init_param->extra;
	(*device)->freq = init_param->freq;
	(*device)->load = init_param->load;
	(*device)->platform_ops = init_param->platform_ops;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_rtc_init().
 * @param device - The RTC descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_rtc_remove(struct no_os_rtc_desc *device)
{
	/* Check if device pointer is NULL */
	if (!device || !device->platform_ops)
		return -EINVAL;
	/* Check if Platform Specific remove function is NULL */
	if (!device->platform_ops->remove)
		return -ENOSYS;

	return device->platform_ops->remove(device);
}


/**
 * @brief Start the RTC device.
 * @param device - The RTC descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_rtc_start(struct no_os_rtc_desc *device)
{
	/* Check if device pointer is NULL */
	if (!device || !device->platform_ops)
		return -EINVAL;
	/* Check if Platform Specific start function is NULL */
	if (!device->platform_ops->start)
		return -ENOSYS;

	return device->platform_ops->start(device);
}

/**
 * @brief Stop the RTC device.
 * @param device - The RTC descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_rtc_stop(struct no_os_rtc_desc *device)
{
	int32_t  ret = 0;

	/* Check if device pointer is NULL */
	if (!device || !device->platform_ops)
		return -EINVAL;

	/* Check if Platform Specific stop function is NULL */
	if (!device->platform_ops->stop)
		return -ENOSYS;

	ret = device->platform_ops->stop(device);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Stop the RTC device.
 * @param device - The RTC descriptor.
 * @param tmr_cnt - Stores the current timer count value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_rtc_get_cnt(struct no_os_rtc_desc *device,
			  uint32_t *tmr_cnt)
{
	/* Check if device pointer or tmr_cnt is NULL */
	if (!device || !device->platform_ops)
		return -EINVAL;

	/* Check if Platform Specific get_cnt function is NULL */
	if (!device->platform_ops->get_cnt)
		return -ENOSYS;

	return device->platform_ops->get_cnt(device, tmr_cnt);
}

/**
 * @brief Stop the RTC device.
 * @param device - The RTC descriptor.
 * @param tmr_cnt - The starting timer count value to set.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_rtc_set_cnt(struct no_os_rtc_desc *device,
			  uint32_t tmr_cnt)
{
	/* Check if device pointer is NULL */
	if (!device || !device->platform_ops)
		return -EINVAL;

	/* Check if Platform Specific set_cnt function is NULL */
	if (!device->platform_ops->set_cnt)
		return -ENOSYS;

	return device->platform_ops->set_cnt(device, tmr_cnt);
}

/**
 * @brief Stop the RTC device.
 * @param device - The RTC descriptor.
 * @param irq_time - The time in seconds to set the IRQ.
 * @return 0 in case of success, negative error code otherwise.
 */

int32_t no_os_rtc_set_irq_time(struct no_os_rtc_desc *device, uint32_t irq_time)
{
	/* Check if device pointer is NULL */
	if (!device || !device->platform_ops)
		return -EINVAL;
	/* Check if Platform Specific set_irq_time function is NULL */
	if (!device->platform_ops->set_irq_time)
		return -ENOSYS;

	/* Check if irq_time is valid */
	if (irq_time < 0 || !irq_time)
		return -EINVAL;

	return device->platform_ops->set_irq_time(device, irq_time);
}
