/***************************************************************************//**
 *   @file   no_os_clk_fixed.c
 *   @brief  Implementation of Fixed Rate Clock Driver based on no_os_clk
 *      framework.
 *   @author Jonathan Santos (jonathan.santos@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "no_os_clk_fixed.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

static int32_t fixed_clk_recalc_rate(struct no_os_clk_desc *hw, uint64_t *rate)
{
	struct no_os_clk_fixed_param *dev = hw->dev_desc;

	*rate = dev->clk_frequency;

	return 0;
}

static const struct no_os_clk_platform_ops fixed_clk_ops = {
	.clk_recalc_rate = fixed_clk_recalc_rate,
};

/**
 * Initialize clock.
 * @param desc - CLK descriptor.
 * @param param - The structure that contains CLK parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_clk_fixed_init(struct no_os_clk_desc **desc,
			     const struct no_os_clk_fixed_param *param)
{

	struct no_os_clk_init_param fixed_clk_ip;

	fixed_clk_ip.name = param->name;
	fixed_clk_ip.platform_ops = &fixed_clk_ops;
	fixed_clk_ip.dev_desc = param;

	return no_os_clk_init(desc, &fixed_clk_ip);
}
