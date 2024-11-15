/***************************************************************************//**
 *   @file   jesd204_clk.c
 *   @brief  Analog Devices JESD204 clock.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_clk.h"
#include "jesd204_clk.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Start the clock.
 * @param clk - The clock structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t jesd204_clk_enable(struct jesd204_clk *clk)
{
	int32_t ret;

	if (clk->xcvr) {
		ret = adxcvr_clk_enable(clk->xcvr);
		if (ret)
			return ret;
	}

	if (clk->jesd_rx) {
		ret = axi_jesd204_rx_lane_clk_enable(clk->jesd_rx);
		if (ret)
			return ret;
	}

	if (clk->jesd_tx) {
		ret = axi_jesd204_tx_lane_clk_enable(clk->jesd_tx);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * Stop the clock.
 * @param clk - The clock structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t jesd204_clk_disable(struct jesd204_clk *clk)
{
	int32_t ret;

	if (clk->xcvr) {
		ret = adxcvr_clk_disable(clk->xcvr);
		if (ret)
			return ret;
	}

	if (clk->jesd_rx) {
		ret = axi_jesd204_rx_lane_clk_disable(clk->jesd_rx);
		if (ret)
			return ret;
	}

	if (clk->jesd_tx) {
		ret = axi_jesd204_tx_lane_clk_disable(clk->jesd_tx);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * Change the frequency of the clock.
 * @param clk - The clock structure.
 * @param chan - The clock channel.
 * @param rate - The desired frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t jesd204_clk_set_rate(struct jesd204_clk *clk, uint32_t chan,
			     uint32_t rate)
{
	if (clk->xcvr)
		return adxcvr_clk_set_rate(clk->xcvr, rate, clk->xcvr->ref_rate_khz);

	return 0;
}

/**
 * @brief Initialize the CLK structure.
 *
 * @param desc - The CLK descriptor.
 * @param init_param - The structure holding the device initial parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int jesd204_no_os_clk_init(struct no_os_clk_desc **desc,
				  const struct no_os_clk_init_param *init_param)
{
	struct jesd204_clk  *jesd204_clk_d;

	/* Exit if we have no init_params */
	if (!init_param) {
		return -EINVAL;
	}

	*desc = no_os_calloc(1, sizeof(**desc));
	/* Exit if memory cannot be allocated */
	if(!*desc)
		goto error;

	jesd204_clk_d = init_param->dev_desc;
	/* Exit if no hardware device specified in init_param */
	if(!jesd204_clk_d)
		goto error;

	(*desc)->name = init_param->name;
	(*desc)->hw_ch_num = init_param->hw_ch_num;

	(*desc)->dev_desc = (void *)no_os_calloc(1, sizeof(struct jesd204_clk));

	(*desc)->dev_desc = init_param->dev_desc;

	return 0;
error:
	free(*desc);
	return -ENOMEM;
}

/**
 * @brief Remove the CLK structure.
 *
 * @param desc - The CLK descriptor.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int jesd204_no_os_clk_remove(struct no_os_clk_desc *desc)
{
	struct jesd204_clk  *jesd204_clk_dev;

	jesd204_clk_dev = desc->dev_desc;

	if(!jesd204_clk_dev)
		return -ENODEV;

	free(desc);

	return 0;
}

/*
 * @brief Start the clock.
 *
 * @param desc - The CLK descriptor.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int jesd204_no_os_clk_enable(struct no_os_clk_desc *desc)
{
	return jesd204_clk_enable(desc->dev_desc);
}

/*
 * @brief Stop the clock.
 *
 * @param desc - The CLK descriptor.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int jesd204_no_os_clk_disable(struct no_os_clk_desc *desc)
{
	return jesd204_clk_disable(desc->dev_desc);
}

/*
 * @brief Set clock rate.
 *
 * @param desc - The CLK descriptor.
 * @param rate - Clock rate.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int jesd204_no_os_clk_set_rate(struct no_os_clk_desc *desc,
				      uint64_t rate)
{
	return jesd204_clk_set_rate(desc->dev_desc, 0, (uint32_t)rate);
}

/*
 * @brief jesd204 platform specific CLK platform ops structure
 */
const struct no_os_clk_platform_ops jesd204_clk_ops = {
	.init = &jesd204_no_os_clk_init,
	.clk_enable = &jesd204_no_os_clk_enable,
	.clk_disable = &jesd204_no_os_clk_disable,
	.clk_set_rate = &jesd204_no_os_clk_set_rate,
	.remove = &jesd204_no_os_clk_remove
};
