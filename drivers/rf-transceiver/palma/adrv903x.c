/***************************************************************************//**
 *   @file   adrv903x.c
 *   @brief  ADRV903X (ADRV9030/ADRV9032) no-OS driver implementation.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders. This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adrv903x.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

/******************************************************************************/
/********************** Functions Implementation ******************************/
/******************************************************************************/

/**
 * @brief Initialize ADRV903X device.
 *
 * Allocates and initializes the device structure. Full device bring-up
 * (HwOpen, PreMcsInit, PostMcsInit) will be added in subsequent steps.
 *
 * @param phy       - Output pointer to the allocated device structure.
 * @param init_param - Initialization parameters.
 *
 * @return 0 on success, negative error code on failure.
 */
int adrv903x_init(struct adrv903x_rf_phy **phy,
		  const struct adrv903x_init_param *init_param)
{
	struct adrv903x_rf_phy *p;

	if (!phy || !init_param)
		return -EINVAL;

	p = no_os_calloc(1, sizeof(*p));
	if (!p)
		return -ENOMEM;

	p->hal = init_param->hal;
	p->dev_clk = init_param->dev_clk;
	p->palmaDevice = &p->adi_adrv903x_device;
	p->is_initialized = false;

	pr_info("ADRV903X: driver initialized (stub - hardware init not yet implemented)\n");

	*phy = p;
	return 0;
}

/**
 * @brief Free resources allocated by adrv903x_init().
 *
 * @param phy - Pointer to device structure.
 *
 * @return 0 on success, negative error code on failure.
 */
int adrv903x_remove(struct adrv903x_rf_phy *phy)
{
	if (!phy)
		return -EINVAL;

	no_os_free(phy);
	return 0;
}
