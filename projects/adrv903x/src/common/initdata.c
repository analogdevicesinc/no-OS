/***************************************************************************//**
 *   @file   initdata.c
 *   @brief  ADRV903X default PostMcsInit data structure.
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

#include "initdata.h"
#include "adi_adrv903x_radioctrl_types.h"

adi_adrv903x_PostMcsInit_t utilityInit = {
	.radioCtrlCfg = {
		.txRadioCtrlModeCfg = {
			.txEnableMode = ADI_ADRV903X_TX_EN_SPI_MODE,
			.txChannelMask = 0xFF,
		},
		.rxRadioCtrlModeCfg = {
			.rxEnableMode = ADI_ADRV903X_RX_EN_SPI_MODE,
			.rxChannelMask = 0xFF,
		},
		.orxRadioCtrlModeCfg = {
			.orxEnableMode = ADI_ADRV903X_ORX_EN_SPI_MODE,
			.orxChannelMask = 0x300,
		},
	},
	.radioCtrlGpioCfg = {
		.txEnMapping  = { 0, 0, 0, 0, 0, 0, 0, 0 },
		.txAltMapping = { 0, 0, 0, 0, 0, 0, 0, 0 },
		.rxEnMapping  = { 0, 0, 0, 0, 0, 0, 0, 0 },
		.rxAltMapping = { 0, 0, 0, 0, 0, 0, 0, 0 },
	},
	.radioCtrlTxRxEnPinSel = 0,
	.radioCtrlTxRxEnCfgSel = 0,
	.gpIntPostInit = {
		.gpInt0Mask = {
			.lowerMask = 0xFFFFFFFFFFFF,
			.upperMask = 0xFFFFFFFFFFFF,
		},
		.gpInt1Mask = {
			.lowerMask = 0xFFFFFFFFFFFF,
			.upperMask = 0xFFFFFFFFFFFF,
		},
	},
	.initCals = {
		.calMask        = 0x7E,
		.rxChannelMask  = 0xFF,
		.txChannelMask  = 0xFF,
		.orxChannelMask = 0x03,
		.warmBoot       = 0,
	},
};
