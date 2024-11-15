/***************************************************************************//**
 *   @file   no_os_clk.c
 *   @brief  Implementation of Clock Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
