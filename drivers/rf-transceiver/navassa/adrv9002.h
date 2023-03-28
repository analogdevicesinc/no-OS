/***************************************************************************//**
 *   @file   adrv9002.h
 *   @brief  adrv9002 driver header.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#ifndef IIO_TRX_ADRV9002_H_
#define IIO_TRX_ADRV9002_H_

#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_clk.h"

#include "adi_common_log.h"
#include "adi_adrv9001_user.h"
#include "adi_adrv9001_cals_types.h"
#include "adi_adrv9001_fh_types.h"
#include "adi_adrv9001_radio_types.h"
#include "adi_adrv9001_rx_gaincontrol_types.h"
#include "adi_adrv9001_rxSettings_types.h"
#include "adi_adrv9001_ssi_types.h"

#include "no_os_platform.h"

#include "axi_adc_core.h"

#define ADRV_ADDRESS(port, chan)	((port) << 8 | (chan))
#define ADRV_ADDRESS_PORT(addr)		((addr) >> 8)
#define ADRV_ADDRESS_CHAN(addr)		((addr) & 0xFF)
#define ADRV9002_FH_HOP_SIGNALS_NR	2
#define ADRV9002_FH_TABLES_NR		2
#define ADRV9002_FH_BIN_ATTRS_CNT	(ADRV9002_FH_HOP_SIGNALS_NR * ADRV9002_FH_TABLES_NR)
#define ADRV9002_RX_MIN_GAIN_IDX       ADI_ADRV9001_RX_GAIN_INDEX_MIN
#define ADRV9002_RX_MAX_GAIN_IDX       ADI_ADRV9001_RX_GAIN_INDEX_MAX

enum {
	ADRV9002_CHANN_1,
	ADRV9002_CHANN_2,
	ADRV9002_CHANN_MAX,
};

struct adrv9002_fh_bin_table {
	/* max of 64 entries! */
	uint8_t bin_table[64];
};

#ifdef ADI_COMMON_VERBOSE
/*
 * Enable log if ADI_COMMON_VERBOSE is defined
 */
#define	adrv9002_log_enable(common)	\
	(common)->error.logEnable = true
#else
#define	adrv9002_log_enable(common)
#endif

enum ad900x_device_id {
	ID_ADRV9002,
	ID_ADRV9002_RX2TX2,
	ID_ADRV9003,
	ID_ADRV9003_RX2TX2,
};

enum adrv9002_clocks {
	RX1_SAMPL_CLK,
	RX2_SAMPL_CLK,
	TX1_SAMPL_CLK,
	TX2_SAMPL_CLK,
	TDD1_INTF_CLK,
	TDD2_INTF_CLK,
	NUM_ADRV9002_CLKS,
};

enum adrv9002_rx_ext_info {
	RX_QEC_FIC,
	RX_QEC_W_POLY,
	RX_AGC,
	RX_TRACK_BBDC,
	RX_HD2,
	RX_RSSI_CAL,
	RX_RFDC,
	RX_RSSI,
	RX_DECIMATION_POWER,
	RX_RF_BANDWIDTH,
	RX_POWERDOWN,
	RX_GAIN_CTRL_PIN_MODE,
	RX_ENSM_MODE,
	RX_NCO_FREQUENCY,
	RX_ADC_SWITCH,
	RX_BBDC,
};

enum adrv9002_tx_ext_info {
	TX_QEC,
	TX_LOL,
	TX_LB_PD,
	TX_PAC,
	TX_CLGC,
	TX_RF_BANDWIDTH,
	TX_POWERDOWN,
	TX_ATTN_CTRL_PIN_MODE,
	TX_ENSM_MODE,
	TX_NCO_FREQUENCY
};

#define api_call(phy, func, args...)	({				\
	int __ret = func((phy)->adrv9001, ##args);			\
									\
	if (__ret)							\
		__ret = adrv9002_dev_err(phy);	\
									\
	__ret;								\
})

struct adrv9002_clock {
	struct spi_device	*spi;
	struct adrv9002_rf_phy	*phy;
	unsigned long		rate;
	enum adrv9002_clocks	source;
};

struct adrv9002_chan {
	struct no_os_clk *clk;
	struct adrv9002_ext_lo *ext_lo;
	/*
	 * These values are in nanoseconds. They need to be converted with
	 * @adrv9002_chan_ns_to_en_delay() before passing them to the API.
	 */
	struct adi_adrv9001_ChannelEnablementDelays en_delays_ns;
	unsigned long rate;
	adi_adrv9001_ChannelState_e cached_state;
	adi_common_ChannelNumber_e number;
	adi_common_Port_e port;
	uint32_t power;
	int nco_freq;
	uint8_t idx;
	uint8_t enabled;;
};

struct adrv9002_rx_chan {
	struct adrv9002_chan channel;
	struct adi_adrv9001_GainControlCfg agc;
	struct adi_adrv9001_RxGainControlPinCfg *pin_cfg;
	struct no_os_gpio_desc *orx_gpio;
	uint8_t orx_en;
#ifdef CONFIG_DEBUG_FS
	struct adi_adrv9001_RxSsiTestModeCfg ssi_test;
#endif
};

struct adrv9002_tx_chan {
	struct adrv9002_chan channel;
	struct adi_adrv9001_TxAttenuationPinControlCfg *pin_cfg;
	uint8_t dac_boost_en;
#ifdef CONFIG_DEBUG_FS
	struct adi_adrv9001_TxSsiTestModeCfg ssi_test;
	uint8_t loopback;
#endif
};

struct adrv9002_gpio {
	struct adi_adrv9001_GpioCfg gpio;
	uint32_t signal;
};

#define to_clk_priv(_hw) container_of(_hw, struct adrv9002_clock, hw)

struct mutex {
	uint32_t unused;
};

struct adrv9002_chip_info {
	const char *cmos_profile;
	const char *lvd_profile;
	const char *name;
	uint32_t n_tx;
	bool rx2tx2;
};

struct adrv9002_ext_lo {
	struct no_os_clk *clk;
	unsigned short divider;
};

struct adrv9002_rf_phy {
	const struct adrv9002_chip_info *chip;
	struct spi_device		*spi;
	struct iio_dev			*indio_dev;
	struct no_os_gpio_desc		*reset_gpio;
	struct no_os_gpio_desc		*ssi_sync;
	struct mutex			lock;
	struct adrv9002_clock		clk_priv[NUM_ADRV9002_CLKS];
	/* each LO controls two ports (at least) */
	struct adrv9002_ext_lo		ext_los[ADRV9002_CHANN_MAX];
	char                        	profile_buf[350];
	size_t                      	profile_len;
	uint8_t				*stream_buf;
	uint16_t			stream_size;
	struct adrv9002_fh_bin_table	fh_table_bin_attr[ADRV9002_FH_BIN_ATTRS_CNT];
	adi_adrv9001_FhCfg_t		fh;
	struct adrv9002_rx_chan		rx_channels[ADRV9002_CHANN_MAX];
	struct adrv9002_tx_chan		tx_channels[ADRV9002_CHANN_MAX];
	struct adrv9002_chan		*channels[ADRV9002_CHANN_MAX * 2];
	struct adrv9002_gpio 		*adrv9002_gpios;
	struct adi_adrv9001_Device	adrv9001_device;
	struct adi_adrv9001_Device	*adrv9001;
	struct adrv9002_hal_cfg		hal;
	struct adi_adrv9001_Init	*curr_profile;
	struct adi_adrv9001_Init	profile;
	struct adi_adrv9001_InitCals	init_cals;
	uint32_t			n_clks;
	int				spi_device_id;
	int				ngpios;
	uint8_t				rx2tx2;
	/* ssi type of the axi cores - cannot really change at runtime */
	enum adi_adrv9001_SsiType	ssi_type;
	/*
	 * Tells if TX only profiles are valid. If not set, it means that TX1/TX2 SSI clocks are
	 * derived from RX1/RX2 which means that TX cannot be enabled if RX is not...
	 */
	uint8_t				tx_only;
#ifdef CONFIG_DEBUG_FS
	struct adi_adrv9001_SsiCalibrationCfg ssi_delays;
#endif
	struct axi_adc			*rx1_adc;
	struct axi_dac			*tx1_dac;
	struct axi_adc			*rx2_adc;
	struct axi_dac			*tx2_dac;
	struct axi_dmac			*rx1_dmac;
	struct axi_dmac			*tx1_dmac;
	struct axi_dmac			*rx2_dmac;
	struct axi_dmac			*tx2_dmac;
};

int adrv9002_post_setup(struct adrv9002_rf_phy *phy);
int adrv9002_setup(struct adrv9002_rf_phy *phy);
adi_adrv9001_SsiType_e adrv9002_axi_ssi_type_get(struct adrv9002_rf_phy *phy);

/* Main driver API's */
void adrv9002_en_delays_ns_to_arm(const struct adrv9002_rf_phy *phy,
				  const struct adi_adrv9001_ChannelEnablementDelays *d_ns,
				  struct adi_adrv9001_ChannelEnablementDelays *d);
void adrv9002_en_delays_arm_to_ns(const struct adrv9002_rf_phy *phy,
				  const struct adi_adrv9001_ChannelEnablementDelays *d,
				  struct adi_adrv9001_ChannelEnablementDelays *d_ns);
/* phy lock must be held before entering the API */
int adrv9002_channel_to_state(const struct adrv9002_rf_phy *phy,
			      struct adrv9002_chan *chann,
			      const adi_adrv9001_ChannelState_e state, const bool cache_state);
int adrv9002_init(struct adrv9002_rf_phy *phy,
		  struct adi_adrv9001_Init *profile);
int __adrv9002_dev_err(const struct adrv9002_rf_phy *phy, const char *function,
		       const int line);
#define adrv9002_dev_err(phy)	__adrv9002_dev_err(phy, __func__, __LINE__)

/* AXI core API's */
int adrv9002_register_axi_converter(struct adrv9002_rf_phy *phy);
int adrv9002_axi_interface_set(const struct adrv9002_rf_phy *phy,
			       const uint8_t n_lanes,
			       const bool cmos_ddr,
			       const int channel,
			       const bool tx);
int adrv9002_axi_intf_tune(const struct adrv9002_rf_phy *phy, const bool tx,
			   const int chann,
			   uint8_t *clk_delay, uint8_t *data_delay);
void adrv9002_axi_interface_enable(struct adrv9002_rf_phy *phy, const int chan,
				   const bool tx, const bool en);
int __maybe_unused adrv9002_axi_tx_test_pattern_cfg(struct adrv9002_rf_phy *phy,
		const int channel,
		const adi_adrv9001_SsiTestModeData_e data);
int adrv9002_post_init(struct adrv9002_rf_phy *phy);
int adrv9002_intf_test_cfg(const struct adrv9002_rf_phy *phy, const int chann,
			   const bool tx,
			   const bool stop);
int adrv9002_check_tx_test_pattern(const struct adrv9002_rf_phy *phy,
				   const int chann);
int adrv9002_intf_change_delay(const struct adrv9002_rf_phy *phy,
			       const int channel,
			       uint8_t clk_delay,
			       uint8_t data_delay, const bool tx);
uint32_t adrv9002_axi_dds_rate_get(const struct adrv9002_rf_phy *phy,
				   const int chan);

struct adi_adrv9001_SpiSettings *adrv9002_spi_settings_get(void);


static inline void adrv9002_sync_gpio_toogle(const struct adrv9002_rf_phy *phy)
{
	if (phy->rx2tx2) {
		/* toogle ssi sync gpio */
		no_os_gpio_set_value(phy->ssi_sync, 1);
		no_os_udelay(5000);
		no_os_gpio_set_value(phy->ssi_sync, 0);
	}
}
#endif
