/***************************************************************************//**
 *   @file   adrv903x.h
 *   @brief  ADRV903X (ADRV9030/ADRV9032) no-OS driver header.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#ifndef ADRV903X_H_
#define ADRV903X_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adi_adrv903x_types.h"
#include "adi_adrv903x_utilities_types.h"
#include "adi_adrv903x_core.h"
#include "no_os_clk.h"
#include "no_os_platform.h"

struct jesd204_dev;

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define NUM_ADRV903X_CLKS	3

/* JESD204 link IDs — must match the topology in the basic_example */
#define DEFRAMER0_LINK_TX	0
#define FRAMER0_LINK_RX		2

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct adrv903x_rf_phy
 * @brief  ADRV903X RF PHY device structure.
 */
struct adrv903x_rf_phy {
	adi_adrv903x_Device_t		adi_adrv903x_device;
	adi_adrv903x_Device_t		*palmaDevice;
	adi_adrv903x_SpiConfigSettings_t spiSettings;
	adi_adrv903x_TrxFileInfo_t	trxBinaryInfoPtr;
	adi_adrv903x_Init_t		deviceInitStruct;
	adi_adrv903x_PostMcsInit_t	postMcsInitInst;
	struct adrv903x_hal_cfg		hal;

	struct jesd204_dev		*jdev;
	struct no_os_clk_desc		*dev_clk;
	struct no_os_clk_desc		*clks[NUM_ADRV903X_CLKS];

	bool				is_initialized;
};

/**
 * @struct adrv903x_init_param
 * @brief  ADRV903X initialization parameters.
 */
struct adrv903x_init_param {
	struct no_os_clk_desc		*dev_clk;
};

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/

/* Accessor functions defined in platform/xilinx/main.c */
adi_adrv903x_SpiConfigSettings_t *adrv903x_spi_settings_get(void);
adi_adrv903x_SpiOptions_t *adrv903x_spi_options_get(void);

int adrv903x_init(struct adrv903x_rf_phy **phy,
		  const struct adrv903x_init_param *init_param);
int adrv903x_remove(struct adrv903x_rf_phy *phy);

#endif /* ADRV903X_H_ */
