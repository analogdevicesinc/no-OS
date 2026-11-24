/***************************************************************************//**
 *   @file   adrv910x.h
 *   @brief  adrv910x driver header.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#ifndef IIO_TRX_ADRV910X_H_
#define IIO_TRX_ADRV910X_H_

#include "adi_adrv910x_types.h"
#include "adi_adrv910x_arm_types.h"
#include "adi_common_types.h"
#include "device_profile_top_t.h"
#include "adi_adrv910x_cals_types.h"
#include "device_profile_init_cals_t.h"
#include "adi_adrv910x_rx_types.h"
#include "adi_adrv910x_tx_types.h"
#include "device_profile_pfir_buffer_t.h"
#include "device_profile_channels_t.h"
#include "adi_adrv910x_radio_types.h"
#include "adi_adrv910x_ssi_types.h"
#include "device_profile_ssi_t.h"

#include "no_os_platform.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_clk.h"

#define api_call(phy, func, args...)	({				\
	int __ret = func((phy)->adrv910x, ##args);			\
									\
	if (__ret)							\
		__ret = adrv910x_dev_err(phy);	\
									\
	__ret;								\
})

enum {
	ADRV910X_RX1,
	ADRV910X_RX2,
	ADRV910X_RX_MAX,
	ADRV910X_TX = ADRV910X_RX_MAX,
	ADRV910X_CHAN_MAX
};

/* RF port bookkeeping, mirrors struct adrv9104_chan of the Linux driver */
struct adrv910x_chan {
	uint64_t carrier_hz;
	uint32_t rate;
	adi_common_Port_e port;
	adi_common_ChannelNumber_e number;
	LoId_e lo;
	bool enabled;
	uint8_t idx;
};

struct adrv910x_rx {
	struct adrv910x_chan channel;
	adi_adrv910x_GainControlCfg_t agc;
};

struct adrv910x_tx {
	struct adrv910x_chan channel;
	bool dac_boost_en;
	bool txnb;
};

struct adrv910x_rf_phy {
	struct spi_device *spi;
	struct no_os_gpio_desc *reset_gpio;
	struct no_os_clk_desc *dev_clk;

	adi_adrv910x_Device_t adrv910x_device;
	adi_adrv910x_Device_t *adrv910x;
	struct adrv910x_hal_cfg hal;

	/* Profile and calibration configuration */
	adi_adrv910x_Profiles_t *device_profile;
	uint8_t *arm_binary;
	uint32_t arm_binary_size;
	initCals_t init_cals;
	trackingCals_t tracking_cals;
	pfirBuffer_t pfbuf;

	adi_adrv910x_RxGainTableRow_t *rx_gain_table;
	adi_adrv910x_TxAttenTableRow_t *tx_atten_table;

	/* RF ports, enabled according to the profile channel mask */
	struct adrv910x_chan *channels[ADRV910X_CHAN_MAX];
	struct adrv910x_rx rx_channels[ADRV910X_RX_MAX];
	struct adrv910x_tx tx_channel;
	/* SSI type of the enabled ports, taken from the profile */
	ssiType_e ssi_type;
	/* Re-run the init calibrations when a carrier moves 100 MHz or more */
	bool rerun_cals;

	/* Configuration flags */
	bool profile_loaded;
	bool channels_configured;
	bool calibrations_run;

	/* Clock configuration */
	uint32_t dev_clk_frequency_hz;
	adi_adrv910x_DeviceClockDivisor_e clk_out_divisor;
};

struct adrv910x_init_param {
	struct no_os_clk_desc *dev_clk;
	struct no_os_gpio_desc *reset_gpio_init;
	struct spi_device *spi;
	adi_adrv910x_Profiles_t *device_profile;
	uint8_t *arm_binary_ptr;
	uint32_t arm_binary_size;
	uint32_t dev_clk_frequency_hz;
	adi_adrv910x_DeviceClockDivisor_e clk_out_divisor;
	adi_adrv910x_RxGainTableRow_t *rx_gain_table;
	adi_adrv910x_TxAttenTableRow_t *tx_atten_table;
	pfirBuffer_t pfbuf;
	/*
	 * Per port settings (devicetree properties in the Linux driver). All
	 * optional: a carrier of 0 selects the default one (2.4 GHz for RX,
	 * 2.45 GHz for TX) and a NULL AGC selects the default AGC settings.
	 */
	uint64_t rx_carrier_hz[ADRV910X_RX_MAX];
	uint64_t tx_carrier_hz;
	const adi_adrv910x_GainControlCfg_t *rx_agc[ADRV910X_RX_MAX];
	bool tx_dac_boost_en;
	bool rerun_cals_on_lo_retune;
};

int __adrv910x_dev_err(const struct adrv910x_rf_phy *phy, const char *function,
		       const int line);

#define adrv910x_dev_err(phy)	__adrv910x_dev_err(phy, __func__, __LINE__)

/* Initialize the device from the init parameters. */
int adrv910x_init(struct adrv910x_rf_phy *phy,
		  const struct adrv910x_init_param *init_param);
int adrv910x_init_with_profile(struct adrv910x_rf_phy *phy,
			       adi_adrv910x_Profiles_t *profile,
			       const pfirBuffer_t *pfbuf);
int adrv910x_channel_to_state_cache(struct adrv910x_rf_phy *phy,
				    const struct adrv910x_chan *chan,
				    adi_adrv910x_ChannelState_e state,
				    adi_adrv910x_ChannelState_e *curr);
int adrv910x_channel_to_state(struct adrv910x_rf_phy *phy,
			      const struct adrv910x_chan *chan,
			      adi_adrv910x_ChannelState_e state);

/*
 * Runtime API, the counterpart of the Linux IIO attributes. 'port' is one of
 * ADRV910X_RX1, ADRV910X_RX2 or ADRV910X_TX and must be enabled by the profile.
 */
int adrv910x_hardware_gain_set(struct adrv910x_rf_phy *phy, uint32_t port,
			       int32_t gain_mdb);
int adrv910x_hardware_gain_get(struct adrv910x_rf_phy *phy, uint32_t port,
			       int32_t *gain_mdb);
int adrv910x_sampling_frequency_get(struct adrv910x_rf_phy *phy, uint32_t port,
				    uint32_t *rate_hz);
int adrv910x_carrier_freq_set(struct adrv910x_rf_phy *phy, uint32_t port,
			      uint64_t freq_hz);
int adrv910x_carrier_freq_get(struct adrv910x_rf_phy *phy, uint32_t port,
			      uint64_t *freq_hz);
int adrv910x_rf_bandwidth_get(struct adrv910x_rf_phy *phy, uint32_t port,
			      uint32_t *bw_hz);
int adrv910x_rssi_get(struct adrv910x_rf_phy *phy, uint32_t port,
		      uint32_t *rssi_mdb);
int adrv910x_track_cal_get(struct adrv910x_rf_phy *phy, uint32_t port,
			   uint32_t mask, bool *enable);
int adrv910x_track_cal_set(struct adrv910x_rf_phy *phy, uint32_t port,
			   uint32_t mask, bool enable);
int adrv910x_port_en_mode_get(struct adrv910x_rf_phy *phy, uint32_t port,
			      adi_adrv910x_ChannelEnableMode_e *mode);
int adrv910x_port_en_mode_set(struct adrv910x_rf_phy *phy, uint32_t port,
			      adi_adrv910x_ChannelEnableMode_e mode);
int adrv910x_ensm_mode_get(struct adrv910x_rf_phy *phy, uint32_t port,
			   adi_adrv910x_ChannelState_e *state);
int adrv910x_ensm_mode_set(struct adrv910x_rf_phy *phy, uint32_t port,
			   adi_adrv910x_ChannelState_e state);
int adrv910x_gain_control_mode_get(struct adrv910x_rf_phy *phy, uint32_t port,
				   adi_adrv910x_RxGainControlMode_e *mode);
int adrv910x_gain_control_mode_set(struct adrv910x_rf_phy *phy, uint32_t port,
				   adi_adrv910x_RxGainControlMode_e mode);
int adrv910x_digital_gain_control_mode_get(struct adrv910x_rf_phy *phy,
		uint32_t port, adi_adrv910x_RxInterfaceGainCtrlMode_e *mode);
int adrv910x_digital_gain_control_mode_set(struct adrv910x_rf_phy *phy,
		uint32_t port, adi_adrv910x_RxInterfaceGainCtrlMode_e mode);
int adrv910x_atten_control_mode_get(struct adrv910x_rf_phy *phy, uint32_t port,
				    adi_adrv910x_TxAttenuationControlMode_e *mode);
int adrv910x_atten_control_mode_set(struct adrv910x_rf_phy *phy, uint32_t port,
				    adi_adrv910x_TxAttenuationControlMode_e mode);
int adrv910x_reg_read(struct adrv910x_rf_phy *phy, uint16_t reg, uint8_t *val);
int adrv910x_reg_write(struct adrv910x_rf_phy *phy, uint16_t reg, uint8_t val);

/* Counterparts of the Linux debugfs entries */
int adrv910x_api_version_get(struct adrv910x_rf_phy *phy,
			     adi_common_ApiVersion_t *version);
int adrv910x_rx_agc_get(struct adrv910x_rf_phy *phy, uint32_t port,
			adi_adrv910x_GainControlCfg_t *agc);
int adrv910x_ssi_delays_set(struct adrv910x_rf_phy *phy,
			    adi_adrv910x_SsiCalibrationCfg_t *delays);
int adrv910x_ssi_delays_get(struct adrv910x_rf_phy *phy,
			    adi_adrv910x_SsiCalibrationCfg_t *delays);
int adrv910x_load_profile(struct adrv910x_rf_phy *phy, deviceProfile_t *profile);
int adrv910x_configure_channels(struct adrv910x_rf_phy *phy);
int adrv910x_run_init_calibrations(struct adrv910x_rf_phy *phy);
int adrv910x_setup_tracking_calibrations(struct adrv910x_rf_phy *phy);
int adrv910x_setup(struct adrv910x_rf_phy *phy);

struct adi_adrv910x_SpiSettings *adrv910x_spi_settings_get(void);

#endif
