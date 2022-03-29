/***************************************************************************//**
 *   @file   maxim/maxim_rtc.c
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
#include "rtc.h"
#include "rtc_extra.h"
#include "rtc_reva_regs.h"
#include "mxc_errors.h"

#define MS_TO_RSSA(x) (0 - ((x * 256) / 1000))

/**
* @brief Callback descriptor that contains a function to be called when an
* interrupt occurs.
*/
static struct no_os_callback_desc *cb;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

void RTC_IRQHandler()
{
	mxc_rtc_regs_t *rtc_regs;
	volatile uint32_t rtc_ctrl;
	uint8_t n_int;

	rtc_regs = MXC_RTC;
	rtc_ctrl = rtc_regs->ctrl;
	/** Sub-second alarm flag clear */
	rtc_regs->ctrl &= ~NO_OS_BIT(7);
	/** Time-of-day alarm flag clear */
	rtc_regs->ctrl &= ~NO_OS_BIT(6);
	/** RTC (read) ready flag */
	rtc_regs->ctrl &= ~NO_OS_BIT(5);

	if(!cb)
		return;

	/** Shift right so the interrupt flags will be the first 3 bits */
	rtc_ctrl >>= 5UL;
	/** Clear the remaining bits */
	rtc_ctrl &= 0x7UL;
	while(rtc_ctrl) {
		n_int = no_os_find_first_set_bit(rtc_ctrl);
		if (rtc_ctrl & (rtc_regs->ctrl & NO_OS_BIT(n_int))) {
			cb->callback(cb->ctx, n_int, cb->config);
		}
		rtc_ctrl >>= n_int + 1;
	}
}

/**
 * @brief Initialize the RTC peripheral.
 * @param device - The RTC descriptor.
 * @param init_param - The structure that contains the RTC initialization.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_init(struct rtc_desc **device,
		       struct rtc_init_param *init_param)
{
	int32_t ret;
	struct rtc_desc *dev;

	if(!init_param)
		return -EINVAL;

	dev = calloc(1, sizeof(*dev));
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
	free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_rtc_init().
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_remove(struct rtc_desc *dev)
{
	if (!dev)
		return -EINVAL;

	free(dev);

	return 0;
}

/**
 * @brief Start the real time clock.
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_start(struct rtc_desc *dev)
{
	MXC_RTC_Start();

	/** Wait for synchronization */
	if (MXC_RTC_CheckBusy())
		return -EBUSY;

	return 0;
}

/**
 * @brief Stop the real time clock.
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_stop(struct rtc_desc *dev)
{
	int32_t ret;

	ret = MXC_RTC_Stop();
	if (ret == E_BUSY)
		return -EBUSY;

	return 0;
}

/**
 * @brief Get the current count for the real time clock.
 * @param dev - The RTC descriptor.
 * @param tmr_cnt - Pointer where the read counter will be stored.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_get_cnt(struct rtc_desc *dev, uint32_t *tmr_cnt)
{
	if (MXC_RTC_CheckBusy())
		return -EBUSY;

	*tmr_cnt = MXC_RTC_GetSecond();

	return 0;
}

/**
 * @brief Set the current count for the real time clock.
 * @param dev - The RTC descriptor.
 * @param tmr_cnt - New value of the timer counter.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t no_os_rtc_set_cnt(struct rtc_desc *dev, uint32_t tmr_cnt)
{
	mxc_rtc_regs_t *rtc_regs;

	if (!dev)
		return-EINVAL;

	rtc_regs = MXC_RTC;

	if (MXC_RTC_CheckBusy())
		return -EBUSY;

	rtc_regs->ctrl |= MXC_F_RTC_REVA_CTRL_WR_EN;
	no_os_rtc_stop(dev);

	if (MXC_RTC_CheckBusy())
		return -EBUSY;

	rtc_regs->sec = tmr_cnt;
	no_os_rtc_start(dev);

	if (MXC_RTC_CheckBusy())
		return -EBUSY;

	rtc_regs->ctrl &= ~MXC_F_RTC_REVA_CTRL_WR_EN;

	return 0;
}

static int32_t max_rtc_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
				     const struct no_os_irq_init_param *param)
{
	struct no_os_irq_ctrl_desc *descriptor;

	if (!param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));

	if (!descriptor)
		return -ENOMEM;

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->platform_ops = param->platform_ops;
	descriptor->extra = param->extra;

	*desc = descriptor;

	return 0;
}

static int32_t max_rtc_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	if (!desc)
		return -EINVAL;

	free(desc);
}

/**
 * @brief Register a function to be called when an interrupt occurs.
 * @param dev - the callback descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_rtc_register_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, struct no_os_callback_desc *callback_desc)
{
	struct max_rtc_alarm_desc *alarm_desc;

	if (!desc || !callback_desc || !callback_desc->config)
		return -EINVAL;

	if (!cb) {
		cb = calloc(1, sizeof(*cb));
		if (!cb)
			return -ENOMEM;
	}

	alarm_desc = callback_desc->config;

	if (irq_id == RTC_TIMEOFDAY_INT)
		MXC_RTC_SetTimeofdayAlarm(alarm_desc->irq_time);
	else if (irq_id == RTC_SUBSEC_INT)
		MXC_RTC_SetSubsecondAlarm(MS_TO_RSSA(alarm_desc->irq_time));
	else {
		free(cb);
		return -EINVAL;
	}

	cb->ctx = callback_desc->ctx;
	cb->callback = callback_desc->callback;
	cb->config = callback_desc->config;

	return 0;
}

/**
 * @brief Unregister a callback function.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_rtc_unregister_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id)
{
	if (!cb)
		return -EINVAL;

	free(cb);
	cb = NULL;

	return 0;
}

/**
 * @brief Enable a specific interrupt.
 * @param int_id - the interrupt identifier.
 * @param irq_time - the time at which the interrupt must occur (one-shot).
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_rtc_enable_irq(struct no_os_irq_ctrl_desc *desc,
				  uint32_t irq_id)
{
	if (irq_id == RTC_TIMEOFDAY_INT)
		MXC_RTC_EnableInt(MXC_RTC_INT_EN_LONG);
	else if (irq_id == RTC_SUBSEC_INT)
		MXC_RTC_EnableInt(MXC_RTC_INT_EN_SHORT);
	else
		return -EINVAL;

	return 0;
}

/**
 * @brief Disable a specific interrupt.
 * @param int_id - the interrupt identifier.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_rtc_disable_irq(struct no_os_irq_ctrl_desc *desc,
				   uint32_t irq_id)
{
	if (irq_id == RTC_TIMEOFDAY_INT)
		MXC_RTC_DisableInt(MXC_RTC_INT_EN_LONG);
	else if (irq_id == RTC_SUBSEC_INT)
		MXC_RTC_DisableInt(MXC_RTC_INT_EN_SHORT);
	else
		return -EINVAL;

	return 0;
}

const struct no_os_irq_platform_ops max_rtc_irq_ops = {
	.init = &max_rtc_irq_ctrl_init,
	.remove = &max_rtc_irq_ctrl_remove,
	.register_callback = &max_rtc_register_callback,
	.unregister = &max_rtc_unregister_callback,
	.enable = &max_rtc_enable_irq,
	.disable = &max_rtc_disable_irq
};
