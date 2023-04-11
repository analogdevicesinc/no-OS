/***************************************************************************//**
 *   @file   maxim_rtc.c
 *   @brief  Implementation of RTC driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <stdlib.h>
#include <errno.h>
#include "no_os_irq.h"
#include "no_os_rtc.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "rtc.h"
#include "maxim_rtc.h"
#include "rtc_reva_regs.h"
#include "mxc_errors.h"

#define MS_TO_RSSA(x) (0 - ((x * 256) / 1000))

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the RTC peripheral.
 * @param device - The RTC descriptor.
 * @param init_param - The structure that contains the RTC initialization.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_init(struct no_os_rtc_desc **device,
		       struct no_os_rtc_init_param *init_param)
{
	int32_t ret;
	struct no_os_rtc_desc *dev;

	if(!init_param)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->id = init_param->id;
	dev->freq = init_param->freq;
	dev->load = init_param->load;
	dev->extra = init_param->extra;

	if (MXC_RTC_Init(dev->load, 0) != E_NO_ERROR) {
		ret = -EINVAL;
		goto error;
	}

	*device = dev;

	return 0;

error:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_rtc_init().
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_remove(struct no_os_rtc_desc *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Start the real time clock.
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_start(struct no_os_rtc_desc *dev)
{
	while(MXC_RTC_GetBusyFlag());
	MXC_RTC_Start();

	return 0;
}

/**
 * @brief Stop the real time clock.
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_stop(struct no_os_rtc_desc *dev)
{
	while(MXC_RTC_GetBusyFlag());
	MXC_RTC_Stop();

	return 0;
}

/**
 * @brief Get the current count for the real time clock.
 * @param dev - The RTC descriptor.
 * @param tmr_cnt - Pointer where the read counter will be stored.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_get_cnt(struct no_os_rtc_desc *dev, uint32_t *tmr_cnt)
{
	*tmr_cnt = MXC_RTC_GetSecond();

	return 0;
}

/**
 * @brief Set the current count for the real time clock.
 * @param dev - The RTC descriptor.
 * @param tmr_cnt - New value of the timer counter.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_set_cnt(struct no_os_rtc_desc *dev, uint32_t tmr_cnt)
{
	mxc_rtc_regs_t *rtc_regs;

	if (!dev)
		return-EINVAL;

	rtc_regs = MXC_RTC;

	while(MXC_RTC_GetBusyFlag());
	rtc_regs->ctrl |= MXC_F_RTC_REVA_CTRL_WR_EN;

	while(MXC_RTC_GetBusyFlag());
	no_os_rtc_stop(dev);

	while(MXC_RTC_GetBusyFlag());
	rtc_regs->sec = tmr_cnt;

	while(MXC_RTC_GetBusyFlag());
	no_os_rtc_start(dev);

	while(MXC_RTC_GetBusyFlag());
	rtc_regs->ctrl &= ~MXC_F_RTC_REVA_CTRL_WR_EN;

	return 0;
}

/**
 * @brief Set the current count for the real time clock.
 * @param dev - The RTC descriptor.
 * @param irq_time - The number of seconds at which the interrupt must occur.
 * @return 0
 */
int32_t no_os_rtc_set_irq_time(struct no_os_rtc_desc *dev, uint32_t irq_time)
{
	MXC_RTC_SetTimeofdayAlarm(irq_time);

	return 0;
}
