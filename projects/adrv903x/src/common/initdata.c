/***************************************************************************//**
 *   @file   initdata.c
 *   @brief  ADRV903X default PostMcsInit data structure.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
