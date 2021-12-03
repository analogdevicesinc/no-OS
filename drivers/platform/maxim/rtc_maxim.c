#include <stdlib.h>
#include <errno.h>
#include "no-os/rtc.h"
#include "rtc.h"
#include "rtc_extra.h"
#include "rtc_regs.h"

static struct callback_desc *cb;

void RTC_IRQHandler()
{
	if(!cb)
		return;

	mxc_rtc_reg_t *rtc_regs = MXC_RTC;
	volatile uint32_t rtc_ctrl = rtc_regs->ctrl;
	uint8_t n_int = 0;
	/** Sub-second alarm flag clear */
	rtc_regs->ctrl &= ~BIT(7);
	/** Time-of-day alarm flag clear */
	rtc_regs->ctrl &= ~BIT(6);
	/** RTC (read) ready flag */
	rtc_regs->ctrl &= ~BIT(5);

	/** Shift right so the interrupt flags will be the first 3 bits */	
	rtc_ctrl >>= 5UL;
	/** Clear the remaining bits */
	rtc_ctrl &= 0x7UL;
	while(rtc_ctrl){
		if((rtc_ctrl & 1) && (rtc_regs->ctrl & BIT(n_int))){
			cb->callback(cb->ctx, n_int, ctx->config);
		}
		n_int++;
		rtc_ctrl >>= 1;
	}
}

/**
 * @brief Initialize the RTC peripheral.
 * @param device - The RTC descriptor.
 * @param init_param - The structure that contains the RTC initialization.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t rtc_init(struct rtc_desc **device, struct rtc_init_param *init_param)
{
	int32_t ret = 0;
	struct rtc_desc *dev;
	sys_cfg_rtc_t sys_cfg;

	if(!init_param)
		return -EINVAL;

	struct rtc_init_maxim *maxim_init_param = (struct rtc_init_maxim *)init_param->extra;
	struct rtc_desc_maxim *rtc_maxim = calloc(1, sizeof(*rtc_maxim));
	mxc_rtc_regs_t *rtc_regs = MXC_RTC;
	dev = calloc(1, sizeof(*dev));
	if(!dev || !rtc_maxim){
		ret = -ENOMEM;
		goto error;
	}

	rtc_maxim->ms = 0;
	rtc_maxim->rtc_regs = rtc_regs;

	dev->id = init_param->id;
	dev->freq = init_param->freq;
	dev->load = init_param->load;
	dev->extra = rtc_maxim;

	sys_cfg.tmr = MXC_TMR0;
	MXC_TMR0->cn |= MXC_F_TMR_CN_TEN;
	if(RTC_Init(MXC_RTC, dev->load, maxim_init_param->ms_load, &sys_cfg) != E_NO_ERROR) {
		ret = -1;
		goto error;
	}

	*device = dev;

	return 0;

error:
	free(rtc_maxim);
	free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by rtc_init().
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t rtc_remove(struct rtc_desc *dev)
{
	free(dev->extra);
	free(dev);
	return 0;
}

/**
 * @brief Start the real time clock.
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t rtc_start(struct rtc_desc *dev)
{
	uint32_t temp_cnt;
	struct rtc_desc_maxim *rtc_maxim = (struct rtc_desc_maxim *)dev->extra;

	if(!dev)
		return -EINVAL;
	
	RTC_EnableRTCE(rtc_maxim->rtc_regs);	
	
	/** Wait for synchronization */
	if(RTC_CheckBusy())
		return -EBUSY;	

	return 0;
}

/**
 * @brief Stop the real time clock.
 * @param dev - The RTC descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t rtc_stop(struct rtc_desc *dev)
{
	uint32_t temp_cnt;
	struct rtc_desc_maxim *rtc_maxim = (struct rtc_desc_maxim *)dev->extra;

	if(!dev)
		return -EINVAL;
	RTC_DisableRTCE(rtc_maxim->rtc_regs);	

	return 0;
}

/**
 * @brief Get the current count for the real time clock.
 * @param dev - The RTC descriptor.
 * @param tmr_cnt - Pointer where the read counter will be stored.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t rtc_get_cnt(struct rtc_desc *dev, uint32_t *tmr_cnt)
{
	uint32_t temp_cnt;
	struct rtc_desc_maxim *rtc_maxim = (struct rtc_desc_maxim *)dev->extra;

	if(!dev)
		return -EINVAL;

	/** Wait for ctrl rdy */	
	if(RTC_CheckBusy())
		return -EBUSY;
	
	*tmr_cnt = RTC_GetSecond();

	return 0;
}

/**
 * @brief Set the current count for the real time clock.
 * @param dev - The RTC descriptor.
 * @param tmr_cnt - New value of the timer counter.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t rtc_set_cnt(struct rtc_desc *dev, uint32_t tmr_cnt)
{
	struct rtc_desc_maxim *rtc_maxim = (struct rtc_desc_maxim *)dev->extra;
	int a = RTC_IsEnabled();
	if(!dev)
		return -EINVAL;

	rtc_maxim->rtc_regs->ctrl |= MXC_F_RTC_CTRL_WE;
	if(RTC_CheckBusy())
		return -EBUSY;

	rtc_maxim->rtc_regs->sec = tmr_cnt;
	if(RTC_CheckBusy())
		return -EBUSY;
	
	rtc_maxim->rtc_regs->ctrl &= ~MXC_F_RTC_CTRL_WE;

	return 0;
}

int32_t rtc_register_callback(struct callback_desc *desc)
{
	if(!desc || !desc->config)
		return -EINVAL;
	if(!cb){
		cb = calloc(1, sizeof(*cb));
		if(!cb)
			return -ENOMEM;
	}
	
	cb->ctx = desc->ctx;
	cb->callback = desc->callback;
	cb->config = desc->config;

	return 0;
}

int32_t rtc_unregister_callback()
{
	if(!cb)
		return -EINVAL;
	
	cb->ctx = NULL;
	cb->callback = NULL;
	cb->config = NULL;

	return 0;
}

