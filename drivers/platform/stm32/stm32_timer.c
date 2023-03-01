/***************************************************************************//**
 *   @file   stm32/stm32_timer.c
 *   @brief  Implementation of stm32 timer driver.
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
#include <stdlib.h>
#include <errno.h>
#include "no_os_util.h"
#include "no_os_timer.h"
#include "no_os_alloc.h"
#include "stm32_timer.h"


static int get_timer_base(uint32_t device_id, TIM_TypeDef **base,
			  uint32_t *clk_freq)
{
	uint32_t apb2_freq = HAL_RCC_GetPCLK2Freq();
	*clk_freq = HAL_RCC_GetPCLK1Freq();

	switch (device_id) {
#if defined(TIM1)
	case 1:
		*base = TIM1;
		*clk_freq = apb2_freq;
		break;
#endif
#if defined(TIM2)
	case 2:
		*base = TIM2;
		break;
#endif
#if defined(TIM3)
	case 3:
		*base = TIM3;
		break;
#endif
		break;
#if defined(TIM4)
	case 4:
		*base = TIM4;
		break;
#endif
#if defined(TIM5)
	case 5:
		*base = TIM5;
		break;
#endif
#if defined(TIM6)
	case 6:
		*base = TIM6;
		break;
#endif
#if defined(TIM7)
	case 7:
		*base = TIM7;
		break;
#endif
#if defined(TIM8)
	case 8:
		*base = TIM8;
		*clk_freq = apb2_freq;
		break;
#endif
#if defined(TIM9)
	case 9:
		*base = TIM9;
		break;
#endif
#if defined(TIM10)
	case 10:
		*base = TIM10;
		break;
#endif
#if defined(TIM11)
	case 11:
		*base = TIM11;
		break;
#endif
#if defined(TIM12)
	case 12:
		*base = TIM12;
		break;
#endif
#if defined(TIM13)
	case 13:
		*base = TIM13;
		break;
#endif
#if defined(TIM14)
	case 14:
		*base = TIM14;
		break;
#endif
	default:
		return -EINVAL;
	};

	return 0;
}

/**
 * @brief Initialize the timer peripheral.
 * @param desc  - The timer descriptor.
 * @param param - The structure that contains the timer parameters.
 * @return ret  - Result of the init procedure.
 */
int32_t stm32_timer_init(struct no_os_timer_desc **desc,
			 const struct no_os_timer_init_param *param)
{
	int32_t ret;
	struct no_os_timer_desc	*no_os_desc;
	struct stm32_timer_desc *stm_desc;
	uint32_t src_freq;
	TIM_TypeDef *base = NULL;

	if (!desc || !param)
		return -EINVAL;

	no_os_desc = (struct no_os_timer_desc *)no_os_calloc(1, sizeof(*no_os_desc));
	if (!no_os_desc)
		return -ENOMEM;

	stm_desc = param->extra;

	ret = get_timer_base(param->id, &base, &src_freq);
	if (ret)
		goto error;
	/* Make sure the same id is used as the selected instance */
	if (base != stm_desc->htimer->Instance) {
		ret = -EINVAL;
		goto error;
	}

	/* Make sure the selected frequency is in range */
	if (src_freq < param->freq_hz) {
		ret = -EINVAL;
		goto error;
	}

	/* Overwrite generated values with given values */
	stm_desc->htimer->Init.Prescaler = src_freq/param->freq_hz;
	stm_desc->htimer->Init.Period = param->ticks_count;

	ret = HAL_TIM_Base_Init(stm_desc->htimer);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	/* Copy settings to device descriptor */
	no_os_desc->extra = stm_desc->htimer;
	no_os_desc->id = param->id;
	no_os_desc->freq_hz = param->freq_hz;
	no_os_desc->ticks_count = param->ticks_count;
	*desc = no_os_desc;

	return 0;

error:
	no_os_free(no_os_desc);
	return ret;
}

/**
 * @brief Free the resources allocated by no_os_timer_init().
 * @param desc - The timer descriptor.
 *
 * @return ret - Result of the remove procedure.
 */
int32_t stm32_timer_remove(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (!HAL_TIM_Base_DeInit(desc->extra)) {
		no_os_free(desc);
		return 0;
	}

	return -1;
}

/**
 * @brief Start a timer.
 * @param desc - Pointer to the device handler.
 *
 * @return ret - Result of the start procedure.
 */
int32_t stm32_timer_start(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (!HAL_TIM_Base_Start_IT((TIM_HandleTypeDef*)desc->extra))
		return 0;

	return -1;
}

/**
 * @brief Stop a timer from counting.
 * @param desc - Pointer to the device handler.
 *
 * @return ret - Result of the stop procedure.
 */
int32_t stm32_timer_stop(struct no_os_timer_desc *desc)
{
	if (!desc)
		return -EINVAL;

	if (!HAL_TIM_Base_Stop_IT((TIM_HandleTypeDef*)desc->extra))
		return 0;

	return -1;
}

/**
 * @brief Get the value of the counter register for the timer.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] counter - Pointer to the counter value.
 * @return 0 in case of success, error code otherwise.
 */
int32_t stm32_timer_counter_get(struct no_os_timer_desc *desc,
				uint32_t *counter)
{
	if (!desc && !counter)
		return -EINVAL;

	*counter = __HAL_TIM_GetCounter((TIM_HandleTypeDef*)desc->extra);
	return 0;
}

/**
 * @brief Set the timer counter register value.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] new_val - The new value of the counter register.
 * @return 0 in case of success, error code otherwise.
 */
int32_t stm32_timer_counter_set(struct no_os_timer_desc *desc, uint32_t new_val)
{
	if (!desc || (new_val > desc->ticks_count))
		return -EINVAL;

	__HAL_TIM_SetCounter((TIM_HandleTypeDef*)desc->extra, new_val);
	return 0;
}

/**
 * @brief Get the timer clock frequency.
 * @param [in]  desc    - Pointer to the device handler.
 * @param [out] freq_hz - The value in Hz of the timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t stm32_timer_count_clk_get(struct no_os_timer_desc *desc,
				  uint32_t *freq_hz)
{
	if (!desc || !freq_hz)
		return -EINVAL;

	*freq_hz = desc->freq_hz;
	return 0;
}

/**
 * @brief Set the timer clock frequency.
 * @param [in] desc    - Pointer to the device handler.
 * @param [in] freq_hz - The value in Hz of the new timer clock.
 * @return 0 in case of success, error code otherwise.
 */
int32_t stm32_timer_count_clk_set(struct no_os_timer_desc *desc,
				  uint32_t freq_hz)
{
	int ret;
	uint32_t src_freq;
	TIM_TypeDef *base = NULL;
	TIM_HandleTypeDef* tim_desc;

	if (!desc)
		return -EINVAL;

	ret = get_timer_base(desc->id, &base, &src_freq);
	if (ret)
		return -EINVAL;

	if (src_freq < freq_hz)
		return -EINVAL;

	tim_desc = desc->extra;

	__HAL_TIM_SET_PRESCALER(tim_desc, src_freq/freq_hz);
	tim_desc->Init.Prescaler = src_freq/freq_hz;

	desc->freq_hz = freq_hz;

	return 0;
}

/**
 * @brief Get the elapsed time in nsec for the timer.
 * @param [in] desc         - Pointer to the device handler.
 * @param [in] elapsed_time - The elapsed time in nsec.
 * @return 0 in case of success, error code otherwise.
 */
int32_t stm32_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
		uint64_t *elapsed_time)
{
	/* Function not implemented because it is not needed at the moment */
	return -ENOSYS;
}

/**
 * @brief stm32 platform specific timer platform ops structure
 */
const struct no_os_timer_platform_ops stm32_timer_ops = {
	.init = &stm32_timer_init,
	.start = &stm32_timer_start,
	.stop = &stm32_timer_stop,
	.counter_get = &stm32_timer_counter_get,
	.counter_set = &stm32_timer_counter_set,
	.count_clk_get = &stm32_timer_count_clk_get,
	.count_clk_set = &stm32_timer_count_clk_set,
	.get_elapsed_time_nsec = &stm32_timer_get_elapsed_time_nsec,
	.remove = &stm32_timer_remove
};

