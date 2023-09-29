/***************************************************************************//**
 *   @file   no_os_clk.c
 *   @brief  Implementation of Clock Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_clk.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * Initialize clock.
 * @param desc - CLK descriptor.
 * @param param - The structure that contains CLK parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_clk_init(struct no_os_clk_desc **desc,
		       const struct no_os_clk_init_param *param)
{
	struct no_os_clk_desc *clk;
	int ret;

	if (!desc || !param || !param->platform_ops)
		return -EINVAL;

	clk = (struct no_os_clk_desc *)no_os_calloc(1, sizeof(*clk));
	if (!clk)
		return -ENOMEM;

	clk->name = param->name;
	clk->hw_ch_num = param->hw_ch_num;
	clk->dev_desc = param->dev_desc;
	clk->platform_ops = param->platform_ops;

	if (param->platform_ops->init) {
		ret = param->platform_ops->init(desc, param);
		if (ret)
			goto error;
	}

	*desc = clk;

	return 0;

error:
	no_os_free(clk);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_clk_init().
 * @param desc - The clock descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_clk_remove(struct no_os_clk_desc *desc)
{
	int ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (desc->platform_ops->remove) {
		ret = desc->platform_ops->remove(desc);
		if (ret)
			return ret;
	}

	no_os_free(desc);

	return 0;
}

/**
 * Start the clock.
 * @param clk - The clock descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_clk_enable(struct no_os_clk_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->clk_enable)
		return -ENOSYS;

	return desc->platform_ops->clk_enable(desc);
}

/**
 * Stop the clock.
 * @param clk - The clock descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_clk_disable(struct no_os_clk_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->clk_disable)
		return -ENOSYS;

	return desc->platform_ops->clk_disable(desc);
}

/**
 * Get the current frequency of the clock.
 * @param clk - The clock descriptor.
 * @param rate - The current frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_clk_recalc_rate(struct no_os_clk_desc *desc,
			      uint64_t *rate)
{
	if (!desc || !desc->platform_ops || !rate)
		return -EINVAL;

	if (!desc->platform_ops->clk_recalc_rate)
		return -ENOSYS;

	return desc->platform_ops->clk_recalc_rate(desc, rate);
}

/**
 * Round the desired frequency to a rate that the clock can actually output.
 * @param clk - The clock descriptor.
 * @param rate - The desired frequency.
 * @param rounded_rate - The rounded frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_clk_round_rate(struct no_os_clk_desc *desc,
			     uint64_t rate,
			     uint64_t *rounded_rate)
{
	if (!desc || !desc->platform_ops || !rounded_rate)
		return -EINVAL;

	if (!desc->platform_ops->clk_round_rate)
		return -ENOSYS;

	return desc->platform_ops->clk_round_rate(desc, rate, rounded_rate);
}

/**
 * Change the frequency of the clock.
 * @param clk - The clock descriptor.
 * @param rate - The desired frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_clk_set_rate(struct no_os_clk_desc *desc,
			   uint64_t rate)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->clk_set_rate)
		return -ENOSYS;

	return desc->platform_ops->clk_set_rate(desc, rate);
}
