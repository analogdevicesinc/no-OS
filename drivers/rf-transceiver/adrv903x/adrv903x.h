/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ADRV903x
 *
 * Copyright 2020-2023 Analog Devices Inc.
 *
 * Licensed under the GPL-2.
 */

#ifndef IIO_TRX_ADRV903X_H_
#define IIO_TRX_ADRV903X_H_

#include "adi_adrv903x_types.h"
#include "adi_adrv903x_core.h"
#include "no_os_platform.h"
#include "no_os_mutex.h"
#include "no_os_clk.h"
#include <stdbool.h>
#include <stdint.h>

/*
 * JESD204-FSM defines
 */

#define DEFRAMER0_LINK_TX	0
#define DEFRAMER1_LINK_TX	1
#define FRAMER0_LINK_RX		2
#define FRAMER1_LINK_RX		3
#define FRAMER2_LINK_RX		4

enum adrv903x_clocks {
	ADRV903X_RX_SAMPL_CLK,
	ADRV903X_TX_SAMPL_CLK,
	ADRV903X_ORX_SAMPL_CLK,
	NUM_ADRV903X_CLKS,
};

struct adrv903x_rf_phy {
	struct no_os_spi_desc			*spi_desc;
	adi_adrv903x_Device_t			adi_adrv903x_device;
	adi_adrv903x_Device_t			*kororDevice;
	adi_adrv903x_SpiConfigSettings_t	spiSettings;
	adi_adrv903x_SpiOptions_t 		spiOptions;
	adi_adrv903x_Init_t			deviceInitStruct;
	adi_adrv903x_TrxFileInfo_t 		trxBinaryInfoPtr;
	adi_adrv903x_PostMcsInit_t		adrv903xPostMcsInitInst;
	struct adrv903x_hal_cfg			hal;

	struct jesd204_dev			*jdev;
	/* protect against device accesses */
	void					*lock;
	uint32_t				tx_iqRate_kHz;
	uint32_t				rx_iqRate_kHz;
	uint32_t				orx_iqRate_kHz;

	struct no_os_clk_desc			*dev_clk;
	struct no_os_clk_desc			*clks[NUM_ADRV903X_CLKS];

	uint8_t					device_id;
	bool					is_initialized;
	int					spi_device_id;

	struct axi_adc				*rx_adc;
	struct axi_dac				*tx_dac;
};

struct adrv903x_init_param {
	struct no_os_spi_init_param	*spi_init;
	struct no_os_clk_desc		*dev_clk;
	struct adi_adrv903x_Device	*adrv903x_device;
};

// /* Initialize the device. */
int32_t adrv903x_init(struct adrv903x_rf_phy **device,
		      const struct adrv903x_init_param *init_param);

int adrv903x_setup(struct adrv903x_rf_phy *phy);
int adrv903x_remove(struct adrv903x_rf_phy *phy);

adi_adrv903x_SpiConfigSettings_t *adrv903x_spi_settings_get(void);
adi_adrv903x_SpiOptions_t *adrv903x_spi_options_get(void);

int adrv903x_post_setup(struct adrv903x_rf_phy *phy);

#endif
