/***************************************************************************//**
 *   @file   maxim_timer.c
 *   @brief  Source file for Maxim timer platform driver.
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
/***************************** Include Files **********************************/
/******************************************************************************/
#include "tmr.h"
#include "timer_extra.h"
#include "no_os_error.h"
#include "no_os_timer.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/** For now only the external crystal will be used as a clock source */
#define SOURCE_CLOCK_FREQ       32768

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * @brief Convert the prescaler from a clock division value to
 * an enum's value for the init function.
 * @param div - clock division value (this should be a power of 2).
 * @param prescaler - enum value to be provided to the init function.
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int _get_prescaler(uint32_t div, mxc_tmr_mode_t *prescaler)
{
	int div_log;

	if (div > 4096)
		return -EINVAL;

	/** The prescaler can only be a power of 2 */
	if (no_os_hweight32(div) != 1)
		return -EINVAL;

	div_log = no_os_log_base_2(div);
	*prescaler = div_log << MXC_F_TMR_CTRL0_CLKDIV_A_POS;

	return 0;
}

/**
 * @brief Timer driver init function
 * @param desc - timer descriptor to be initialized
 * @param param - initialization parameter for the desc
 * @return 0 in case of success, negative errno error codes otherwise.
 */
int max_timer_init(struct no_os_timer_desc **desc,
		   struct no_os_timer_init_param *param)
{
	int ret;
	uint32_t prescaler;
	uint32_t timer_freq;
	mxc_tmr_cfg_t *cfg;
	mxc_tmr_regs_t *tmr_regs;
	struct no_os_timer_desc *descriptor;
	struct max_timer_param *pextra;

	if (!param || param->id >= MXC_CFG_TMR_INSTANCES)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	cfg = calloc(1, sizeof(*cfg));
	if (!cfg) {
		ret = -ENOMEM;
		goto free_descriptor;
	}

	descriptor->id = param->id;
	descriptor->freq_hz = param->freq_hz;
	descriptor->load_value = param->load_value;
	descriptor->platform_ops = param->platform_ops;
	pextra = param->extra;

	tmr_regs = MXC_TMR_GET_TMR(param->id);

	prescaler = SOURCE_CLOCK_FREQ / param->freq_hz;
	ret = _get_prescaler(prescaler, &prescaler);
	if (ret)
		goto free_cfg;

	cfg->bitMode = TMR_BIT_MODE_16A;
	cfg->mode = TMR_MODE_CONTINUOUS;
	cfg->clock = MXC_TMR_32K_CLK;
	cfg->cmp_cnt = descriptor->load_value;
	cfg->pol = 1;
	cfg->pres = prescaler;

	descriptor->extra = cfg;

	MXC_TMR_Shutdown(MXC_TMR_GET_TMR(param->id));
	ret = MXC_TMR_Init(MXC_TMR_GET_TMR(param->id), cfg, 0);
	if (ret) {
		ret = -EINVAL;
		goto free_cfg;
	}

	*desc = descriptor;

	return 0;

free_cfg:
	free(cfg);
free_descriptor:
	free(descriptor);

	return ret;
}

/**
 * @brief Timer driver remove function
 * @param desc - timer descriptor
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_remove(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_TMR_Shutdown(MXC_TMR_GET_TMR(desc->id));
	free(desc->extra);
	free(desc);

	return 0;
}

/**
 * @brief Timer count start function
 * @param desc - timer descriptor
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_start(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_TMR_Start(MXC_TMR_GET_TMR(desc->id));

	return 0;
}

/**
 * @brief Timer count stop function
 * @param desc - timer descriptor
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_stop(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_TMR_Stop(MXC_TMR_GET_TMR(desc->id));

	return 0;
}

/**
 * @brief Function to get the current timer counter value
 * @param desc - timer descriptor
 * @param counter - the timer counter value
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_counter_get(struct no_os_timer_desc *desc, uint32_t *counter)
{
	if (!desc)
		return -EINVAL;

	*counter = MXC_TMR_GetCount(MXC_TMR_GET_TMR(desc->id));

	return 0;
}

/**
 * @brief Function to set the timer counter value
 * @param desc - timer descriptor
 * @param new_val - timer counter value to be set
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_counter_set(struct no_os_timer_desc *desc, uint32_t new_val)
{
	if (!desc || new_val >= SOURCE_CLOCK_FREQ)
		return -EINVAL;

	MXC_TMR_SetCount(MXC_TMR_GET_TMR(desc->id), new_val);

	return 0;
}

/**
 * @brief Function to get the timer frequency
 * @param desc - timer descriptor
 * @param freq_hz - the timer frequency value
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max_timer_count_clk_get(struct no_os_timer_desc *desc, uint32_t *freq_hz)
{
	if (!desc)
		return -EINVAL;

	*freq_hz = desc->freq_hz;

	return 0;
}

/**
 * @brief Function to set the timer frequency.
 * @param desc - timer descriptor.
 * @param freq_hz - the timer frequency value to be set.
 * @return 0 in case of success, negative errno error codes otherwise.
 */
int max_timer_count_clk_set(struct no_os_timer_desc *desc, uint32_t freq_hz)
{
	int ret;
	uint32_t div;
	uint32_t prescaler;
	uint32_t counter;
	uint32_t is_enabled;
	mxc_tmr_cfg_t *cfg;

	if (!desc)
		return -EINVAL;

	is_enabled = (MXC_TMR_GET_TMR(desc->id)->ctrl0 & NO_OS_BIT(15));
	div = SOURCE_CLOCK_FREQ / freq_hz;
	ret = _get_prescaler(div, &prescaler);
	if (ret)
		return ret;

	cfg = desc->extra;
	cfg->pres = prescaler;

	ret = no_os_timer_counter_get(desc, &counter);
	if (ret)
		return ret;

	MXC_TMR_Shutdown(MXC_TMR_GET_TMR(desc->id));
	ret = MXC_TMR_Init(MXC_TMR_GET_TMR(desc->id), cfg, 0);
	if (ret)
		return -EINVAL;

	MXC_TMR_SetCount(MXC_TMR_GET_TMR(desc->id), counter);
	if (is_enabled)
		MXC_TMR_Start(MXC_TMR_GET_TMR(desc->id));

	desc->freq_hz = freq_hz;

	return 0;
}

/**
 * @brief Not implemented
 * @param desc - timer descriptor
 * @param elapsed_time - time in nanoseconds
 * @return -ENOSYS
 */
int max_get_elapsed_nsec(struct no_os_timer_desc *desc, uint64_t *elapsed_time)
{
	return -ENOSYS;
}

/**
 * @brief maxim platform specific timer platform ops structure
 */
const struct no_os_timer_platform_ops max_timer_ops = {
	.init = &max_timer_init,
	.start = &max_timer_start,
	.stop = &max_timer_stop,
	.counter_get = &max_timer_counter_get,
	.counter_set = &max_timer_counter_set,
	.count_clk_get = &max_timer_count_clk_get,
	.count_clk_set = &max_timer_count_clk_set,
	.get_elapsed_time_nsec = &max_get_elapsed_nsec,
	.remove = &max_timer_remove
};
