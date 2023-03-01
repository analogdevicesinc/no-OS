/***************************************************************************//**
 *   @file   iio_ad9361.c
 *   @brief  Implementation of iio_ad9361
 *   This module is a wrapper over "ad9361_api", and it is used by "iio".
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include <sys/types.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include "no_os_error.h"
#include "iio_ad9361.h"
#include "ad9361_api.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * Calibration modes.
 */
static const char * const ad9361_calib_mode[] =
{"auto", "manual", "tx_quad", "rf_dc_offs", "rssi_gain_step"};

/**
 * Receive channel port selection.
 */
static const char * const ad9361_rf_rx_port[] = {
	"A_BALANCED", "B_BALANCED", "C_BALANCED",
	"A_N", "A_P", "B_N", "B_P", "C_N", "C_P", "TX_MONITOR1",
	"TX_MONITOR2", "TX_MONITOR1_2"
};

/**
 * Transmit channel port selection.
 */
static const char * const ad9361_rf_tx_port[] =
{"A", "B"};

/**
 * Gain control modes.
 */
static const char * const ad9361_agc_modes[] =
{"manual", "fast_attack", "slow_attack", "hybrid"};

/**
 * State machine modes.
 */
extern const char *ad9361_ensm_states[12];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief get_rf_port_select().
 * @param device- Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_rf_port_select(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	int ret = 0;
	uint32_t mode;
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	if (channel->ch_out) {
		ret = ad9361_get_tx_rf_port_output(ad9361_phy, &mode);
		return ret < 0 ? ret : sprintf(buf, "%s", ad9361_rf_tx_port[mode]);
	} else {
		ret = ad9361_get_rx_rf_port_input(ad9361_phy, &mode);
		return ret < 0 ? ret : sprintf(buf, "%s", ad9361_rf_rx_port[mode]);
	}
}

/**
 * @brief get_hardwaregain().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_hardwaregain(void *device, char *buf, uint32_t len,
			    const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	struct rf_rx_gain rx_gain = {0};
	int32_t i = 0, ret, val1, val2;

	if (channel->ch_out) {
		ret = ad9361_get_tx_atten(ad9361_phy, channel->ch_num + 1);
		if (ret < 0)
			return -EINVAL;
		val1 = -1 * (ret / 1000);
		val2 = (ret % 1000) * 1000;
		if (!val1)
			val2 *= -1;
		if (val2 < 0 && val1 >= 0) {
			ret = (int) snprintf(buf, len, "-");
			if (ret < 0)
				return ret;
			i++;
		}
		ret = i + (int) snprintf(&buf[i], len,
					 "%"PRIi32".%.6"PRIi32" dB", val1,
					 abs(val2));

		return ret;
	} else {
		ret = ad9361_get_rx_gain(ad9361_phy,
					 ad9361_1rx1tx_channel_map(ad9361_phy,
							 false, channel->ch_num + 1), &rx_gain);
		if (ret < 0)
			return ret;

		return (int) snprintf(buf, len, "%"PRIi16".000000 dB",
				      (int)rx_gain.gain_db);
	}
}

/**
 * @brief get_rssi().
 * @param device- Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_rssi(void *device, char *buf, uint32_t len,
		    const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int ret = 0;

	if (channel->ch_out) {
		uint32_t rssi_db_x_1000;
		ret = ad9361_get_tx_rssi(ad9361_phy, channel->ch_num, &rssi_db_x_1000);
		if (ret < 0)
			return -EINVAL;
		return ret < 0 ? ret : sprintf(buf, "%"PRIu32".%02"PRIu32" dB",
					       rssi_db_x_1000 / 1000, rssi_db_x_1000 % 1000);
	} else {
		struct rf_rssi rssi = {0};
		ret = ad9361_get_rx_rssi (ad9361_phy, channel->ch_num, &rssi);
		return ret < 0 ? ret : sprintf(buf, "%"PRIu32".%02"PRIu32" dB",
					       rssi.symbol / rssi.multiplier, rssi.symbol % rssi.multiplier);
	}
}

/**
 * @brief get_hardwaregain_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_hardwaregain_available(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	if (channel->ch_out)
		return (int) snprintf(buf, len, "[%"PRIi16", %"PRIi16", %"PRIi16"]", 0, 250,
				      89750);
	else
		return (int) snprintf(buf, len, "[%"PRIi8", %"PRIi16", %"PRIi8"]",
				      ad9361_phy->gt_info[ad9361_gt(ad9361_phy)].abs_gain_tbl[0],
				      1,
				      ad9361_phy->gt_info[ad9361_gt(ad9361_phy)].
				      abs_gain_tbl[ad9361_phy->gt_info[ad9361_gt(ad9361_phy)].
						   max_index - 1]);
}

/**
 * @brief get_sampling_frequency_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_sampling_frequency_available(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int32_t int_dec;
	uint32_t max;

	if (ad9361_phy->pdata->port_ctrl.pp_conf[2] & LVDS_MODE)
		max = 61440000U;
	else
		max = 61440000U / (ad9361_phy->pdata->rx2tx2 ? 2 : 1);

	if (channel->ch_out) {
		if (ad9361_phy->bypass_tx_fir)
			int_dec = 1;
		else
			int_dec = ad9361_phy->tx_fir_int;
	} else {
		if (ad9361_phy->bypass_rx_fir)
			int_dec = 1;
		else
			int_dec = ad9361_phy->rx_fir_dec;
	}

	return (int) snprintf(buf, len, "[%"PRIu32" %"PRIi16" %"PRIu32"]",
			      MIN_ADC_CLK / (12 * int_dec), 1, max);
}

/**
 * @brief get_rf_port_select_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_rf_port_select_available(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	int bytes_no, ret;
	uint16_t i;

	if (channel->ch_out) {
		return (int) sprintf(buf, "%s %s",
				     ad9361_rf_tx_port[0],
				     ad9361_rf_tx_port[1]);
	} else {
		bytes_no = 0;
		for (i = 0; i < NO_OS_ARRAY_SIZE(ad9361_rf_rx_port); i++) {
			if (i > 0)
				bytes_no += sprintf(buf + bytes_no, " ");
			ret = sprintf(buf + bytes_no, "%s", ad9361_rf_rx_port[i]);
			if (ret < 0)
				return ret;
			bytes_no += ret;

		}
		return bytes_no;
	}
}

/**
 * @brief get_filter_fir_en().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_filter_fir_en(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint8_t en_dis;
	int ret;

	if (channel->ch_out)
		ret = ad9361_get_tx_fir_en_dis (ad9361_phy, &en_dis);
	else
		ret = ad9361_get_rx_fir_en_dis (ad9361_phy, &en_dis);
	if (ret < 0)
		return ret;

	return (int) snprintf(buf, len, "%"PRIi16"", en_dis);
}

/**
 * @brief get_sampling_frequency().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_sampling_frequency(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t sampling_freq_hz;
	int ret = ad9361_get_rx_sampling_freq (ad9361_phy, &sampling_freq_hz);

	if (ret < 0)
		return ret;

	return (int) snprintf(buf, len, "%"PRIi32"", sampling_freq_hz);
}

/**
 * @brief get_rf_bandwidth_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_rf_bandwidth_available(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	if (channel->ch_out)
		return snprintf(buf, len, "[200000 1 40000000]");
	else
		return snprintf(buf, len, "[200000 1 56000000]");
}

/**
 * @brief get_rf_bandwidth().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_rf_bandwidth(void *device, char *buf, uint32_t len,
			    const struct iio_ch_info *channel,
			    intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	if (channel->ch_out)
		return snprintf(buf, len, "%"PRIu32"", ad9361_phy->current_tx_bw_Hz);
	else
		return snprintf(buf, len, "%"PRIu32"", ad9361_phy->current_rx_bw_Hz);
}

/**
 * @brief get_gain_control_mode().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_gain_control_mode(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	return (int) snprintf(buf, len, "%s",
			      ad9361_agc_modes[ad9361_phy->agc_mode[channel->ch_num]]);
}

/**
 * @brief get_rf_dc_offset_tracking_en().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_rf_dc_offset_tracking_en(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	if (!channel->ch_out)
		return (int) snprintf(buf, len, "%"PRIi16"", ad9361_phy->rfdc_track_en) + 1;

	return -ENOENT;
}

/**
 * @brief get_quadrature_tracking_en().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_quadrature_tracking_en(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	if (!channel->ch_out)
		return (int) snprintf(buf, len, "%"PRIi16"", ad9361_phy->quad_track_en) + 1;

	return -ENOENT;
}

/**
 * @brief get_gain_control_mode_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_gain_control_mode_available(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	return (int) snprintf(buf, len, "%s %s %s %s",
			      ad9361_agc_modes[0],
			      ad9361_agc_modes[1],
			      ad9361_agc_modes[2],
			      ad9361_agc_modes[3]);
}

/**
 * @brief get_bb_dc_offset_tracking_en().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len -	Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_bb_dc_offset_tracking_en(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	if (!channel->ch_out)
		return (int) snprintf(buf, len, "%"PRIi16"", ad9361_phy->bbdc_track_en) + 1;

	return -ENOENT;
}

/**
 * @brief get_frequency_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_frequency_available(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	return snprintf(buf, len, "[%llu 1 %llu]",
			AD9363A_MIN_CARRIER_FREQ_HZ,
			AD9363A_MAX_CARRIER_FREQ_HZ);
}

/**
 * @brief get_fastlock_save().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_fastlock_save(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint8_t faslock_vals[16];
	uint32_t length;
	int32_t ret = 0;
	int32_t i;

	ret = ad9361_fastlock_save(ad9361_phy, channel->ch_num == 1,
				   ad9361_phy->fastlock.save_profile, faslock_vals);
	if (ret < 0)
		return ret;
	length = snprintf(buf, len, "%"PRIu8" ", ad9361_phy->fastlock.save_profile);

	for (i = 0; i < RX_FAST_LOCK_CONFIG_WORD_NUM; i++)
		length += sprintf(buf + length, "%"PRIu8"%c", faslock_vals[i],
				  i == 15 ? '\n' : ',');

	return length;
}

/**
 * @brief get_powerdown().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_powerdown(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint64_t val = 0;

	val = !!(ad9361_phy->cached_synth_pd[channel->ch_num ? 0 : 1] &
		 RX_LO_POWER_DOWN);

	return snprintf(buf, len, "%"PRIu64, val);
}

/**
 * @brief get_fastlock_load().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_fastlock_load(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	/* This function doesn't have an equivalent function in ad9361,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief get_fastlock_store().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_fastlock_store(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	/* This function doesn't have an equivalent function in ad9361,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief get_frequency().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf -	Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return: Length of chars written in buf, or negative value on failure.
 */
static int get_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel,
			 intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint64_t val = 0;

	val = ad9361_from_clk(clk_get_rate(ad9361_phy,
					   ad9361_phy->ref_clk_scale[channel->ch_num ?
									   TX_RFPLL : RX_RFPLL]));
	return snprintf(buf, len, "%"PRIu64, val);
}

/**
 * @brief get_external().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_external(void *device, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	if (channel->ch_num == 0)
		return (int) snprintf(buf, len, "%"PRIi16"",
				      ad9361_phy->pdata->use_ext_rx_lo);
	else
		return (int) snprintf(buf, len, "%"PRIi16"",
				      ad9361_phy->pdata->use_ext_tx_lo);
}

/**
 * @brief get_fastlock_recall().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len -	Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_fastlock_recall(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	return snprintf(buf, len, "%"PRIi16"",
			ad9361_phy->fastlock.current_profile[channel->ch_num]);
}

/**
 * @brief get_temp0_input().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_temp0_input(void *device, char *buf, uint32_t len,
			   const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int32_t temp;
	int ret = ad9361_get_temperature(ad9361_phy, &temp);
	if (ret < 0)
		return ret;

	return (int) snprintf(buf, len, "%"PRIi32"", temp);
}

/**
 * @brief get_voltage_filter_fir_en().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_voltage_filter_fir_en(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint8_t en_dis_tx, en_dis_rx;
	int ret;

	ret = ad9361_get_tx_fir_en_dis (ad9361_phy, &en_dis_tx);
	if (ret < 0)
		return ret;
	ret = ad9361_get_rx_fir_en_dis (ad9361_phy, &en_dis_rx);
	if (ret < 0)
		return ret;

	return (int) snprintf(buf, len, "%"PRIi16"", en_dis_rx && en_dis_tx);
}

/**
 * @brief set_hardwaregain_available().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_hardwaregain_available(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	/* This function doesn't have an equivalent function in ad9361,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_hardwaregain().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_hardwaregain(void *device, char *buf, uint32_t len,
			    const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int ret = 0;
	float gain = strtof(buf, NULL);
	int32_t val1 = (int32_t)gain;
	int32_t val2 = (int32_t)(gain * 1000) % 1000;

	if (channel->ch_out) {
		int32_t ch;
		if (val1 > 0 || (val1 == 0 && val2 > 0)) {
			return -EINVAL;
		}
		uint32_t code = ((abs(val1) * 1000) + (abs(val2)));
		ch = ad9361_1rx1tx_channel_map(ad9361_phy, true, channel->ch_num);
		ret = ad9361_set_tx_atten(ad9361_phy, code, ch == 0, ch == 1,
					  !ad9361_phy->pdata->update_tx_gain_via_alert);
		if (ret < 0) {
			return -EINVAL;
		}
	} else {
		struct rf_rx_gain rx_gain = {0};
		rx_gain.gain_db = val1;
		ret = ad9361_set_rx_gain(ad9361_phy,
					 ad9361_1rx1tx_channel_map(ad9361_phy, false, channel->ch_num + 1), &rx_gain);
		if (ret < 0) {
			return -EINVAL;
		}
	}

	return len;
}

/**
 * @brief set_rssi().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return  Number of bytes written to device, or negative value on failure.
 */
static int set_rssi(void *device, char *buf, uint32_t len,
		    const struct iio_ch_info *channel, intptr_t priv)
{
	/* This function doesn't have an equivalent function in ad9361,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_rf_port_select().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len -	Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
static int set_rf_port_select(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int ret = 0;
	uint32_t i = 0;

	if (channel->ch_out) {
		for (i = 0; i < NO_OS_ARRAY_SIZE(ad9361_rf_tx_port); i++) {
			if (!strcmp(ad9361_rf_tx_port[i], buf))
				break;
		}
		if (i >= NO_OS_ARRAY_SIZE(ad9361_rf_tx_port))
			return -EINVAL;
		ret = ad9361_set_tx_rf_port_output(ad9361_phy, i);
		return (ret < 0) ? ret : (int)len;
	} else {
		for (i = 0; i < NO_OS_ARRAY_SIZE(ad9361_rf_rx_port); i++) {
			if (!strcmp(ad9361_rf_rx_port[i], buf))
				break;
		}
		if (i >= NO_OS_ARRAY_SIZE(ad9361_rf_tx_port))
			return -EINVAL;

		ret = ad9361_set_rx_rf_port_input(ad9361_phy, i);
		if (ret < 0)
			return ret;

		return len;
	}
}

/**
 * @brief set_gain_control_mode().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_gain_control_mode(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	struct rf_gain_ctrl gc = {0};
	uint32_t mode;
	uint32_t i;
	int ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad9361_agc_modes); i++) {
		if (!strcmp(ad9361_agc_modes[i], buf))
			break;
	}
	if (i >= NO_OS_ARRAY_SIZE(ad9361_agc_modes))
		return -EINVAL;

	mode = i;
	if (ad9361_phy->agc_mode[channel->ch_num] == mode)
		return len;
	gc.ant = ad9361_1rx1tx_channel_map(ad9361_phy, false, channel->ch_num + 1);
	gc.mode = mode;
	ad9361_phy->agc_mode[channel->ch_num] = mode;
	ret = ad9361_set_gain_ctrl_mode(ad9361_phy, &gc);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_rf_port_select_available().
 * @param device Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_rf_port_select_available(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	/* This function doesn't have an equivalent function in ad9361,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_rf_bandwidth().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_rf_bandwidth(void *device, char *buf, uint32_t len,
			    const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int ret = -ENOENT;
	uint32_t rf_bandwidth = no_os_str_to_uint32(buf);

	rf_bandwidth = ad9361_validate_rf_bw(ad9361_phy, rf_bandwidth);
	if (channel->ch_out) {
		if (ad9361_phy->current_tx_bw_Hz != rf_bandwidth)
			ret = ad9361_update_rf_bandwidth(ad9361_phy, ad9361_phy->current_rx_bw_Hz,
							 rf_bandwidth);
	} else {
		if (ad9361_phy->current_rx_bw_Hz != rf_bandwidth)
			ret = ad9361_update_rf_bandwidth(ad9361_phy, rf_bandwidth,
							 ad9361_phy->current_tx_bw_Hz);
	}
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_rf_dc_offset_tracking_en().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_rf_dc_offset_tracking_en(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int8_t en_dis = no_os_str_to_int32(buf);
	int32_t ret;

	if (en_dis < 0)
		return en_dis;

	ad9361_phy->rfdc_track_en = en_dis ? 1 : 0;
	if (!channel->ch_out) {
		ret = ad9361_tracking_control(ad9361_phy, ad9361_phy->bbdc_track_en,
					      ad9361_phy->rfdc_track_en, ad9361_phy->quad_track_en);
		if (ret < 0)
			return ret;
	}

	return len;
}

/**
 * @brief set_sampling_frequency_available().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_sampling_frequency_available(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	/* This function doesn't have an equivalent function in ad9361,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_quadrature_tracking_en().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_quadrature_tracking_en(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int8_t en_dis = no_os_str_to_int32(buf);
	int32_t ret;

	if (en_dis < 0)
		return en_dis;

	ad9361_phy->quad_track_en = en_dis ? 1 : 0;
	if (!channel->ch_out) {
		ret = ad9361_tracking_control(ad9361_phy, ad9361_phy->bbdc_track_en,
					      ad9361_phy->rfdc_track_en, ad9361_phy->quad_track_en);
		if (ret < 0)
			return ret;
	}

	return len;
}

/**
 * @brief set_sampling_frequency().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_sampling_frequency(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t sampling_freq_hz = no_os_str_to_uint32(buf);
	int ret = ad9361_set_rx_sampling_freq (ad9361_phy, sampling_freq_hz);

	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_gain_control_mode_available().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_gain_control_mode_available(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	struct rf_gain_ctrl gc = {0};
	uint32_t mode;
	int ret;
	uint16_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad9361_agc_modes); i++) {
		if (!strcmp(ad9361_agc_modes[i], buf))
			break;
	}
	if (i >= NO_OS_ARRAY_SIZE(ad9361_agc_modes))
		return -EINVAL;

	mode = i;
	if (ad9361_phy->agc_mode[channel->ch_num] == mode)
		return len;
	gc.ant = ad9361_1rx1tx_channel_map(ad9361_phy, false, channel->ch_num + 1);
	gc.mode = mode;
	ad9361_phy->agc_mode[channel->ch_num] = mode;
	ret = ad9361_set_gain_ctrl_mode(ad9361_phy, &gc);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_filter_fir_en().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_filter_fir_en(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int8_t en_dis = no_os_str_to_int32(buf);
	int ret;

	if (en_dis < 0)
		return en_dis;
	en_dis = en_dis ? 1 : 0;
	if (channel->ch_out)
		ret = ad9361_set_tx_fir_en_dis (ad9361_phy, en_dis);
	else
		ret = ad9361_set_rx_fir_en_dis (ad9361_phy, en_dis);

	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_rf_bandwidth_available().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_rf_bandwidth_available(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	/* This function doesn't have an equivalent function in ad9361,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_bb_dc_offset_tracking_en().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_bb_dc_offset_tracking_en(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int8_t en_dis = no_os_str_to_int32(buf);
	int32_t ret;

	if (en_dis < 0)
		return en_dis;

	ad9361_phy->bbdc_track_en = en_dis ? 1 : 0;
	if (!channel->ch_out) {
		ret = ad9361_tracking_control(ad9361_phy, ad9361_phy->bbdc_track_en,
					      ad9361_phy->rfdc_track_en, ad9361_phy->quad_track_en);
		if (ret < 0)
			return ret;
	}

	return -ENOENT;
}

/**
 * @brief set_frequency_available().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_frequency_available(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	/* This function doesn't have an equivalent function in ad9361,
	 * and it should be implemented there first */

	return -ENOENT;
}

/**
 * @brief set_fastlock_save().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_fastlock_save(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t readin = no_os_str_to_uint32(buf);

	ad9361_phy->fastlock.save_profile = readin;

	return len;
}

/**
 * @brief set_powerdown().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_powerdown(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int ret = -ENOENT;
	bool res = no_os_str_to_int32(buf) ? 1 : 0;

	if (channel->ch_num == 0)
		ret = ad9361_synth_lo_powerdown(ad9361_phy, res ? LO_OFF : LO_ON, LO_DONTCARE);
	else if (channel->ch_num == 1)
		ret = ad9361_synth_lo_powerdown(ad9361_phy, LO_DONTCARE, res ? LO_OFF : LO_ON);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_fastlock_load().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
static int set_fastlock_load(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int ret = 0;
	char *line, *ptr = buf;
	uint8_t faslock_vals[16];
	uint32_t profile = 0, val, val2, i = 0;

	while ((line = strsep(&ptr, ","))) {
		if (line >= buf + len)
			break;

		ret = sscanf(line, "%"PRIu32" %"PRIu32"", &val, &val2);
		if (ret == 1) {
			faslock_vals[i++] = val;
			continue;
		} else if (ret == 2) {
			profile = val;
			faslock_vals[i++] = val2;
			continue;
		}
	}
	if (i == 16)
		ret = ad9361_fastlock_load(ad9361_phy, channel->ch_num == 1,
					   profile, faslock_vals);
	else
		ret = -EINVAL;
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_fastlock_store().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_fastlock_store(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t profile = no_os_str_to_uint32(buf);
	int32_t ret;

	ret = ad9361_fastlock_store(ad9361_phy, channel->ch_num == 1, profile);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_frequency().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint64_t lo_freq_hz = no_os_str_to_uint32(buf);
	int ret = 0;

	switch (channel->ch_num) {
	case 0:
		ret = no_os_clk_set_rate(ad9361_phy, ad9361_phy->ref_clk_scale[RX_RFPLL],
					 ad9361_to_clk(lo_freq_hz));
		break;
	case 1:
		ret = no_os_clk_set_rate(ad9361_phy, ad9361_phy->ref_clk_scale[TX_RFPLL],
					 ad9361_to_clk(lo_freq_hz));
		break;
	default:
		ret = -EINVAL;
	}
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_external().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_external(void *device, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	bool select = no_os_str_to_int32(buf) ? 1 : 0;
	int ret = 0;

	if (channel->ch_num == 0)
		ret = ad9361_set_rx_lo_int_ext(ad9361_phy, select);
	else
		ret = ad9361_set_tx_lo_int_ext(ad9361_phy, select);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_fastlock_recall().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_fastlock_recall(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int ret = 0;
	uint32_t profile = no_os_str_to_uint32(buf);

	ret = ad9361_fastlock_recall(ad9361_phy, channel->ch_num == 1, profile);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_voltage_filter_fir_en().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_voltage_filter_fir_en(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int8_t en_dis = no_os_str_to_int32(buf) ? 1 : 0;
	int ret;

	ret = ad9361_set_tx_fir_en_dis (ad9361_phy, en_dis);
	if (ret < 0)
		return ret;
	ret = ad9361_set_rx_fir_en_dis (ad9361_phy, en_dis);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief get_dcxo_tune_coarse().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_dcxo_tune_coarse(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	if (ad9361_phy->pdata->use_extclk)
		return -ENOENT;
	else
		return snprintf(buf, len, "%"PRIi32"", ad9361_phy->pdata->dcxo_coarse);
}

/**
 * @brief get_rx_path_rates().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_rx_path_rates(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t clk[6];
	int ret = ad9361_get_trx_clock_chain(ad9361_phy, clk, NULL);

	if (ret < 0)
		return ret;

	return snprintf(buf, len,
			"BBPLL:%"PRIu32" ADC:%"PRIu32" R2:%"PRIu32" R1:%"PRIu32" RF:%"PRIu32" RXSAMP:%"PRIu32"",
			clk[0], clk[1], clk[2], clk[3], clk[4], clk[5]);
}

/**
 * @brief get_trx_rate_governor().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_trx_rate_governor(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t rate_governor;
	int ret = ad9361_get_trx_rate_gov (ad9361_phy, &rate_governor);

	if (ret < 0)
		return ret;

	return snprintf(buf, len, "%s", rate_governor ? "nominal" : "highest_osr");
}

/**
 * @brief get_calib_mode_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_calib_mode_available(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	return (int) snprintf(buf, len, "%s %s %s %s %s", ad9361_calib_mode[0],
			      ad9361_calib_mode[1], ad9361_calib_mode[2],
			      ad9361_calib_mode[3], ad9361_calib_mode[4]);
}

/**
 * @brief get_xo_correction_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_xo_correction_available(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	return (int) snprintf(buf, len, "%"PRIi16"", 0); /* dummy */
}

/**
 * @brief get_gain_table_config().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_gain_table_config(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	return (int) snprintf(buf, len, "%"PRIi16"", 0); /* dummy */
}

/**
 * @brief get_dcxo_tune_fine().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_dcxo_tune_fine(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	if (ad9361_phy->pdata->use_extclk)
		return -ENOENT;
	else
		return snprintf(buf, len, "%"PRIu32"", ad9361_phy->pdata->dcxo_fine);
}

/**
 * @brief get_dcxo_tune_fine_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len -Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return: Length of chars written in buf, or negative value on failure.
 */
static int get_dcxo_tune_fine_available(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	return snprintf(buf, len, "%s",
			ad9361_phy->pdata->use_extclk ? "[0 0 0]" : "[0 1 8191]");
}

/**
 * @brief get_ensm_mode_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_ensm_mode_available(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	return (int) snprintf(buf, len, "%s", ad9361_phy->pdata->fdd ?
			      "sleep wait alert fdd pinctrl pinctrl_fdd_indep" :
			      "sleep wait alert rx tx pinctrl");
}

/**
 * @brief get_multichip_sync().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - sChannel properties.
 * @return: Length of chars written in buf, or negative value on failure.
 */
static int get_multichip_sync(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel,
			      intptr_t priv)
{
	return (int) snprintf(buf, len, "%"PRIi16"", 0);  /* dummy */
}

/**
 * @brief get_rssi_gain_step_error().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_rssi_gain_step_error(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	return (int) snprintf(buf, len, "%"PRIi16"", 0);  /* dummy */
}

/**
 * @brief get_dcxo_tune_coarse_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_dcxo_tune_coarse_available(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	return (int) snprintf(buf, len, "%s",
			      ad9361_phy->pdata->use_extclk ? "[0 0 0]" : "[0 1 63]");
}

/**
 * @brief get_tx_path_rates().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_tx_path_rates(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t clk[6];
	int ret = ad9361_get_trx_clock_chain(ad9361_phy, NULL, clk);

	if (ret < 0)
		return ret;

	return snprintf(buf, len,
			"BBPLL:%"PRIu32" DAC:%"PRIu32" T2:%"PRIu32" T1:%"PRIu32" TF:%"PRIu32" TXSAMP:%"PRIu32"",
			clk[0], clk[1], clk[2], clk[3], clk[4], clk[5]);
}

/**
 * @brief get_trx_rate_governor_available().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_trx_rate_governor_available(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	return snprintf(buf, len, "%s", "nominal highest_osr");
}

/**
 * @brief get_xo_correction().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_xo_correction(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	return (int) snprintf(buf, len, "%"PRIi16"", 0); /* dummy */
}

/**
 * @brief get_ensm_mode().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_ensm_mode(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int ret = ad9361_ensm_get_state(ad9361_phy);

	if (ret < 0)
		return ret;
	if ((uint32_t)ret >= NO_OS_ARRAY_SIZE(ad9361_ensm_states) ||
	    ad9361_ensm_states[ret] == NULL)
		return -EIO;

	return snprintf(buf, len, "%s", ad9361_ensm_states[ret]);
}

/**
 * @brief get_filter_fir_config().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_filter_fir_config(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;

	return snprintf(buf, len, "FIR Rx: %"PRIu8",%"PRIu8" Tx: %"PRIu8",%"PRIu8"",
			ad9361_phy->rx_fir_ntaps, ad9361_phy->rx_fir_dec,
			ad9361_phy->tx_fir_ntaps, ad9361_phy->tx_fir_int);
}

/**
 * @brief get_calib_mode().
 * @param device - Physical instance of a iio_axi_adc device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static int get_calib_mode(void *device, char *buf, uint32_t len,
			  const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint8_t en_dis;
	int ret = ad9361_get_tx_auto_cal_en_dis(ad9361_phy, &en_dis);

	if (ret < 0)
		return ret;

	return (int) snprintf(buf, len, "%s", en_dis ? "auto" : "manual");
}

/**
 * @brief set_trx_rate_governor().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_trx_rate_governor(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int ret = 0;

	if (!strcmp(buf, "nominal"))
		ad9361_set_trx_rate_gov (ad9361_phy, 1);
	else if (!strcmp(buf, "highest_osr"))
		ad9361_set_trx_rate_gov (ad9361_phy, 0);
	else
		ret =  -ENOENT;

	return ret;
}

/**
 * @brief set_dcxo_tune_coarse().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_dcxo_tune_coarse(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t dcxo_coarse = no_os_str_to_uint32(buf);
	int32_t ret;

	dcxo_coarse = no_os_clamp_t(uint32_t, dcxo_coarse, 0, 63U);
	ad9361_phy->pdata->dcxo_coarse = dcxo_coarse;

	ret = ad9361_set_dcxo_tune(ad9361_phy, ad9361_phy->pdata->dcxo_coarse,
				   ad9361_phy->pdata->dcxo_fine);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_dcxo_tune_fine().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_dcxo_tune_fine(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t dcxo_fine = no_os_str_to_uint32(buf);
	int32_t ret;

	dcxo_fine = no_os_clamp_t(uint32_t, dcxo_fine, 0, 8191U);
	ad9361_phy->pdata->dcxo_fine = dcxo_fine;

	ret = ad9361_set_dcxo_tune(ad9361_phy, ad9361_phy->pdata->dcxo_coarse,
				   ad9361_phy->pdata->dcxo_fine);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_calib_mode().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_calib_mode(void *device, char *buf, uint32_t len,
			  const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int32_t arg = -1;
	int ret = 0;
	uint32_t val = 0;
	val = 0;

	if (!strcmp(buf, "auto")) {
		ret = ad9361_set_tx_auto_cal_en_dis (ad9361_phy, 1);
	} else if (!strcmp(buf, "manual")) {
		ad9361_set_tx_auto_cal_en_dis (ad9361_phy, 0);
	} else if (!strncmp(buf, "tx_quad", 7)) {
		ret = sscanf(buf, "tx_quad %"PRIi32, &arg);
		if (ret != 1)
			arg = -1;
		val = TX_QUAD_CAL;
	} else if (!strcmp(buf, "rf_dc_offs")) {
		val = RFDC_CAL;
	} else if (!strcmp(buf, "rssi_gain_step")) {
		ret = ad9361_rssi_gain_step_calib(ad9361_phy);
	} else {
		return -ENOENT;
	}
	if (ret < 0)
		return ret;
	if (val)
		ret = ad9361_do_calib(ad9361_phy, val, arg);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_ensm_mode().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
static int set_ensm_mode(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t val = 0;
	bool res = false;
	int ret;

	ad9361_phy->pdata->fdd_independent_mode = false;

	if (!strcmp(buf, "tx")) {
		val = ENSM_STATE_TX;
	} else if (!strcmp(buf, "rx")) {
		val = ENSM_STATE_RX;
	} else if (!strcmp(buf, "alert")) {
		val = ENSM_STATE_ALERT;
	} else if (!strcmp(buf, "fdd")) {
		val = ENSM_STATE_FDD;
	} else if (!strcmp(buf, "wait")) {
		val = ENSM_STATE_SLEEP_WAIT;
	} else if (!strcmp(buf, "sleep")) {
		val = ENSM_STATE_SLEEP;
	} else if (!strcmp(buf, "pinctrl")) {
		res = true;
		val = ENSM_STATE_SLEEP_WAIT;
	} else if (!strcmp(buf, "pinctrl_fdd_indep")) {
		val = ENSM_STATE_FDD;
		ad9361_phy->pdata->fdd_independent_mode = true;
	} else {
		return -ENOENT;
	}

	ret = ad9361_set_ensm_mode(ad9361_phy, ad9361_phy->pdata->fdd, res);
	if (ret < 0)
		return ret;

	ret = ad9361_ensm_set_state(ad9361_phy, val, res);
	if (ret < 0)
		return ret;

	return len;
}

/**
 * @brief set_multichip_sync().
 * @param device Physical instance of a iio_axi_dac device.
 * @param buf Value to be written to attribute.
 * @param len Length of the data in "buf".
 * @param channel Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_multichip_sync(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	uint32_t readin = no_os_str_to_uint32(buf);
	int32_t ret;

	ret = ad9361_mcs(ad9361_phy, readin);
	if (ret < 0)
		return ret;

	return len;
}

extern int32_t ad9361_parse_fir(struct ad9361_rf_phy *phy,
				char *data, uint32_t size);

/**
 * @brief set_filter_fir_config().
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int set_filter_fir_config(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad9361_rf_phy *ad9361_phy = (struct ad9361_rf_phy *)device;
	int32_t ret;

	ret = ad9361_parse_fir(ad9361_phy, (char *)buf, len);
	if (ret < 0)
		return ret;

	return len;
}

struct iio_attribute voltage_output_attributes[] = {
	{
		.name = "rf_port_select",
		.show = get_rf_port_select,
		.store = set_rf_port_select,
	},
	{
		.name = "hardwaregain",
		.show = get_hardwaregain,
		.store = set_hardwaregain,
	},
	{
		.name = "rssi",
		.show = get_rssi,
		.store = set_rssi,
	},
	{
		.name = "hardwaregain_available",
		.show = get_hardwaregain_available,
		.store = set_hardwaregain_available,
	},
	{
		.name = "sampling_frequency_available",
		.show = get_sampling_frequency_available,
		.store = set_sampling_frequency_available,
	},
	{
		.name = "rf_port_select_available",
		.show = get_rf_port_select_available,
		.store = set_rf_port_select_available,
	},
	{
		.name = "filter_fir_en",
		.show = get_filter_fir_en,
		.store = set_filter_fir_en,
	},
	{
		.name = "sampling_frequency",
		.show = get_sampling_frequency,
		.store = set_sampling_frequency,
	},
	{
		.name = "rf_bandwidth_available",
		.show = get_rf_bandwidth_available,
		.store = set_rf_bandwidth_available,
	},
	{
		.name = "rf_bandwidth",
		.show = get_rf_bandwidth,
		.store = set_rf_bandwidth,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute voltage_input_attributes[] = {
	{
		.name = "hardwaregain_available",
		.show = get_hardwaregain_available,
		.store = set_hardwaregain_available,
	},
	{
		.name = "hardwaregain",
		.show = get_hardwaregain,
		.store = set_hardwaregain,
	},
	{
		.name = "rssi",
		.show = get_rssi,
		.store = set_rssi,
	},
	{
		.name = "rf_port_select",
		.show = get_rf_port_select,
		.store = set_rf_port_select,
	},
	{
		.name = "gain_control_mode",
		.show = get_gain_control_mode,
		.store = set_gain_control_mode,
	},
	{
		.name = "rf_port_select_available",
		.show = get_rf_port_select_available,
		.store = set_rf_port_select_available,
	},
	{
		.name = "rf_bandwidth",
		.show = get_rf_bandwidth,
		.store = set_rf_bandwidth,
	},
	{
		.name = "rf_dc_offset_tracking_en",
		.show = get_rf_dc_offset_tracking_en,
		.store = set_rf_dc_offset_tracking_en,
	},
	{
		.name = "sampling_frequency_available",
		.show = get_sampling_frequency_available,
		.store = set_sampling_frequency_available,
	},
	{
		.name = "quadrature_tracking_en",
		.show = get_quadrature_tracking_en,
		.store = set_quadrature_tracking_en,
	},
	{
		.name = "sampling_frequency",
		.show = get_sampling_frequency,
		.store = set_sampling_frequency,
	},
	{
		.name = "gain_control_mode_available",
		.show = get_gain_control_mode_available,
		.store = set_gain_control_mode_available,
	},
	{
		.name = "filter_fir_en",
		.show = get_filter_fir_en,
		.store = set_filter_fir_en,
	},
	{
		.name = "rf_bandwidth_available",
		.show = get_rf_bandwidth_available,
		.store = set_rf_bandwidth_available,
	},
	{
		.name = "bb_dc_offset_tracking_en",
		.show = get_bb_dc_offset_tracking_en,
		.store = set_bb_dc_offset_tracking_en,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute altvoltage_attributes[] = {
	{
		.name = "frequency_available",
		.show = get_frequency_available,
		.store = set_frequency_available,
	},
	{
		.name = "fastlock_save",
		.show = get_fastlock_save,
		.store = set_fastlock_save,
	},
	{
		.name = "powerdown",
		.show = get_powerdown,
		.store = set_powerdown,
	},
	{
		.name = "fastlock_load",
		.show = get_fastlock_load,
		.store = set_fastlock_load,
	},
	{
		.name = "fastlock_store",
		.show = get_fastlock_store,
		.store = set_fastlock_store,
	},
	{
		.name = "frequency",
		.show = get_frequency,
		.store = set_frequency,
	},
	{
		.name = "external",
		.show = get_external,
		.store = set_external,
	},
	{
		.name = "fastlock_recall",
		.show = get_fastlock_recall,
		.store = set_fastlock_recall,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute out_attributes[] = {
	{
		.name = "voltage_filter_fir_en",
		.show = get_voltage_filter_fir_en,
		.store = set_voltage_filter_fir_en,
	},
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute temp0_attributes[] = {
	{
		.name = "input",
		.show = get_temp0_input,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute global_attributes[] = {
	{
		.name = "dcxo_tune_coarse",
		.show = get_dcxo_tune_coarse,
		.store = set_dcxo_tune_coarse,
	},
	{
		.name = "rx_path_rates",
		.show = get_rx_path_rates,
		.store = NULL,
	},
	{
		.name = "trx_rate_governor",
		.show = get_trx_rate_governor,
		.store = set_trx_rate_governor,
	},
	{
		.name = "calib_mode_available",
		.show = get_calib_mode_available,
		.store = NULL,
	},
	{
		.name = "xo_correction_available",
		.show = get_xo_correction_available,
		.store = NULL,
	},
	{
		.name = "gain_table_config",
		.show = get_gain_table_config,
		.store = NULL,
	},
	{
		.name = "dcxo_tune_fine",
		.show = get_dcxo_tune_fine,
		.store = set_dcxo_tune_fine,
	},
	{
		.name = "dcxo_tune_fine_available",
		.show = get_dcxo_tune_fine_available,
		.store = NULL,
	},
	{
		.name = "ensm_mode_available",
		.show = get_ensm_mode_available,
		.store = NULL,
	},
	{
		.name = "multichip_sync",
		.show = get_multichip_sync,
		.store = set_multichip_sync,
	},
	{
		.name = "rssi_gain_step_error",
		.show = get_rssi_gain_step_error,
		.store = NULL,
	},
	{
		.name = "dcxo_tune_coarse_available",
		.show = get_dcxo_tune_coarse_available,
		.store = NULL,
	},
	{
		.name = "tx_path_rates",
		.show = get_tx_path_rates,
		.store = NULL,
	},
	{
		.name = "trx_rate_governor_available",
		.show = get_trx_rate_governor_available,
		.store = NULL,
	},
	{
		.name = "xo_correction",
		.show = get_xo_correction,
		.store = NULL,
	},
	{
		.name = "ensm_mode",
		.show = get_ensm_mode,
		.store = set_ensm_mode,
	},
	{
		.name = "filter_fir_config",
		.show = get_filter_fir_config,
		.store = set_filter_fir_config,
	},
	{
		.name = "calib_mode",
		.show = get_calib_mode,
		.store = set_calib_mode,
	},
	END_ATTRIBUTES_ARRAY,
};

#define AD9361_VOLTAGE_IN(_idx) {\
	.name = "voltage" # _idx,\
	.attributes = voltage_input_attributes,\
	.ch_out = false,\
	.scan_type = NULL,\
	.indexed = true,\
	.channel = _idx,\
	.ch_type = IIO_VOLTAGE,\
}

#define AD9361_VOLTAGE_OUT(_idx) {\
	.name = "voltage" # _idx,\
	.attributes = voltage_output_attributes,\
	.ch_out = true,\
	.scan_type = NULL,\
	.indexed = true,\
	.channel = _idx,\
	.ch_type = IIO_VOLTAGE,\
}

#define AD9361_ALTVOLTAGE_OUT(_idx) {\
	.name = "altvoltage" # _idx,\
	.attributes = altvoltage_attributes,\
	.ch_out = true,\
	.scan_type = NULL,\
	.indexed = true,\
	.channel = _idx,\
	.ch_type = IIO_ALTVOLTAGE,\
}

#define AD9361_TEMP(_idx) {\
	.name = "temp" # _idx,\
	.attributes = temp0_attributes,\
	.ch_out = false,\
	.scan_type = NULL,\
	.indexed = true,\
	.channel = _idx,\
	.ch_type = IIO_TEMP,\
}

#define AD9361_OUT() {\
	.name = "out",\
	.attributes = out_attributes,\
	.ch_out = false,\
	.scan_type = NULL,\
}

static struct iio_channel iio_ad9361_channels[] = {
	AD9361_VOLTAGE_IN(0),
	AD9361_VOLTAGE_IN(1),
	AD9361_VOLTAGE_IN(2),
	AD9361_VOLTAGE_OUT(0),
	AD9361_VOLTAGE_OUT(1),
	AD9361_VOLTAGE_OUT(2),
	AD9361_VOLTAGE_OUT(3),
	AD9361_ALTVOLTAGE_OUT(0),
	AD9361_ALTVOLTAGE_OUT(1),
	AD9361_TEMP(0),
	AD9361_OUT(),
};

/**
 * @brief Get iio device descriptor.
 * @param desc - Descriptor.
 * @param dev_descriptor - iio device descriptor.
 */
void iio_ad9361_get_dev_descriptor(struct iio_ad9361_desc *desc,
				   struct iio_device **dev_descriptor)
{
	*dev_descriptor = &desc->dev_descriptor;
}

/**
 * @brief Init for reading/writing and parameterization of a
 * ad9361 device.
 * @param desc - Descriptor.
 * @param init - Configuration structure.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_ad9361_init(struct iio_ad9361_desc **desc,
			struct iio_ad9361_init_param *init)
{
	struct iio_ad9361_desc *iio_ad9361_inst;

	iio_ad9361_inst = (struct iio_ad9361_desc *)no_os_calloc(1,
			  sizeof(struct iio_ad9361_desc));
	if (!iio_ad9361_inst)
		return -1;

	iio_ad9361_inst->dev_descriptor.num_ch = NO_OS_ARRAY_SIZE(iio_ad9361_channels);
	iio_ad9361_inst->dev_descriptor.channels = iio_ad9361_channels;
	iio_ad9361_inst->dev_descriptor.attributes = global_attributes;
	iio_ad9361_inst->dev_descriptor.debug_attributes = NULL;
	iio_ad9361_inst->dev_descriptor.buffer_attributes = NULL;
	iio_ad9361_inst->dev_descriptor.debug_reg_read = (int32_t (*)())ad9361_reg_read;
	iio_ad9361_inst->dev_descriptor.debug_reg_write = (int32_t (
				*)())ad9361_reg_write;
	*desc = iio_ad9361_inst;

	return 0;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_ad9361_remove(struct iio_ad9361_desc *desc)
{
	if (!desc)
		return -1;

	no_os_free(desc);

	return 0;
}
