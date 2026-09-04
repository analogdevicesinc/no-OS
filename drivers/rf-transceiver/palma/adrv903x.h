/***************************************************************************//**
 *   @file   adrv903x.h
 *   @brief  ADRV903X (ADRV9030/ADRV9032) no-OS driver header.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/
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
	struct adrv903x_hal_cfg		hal;

	struct jesd204_dev		*jdev;
	struct no_os_clk_desc		*dev_clk;

	adi_adrv903x_PostMcsInit_t	*post_mcs_init;

	bool				is_initialized;
};

/**
 * @struct adrv903x_init_param
 * @brief  ADRV903X initialization parameters.
 */
struct adrv903x_init_param {
	struct no_os_clk_desc		*dev_clk;
	adi_adrv903x_PostMcsInit_t	*post_mcs_init;
	const char			*profile_file;
	const char			*cpu_fw_file;
	const char			*stream_file;
	const char			*rx_gain_table_file;
	uint32_t			rx_gain_table_mask;
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
