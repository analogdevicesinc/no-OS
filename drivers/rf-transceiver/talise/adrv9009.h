/***************************************************************************//**
 *   @file   adrv9009.h
 *   @brief  ADRV9009 initialization and control routines.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __ADRV9009_H__
#define __ADRV9009_H__

#include <stdbool.h>
#include <stdint.h>
#include "talise_cals_types.h"
#include "talise_agc_types.h"
#include "talise_types.h"
#include "talise_gpio.h"
#include "adi_hal.h"

/*
 * JESD204-FSM defines
 */

#define DEFRAMER_LINK_TX	1
#define FRAMER_LINK_RX		0
#define FRAMER_LINK_ORX		2

enum taliseDeviceId {
	TALISE_A = 0u,
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	TALISE_B,
#endif

	TALISE_DEVICE_ID_MAX
};

//#define MAX_NUM_GAIN_TABLES 10

enum adrv9009_clocks {
	RX_SAMPL_CLK,
	OBS_SAMPL_CLK,
	TX_SAMPL_CLK,
	NUM_ADRV9009_CLKS,
};

enum adrv9009_gain_tables {
	RX1_GT,
	RX2_GT,
	RX1_RX2_GT,
	ORX_RX1_GT,
	ORX_RX2_GT,
	ORX_RX1_RX2_GT,
	NUM_GT,
};

struct gain_table_info {
	uint64_t start;
	uint64_t end;
	uint8_t max_index;
	uint8_t dest;
	int32_t *abs_gain_tbl;
	taliseRxGainTable_t *gainTablePtr;
	taliseOrxGainTable_t *orx_gainTablePtr;
};

struct adrv9009_rf_phy {
	char				fw[32];
	char				stream[32];
	taliseDevice_t 			talise_device;
	taliseDevice_t 			*talDevice;
	taliseInit_t 			talInit;
	taliseAuxDac_t			auxdac;
	taliseAgcCfg_t 			rxAgcCtrl;
	taliseArmGpioConfig_t		arm_gpio_config;
	taliseRxGainCtrlPin_t		rx1_gain_ctrl_pin;
	taliseRxGainCtrlPin_t		rx2_gain_ctrl_pin;
	taliseTxAttenCtrlPin_t		tx1_atten_ctrl_pin;
	taliseTxAttenCtrlPin_t		tx2_atten_ctrl_pin;
	taliseTxPaProtectCfg_t		tx_pa_protection;
	taliseRxHd2Config_t		rx_hd2_config;
	uint32_t 			initCalMask;
	uint16_t			gpio3v3SrcCtrl;
	uint16_t 			gpio3v3PinLevel;
	uint16_t 			gpio3v3OutEn;
	int16_t 			rxFirCoefs[72];
	int16_t 			obsrxFirCoefs[72];
	int16_t 			txFirCoefs[80];
	int16_t 			current_loopBandwidth_kHz[2];
	uint8_t 			loopFilter_stability;
	uint64_t 			trx_lo_frequency;
	uint64_t 			aux_lo_frequency;
	struct adi_hal			hal;
	struct no_os_clk_desc 		*dev_clk;
	struct no_os_clk_desc 		*clk_ext_lo_rx;
	struct no_os_clk_desc 		*clk_ext_lo_tx;
	struct no_os_clk_desc 		*clks[NUM_ADRV9009_CLKS];
	struct jesd204_dev		*jdev;
	struct no_os_gpio_desc		*sysref_req_gpio;
	uint8_t 			device_id;
	uint32_t			tracking_cal_mask;
	bool				is_initialized;
	int				spi_device_id;
	uint32_t 			framer_b_m;
	uint32_t 			framer_b_f;
	uint32_t 			orx_channel_enabled;
	struct axi_adc			*rx_adc;
	struct axi_dac			*tx_dac;
	struct axi_adc			*rx_os_adc;
};

struct adrv9009_init_param {
	struct no_os_spi_init_param	*spi_init;
	struct no_os_clk_desc		*dev_clk;
	taliseDevice_t			*adrv9009_device;
	char				*streamImageFile;
	char				*armImageFile;
	taliseAgcCfg_t		  	*rxAgcConfig_init_param;
	uint64_t			trx_lo_frequency;
	taliseTxPaProtectCfg_t		tx_pa_protection;
};

#define TALISE_NUM_SUBCHANNELS		2 /* I - in-phase and Q - quadrature channels */
#define TALISE_NUM_CHAIN_CHANNELS	2 /* channels per RX/TX chain */
#define TALISE_NUM_CHANNELS		(TALISE_DEVICE_ID_MAX * TALISE_NUM_CHAIN_CHANNELS * TALISE_NUM_SUBCHANNELS)

bool adrv9009_check_sysref_rate(uint32_t lmfc, uint32_t sysref);

/* Initialize the device. */
int32_t adrv9009_init(struct adrv9009_rf_phy **device,
		      const struct adrv9009_init_param *init_param);
int adrv9009_setup(struct adrv9009_rf_phy *phy);

taliseSpiSettings_t *adrv9009_spi_settings_get(void);
taliseInit_t *adrv9009_initial_settings_get(void);

int adrv9009_post_setup(struct adrv9009_rf_phy *phy);

static inline bool has_tx(struct adrv9009_rf_phy *phy)
{
#ifndef ADRV9008_1
	return 1;
#else
	return 0;
#endif
}

static inline bool has_tx_and_en(struct adrv9009_rf_phy *phy)
{
	return has_tx(phy) && (phy->talInit.tx.txChannels != TAL_TXOFF);
}

static inline bool has_obs(struct adrv9009_rf_phy *phy)
{
	return has_tx(phy);
}

static inline bool has_obs_and_en(struct adrv9009_rf_phy *phy)
{
	return has_obs(phy) &&
	       (phy->talInit.obsRx.obsRxChannelsEnable != TAL_ORXOFF);
}

static inline bool has_rx(struct adrv9009_rf_phy *phy)
{
#ifndef ADRV9008_2
	return 1;
#else
	return 0;
#endif
}

static inline bool has_rx_and_en(struct adrv9009_rf_phy *phy)
{
	return has_rx(phy) && (phy->talInit.rx.rxChannels != TAL_RXOFF);
}

#endif /* __ADRV9009_H__ */
