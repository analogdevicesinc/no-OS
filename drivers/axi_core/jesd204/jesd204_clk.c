/***************************************************************************//**
 *   @file   jesd204_clk.c
 *   @brief  Analog Devices JESD204 clock.
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
#include "error.h"
#include "jesd204_clk.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Start the clock.
 * @param clk - The clock structure.
 * @return SUCCESS in case of success, negative error code otherwise.
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

	return SUCCESS;
}

/**
 * Stop the clock.
 * @param clk - The clock structure.
 * @return SUCCESS in case of success, negative error code otherwise.
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

	return SUCCESS;
}

/**
 * Change the frequency of the clock.
 * @param clk - The clock structure.
 * @param chan - The clock channel.
 * @param rate - The desired frequency.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t jesd204_clk_set_rate(struct jesd204_clk *clk, uint32_t chan,
			     uint32_t rate)
{
	if (clk->xcvr)
		return adxcvr_clk_set_rate(clk->xcvr, rate, clk->xcvr->ref_rate_khz);

	return SUCCESS;
}
