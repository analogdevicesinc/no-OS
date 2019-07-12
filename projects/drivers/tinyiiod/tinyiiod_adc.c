/***************************************************************************//**
 *   @file   tinyiiod_adc.c
 *   @brief  Implementation of tinyiiod_adc
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

#include "ad9361_api.h" //todo remove
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <tinyiiod_util.h>
#include <xil_cache.h>
#include "tinyiiod_adc.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "axi_dmac.h"

extern struct ad9361_rf_phy *ad9361_phy; //todo remove this
static uint32_t adc_ddr_baseaddr; //todo init

ssize_t tinyiiod_adc_configure(uint32_t adc_ddr_base)
{
	adc_ddr_baseaddr = adc_ddr_base;
	return 0;
}

/**
 * get_cf_calibphase
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_cf_calibphase(char *buf, size_t len,
			  const struct channel_info *channel)
{
	int32_t val, val2;
	int32_t i = 0;
	ssize_t ret = axi_adc_get_calib_phase(ad9361_phy->rx_adc, channel->ch_num, &val,
					      &val2);
	if(ret < 0)
		return ret;
	if(val2 < 0 && val >= 0) {
		snprintf(buf, len, "-");
		i++;
	}

	return i + snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val, labs(val2));
}

/**
 * get_cf_calibbias
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_cf_calibbias(char *buf, size_t len,
			 const struct channel_info *channel)
{
	int32_t val;
	axi_adc_get_calib_bias(ad9361_phy->rx_adc,
			       channel->ch_num,
			       &val,
			       NULL);

	return snprintf(buf, len, "%"PRIi32"", val);
}

/**
 * get_cf_calibscale
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_cf_calibscale(char *buf, size_t len,
			  const struct channel_info *channel)
{
	int32_t val, val2;
	ssize_t ret = axi_adc_get_calib_scale(ad9361_phy->rx_adc, channel->ch_num, &val,
					      &val2);
	int32_t i = 0;
	if(ret < 0)
		return ret;
	if(val2 < 0 && val >= 0) {
		ret = (ssize_t) snprintf(buf, len, "-");
		i++;
	}
	ret = i + (ssize_t) snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val,
				     labs(val2));

	return ret;
}

/**
 * get_cf_samples_pps
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_cf_samples_pps(char *buf, size_t len,
			   const struct channel_info *channel)
{
	return -ENODEV;
}

/**
 * get_cf_sampling_frequency
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_cf_sampling_frequency(char *buf, size_t len,
				  const struct channel_info *channel)
{
	uint32_t sampling_freq_hz;
	ad9361_get_rx_sampling_freq (ad9361_phy, &sampling_freq_hz);

	return (ssize_t) snprintf(buf, len, "%d", (int)sampling_freq_hz);
}

static struct attrtibute_map cf_voltage_read_attrtibute_map[] = {
	{"calibphase", get_cf_calibphase},
	{"calibbias", get_cf_calibbias},
	{"calibscale", get_cf_calibscale},
	{"samples_pps", get_cf_samples_pps},
	{"sampling_frequency", get_cf_sampling_frequency},
	{NULL, NULL},
};

ssize_t ch_read_adc_attr(const char *channel,
			    bool ch_out, const char *attr, char *buf, size_t len) {

	if(channel == strstr(channel, "voltage")) {
		return ch_exec_read_attr(channel, ch_out, attr, buf, len, cf_voltage_read_attrtibute_map);
	}

	return -ENOENT;
}

/**
 * set_cf_calibphase
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_cf_calibphase(char *buf, size_t len,
			  const struct channel_info *channel)
{
	float calib = strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	axi_adc_set_calib_phase(ad9361_phy->rx_adc, channel->ch_num, val, val2);

	return len;
}

/**
 * set_cf_calibbias
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_cf_calibbias(char *buf, size_t len,
			 const struct channel_info *channel)
{
	int32_t val = read_value(buf);
	axi_adc_set_calib_bias(ad9361_phy->rx_adc,
			       channel->ch_num,
			       val,
			       0);

	return len;
}

/**
 * set_cf_calibscale
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_cf_calibscale(char *buf, size_t len,
			  const struct channel_info *channel)
{
	float calib= strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	axi_adc_set_calib_scale(ad9361_phy->rx_adc, channel->ch_num, val, val2);

	return len;
}

/**
 * set_cf_samples_pps
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_cf_samples_pps(char *buf, size_t len,
			   const struct channel_info *channel)
{
	return -ENODEV;
}

/**
 * set_cf_sampling_frequency
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_cf_sampling_frequency(char *buf, size_t len,
				  const struct channel_info *channel)
{
	return -ENODEV;
}

static struct attrtibute_map cf_voltage_write_attrtibute_map[] = {
	{"calibphase", set_cf_calibphase},
	{"calibbias", set_cf_calibbias},
	{"calibscale", set_cf_calibscale},
	{"samples_pps", set_cf_samples_pps},
	{"sampling_frequency", set_cf_sampling_frequency},
	{NULL, NULL},
};

ssize_t ch_write_adc_attr(const char *channel,
			     bool ch_out, const char *attr, const char *buf, size_t len)
{
	if(channel == strstr(channel, "voltage")) {
		return ch_exec_write_attr(channel, ch_out, attr, buf, len, cf_voltage_write_attrtibute_map);
	}

	return -ENOENT;
}

ssize_t read_adc_attr(const char *attr,
			 char *buf, size_t len, bool debug)
{
	return -ENOENT;
}

ssize_t write_adc_attr(const char *attr,
			  const char *buf, size_t len, bool debug)
{
	return -ENOENT;
}

/**
 * transfer_dev_to_mem data from ADC into RAM
 * @param *device name
 * @param bytes_count
 * @return bytes_count
 */
ssize_t transfer_dev_to_mem(const char *device, size_t bytes_count)
{
//	if (!supporter_dev(device))
//		return -ENODEV;
	ad9361_phy->rx_dmac->flags = 0;
	axi_dmac_transfer(ad9361_phy->rx_dmac,
			adc_ddr_baseaddr, bytes_count);
	Xil_DCacheInvalidateRange(adc_ddr_baseaddr,	bytes_count);

	return bytes_count;
}

/**
 * read data from RAM to pbuf, use "capture()" first
 * @param *device name
 * @param *buff where data's are stored
 * @param *offset to the remaining data
 * @param bytes_count
 * @return bytes_count
 */
ssize_t read_dev(const char *device, char *pbuf, size_t offset,
			size_t bytes_count)
{
	memcpy(pbuf, (char *)adc_ddr_baseaddr + offset, bytes_count);

	return bytes_count;
}
