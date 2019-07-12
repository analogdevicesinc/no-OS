/***************************************************************************//**
 *   @file   tinyiiod_dac.c
 *   @brief  Implementation of tinyiiod_dac
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


#include "tinyiiod_dac.h"
#include "ad9361_api.h"
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <tinyiiod_util.h>
#include "axi_dac_core.h"
#include "axi_dmac.h"

extern struct ad9361_rf_phy *ad9361_phy; //todo remove this
static uint32_t dac_ddr_baseaddr; //todo set this

ssize_t tinyiiod_dac_configure(uint32_t dac_ddr_base)
{
	dac_ddr_baseaddr = dac_ddr_baseaddr;
	return 0;
}

/**
 * get_dds_calibscale
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_dds_calibscale(char *buf, size_t len,
			   const struct channel_info *channel)
{
	int32_t val, val2;
	ssize_t ret = axi_dac_dds_get_calib_scale(ad9361_phy->tx_dac, channel->ch_num,
			&val, &val2);
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
 * get_dds_calibphase
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_dds_calibphase(char *buf, size_t len,
			   const struct channel_info *channel)
{
	int32_t val, val2;
	int32_t i = 0;
	ssize_t ret = axi_dac_dds_get_calib_phase(ad9361_phy->tx_dac, channel->ch_num,
			&val, &val2);
	if(ret < 0)
		return ret;
	if(val2 < 0 && val >= 0) {
		i++;
	}
	return i + snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val, labs(val2));
}

/**
 * get_dds_sampling_frequency
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_dds_sampling_frequency(char *buf, size_t len,
				   const struct channel_info *channel)
{
	return -ENODEV;
}

static struct attrtibute_map dds_voltage_read_attrtibute_map[] = {
	{"calibphase", get_dds_calibphase},
	{"calibscale", get_dds_calibscale},
	{"sampling_frequency", get_dds_sampling_frequency},
	{NULL, NULL},
};

/**
 * get_dds_altvoltage_phase
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_dds_altvoltage_phase(char *buf, size_t len,
				 const struct channel_info *channel)
{
	uint32_t phase;
	axi_dac_dds_get_phase(ad9361_phy->tx_dac, channel->ch_num, &phase);
	return snprintf(buf, len, "%"PRIu32"", phase);
}

/**
 * get_dds_altvoltage_scale
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_dds_altvoltage_scale(char *buf, size_t len,
				 const struct channel_info *channel)
{
	int32_t scale;
	axi_dac_dds_get_scale(ad9361_phy->tx_dac, channel->ch_num, &scale);

	return snprintf(buf, len, "%"PRIi32".%.6"PRIi32"", (scale / 1000000),
			(scale % 1000000));
}

/**
 * get_dds_altvoltage_frequency
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_dds_altvoltage_frequency(char *buf, size_t len,
				     const struct channel_info *channel)
{
	uint32_t freq;
	axi_dac_dds_get_frequency(ad9361_phy->tx_dac, channel->ch_num, &freq);

	return snprintf(buf, len, "%"PRIi32"", freq);
}

extern int32_t ad9361_auxdac_get(struct ad9361_rf_phy *phy, int32_t dac);
/**
 * get_dds_altvoltage_raw
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_dds_altvoltage_raw(char *buf, size_t len,
			       const struct channel_info *channel)
{
	return snprintf(buf, len, "%"PRIi32"", ad9361_auxdac_get(ad9361_phy,
			channel->ch_num));
}

/**
 * get_dds_altvoltage_sampling_frequency
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
ssize_t get_dds_altvoltage_sampling_frequency(char *buf, size_t len,
		const struct channel_info *channel)
{
	return -ENODEV;
}

static struct attrtibute_map dds_altvoltage_read_attrtibute_map[] = {
	{"phase", get_dds_altvoltage_phase},
	{"scale", get_dds_altvoltage_scale},
	{"frequency", get_dds_altvoltage_frequency},
	{"raw", get_dds_altvoltage_raw},
	{"sampling_frequency", get_dds_altvoltage_sampling_frequency},
	{NULL, NULL},
};

ssize_t ch_read_dac_attr(const char *channel,
			    bool ch_out, const char *attr, char *buf, size_t len) {

	if(channel == strstr(channel, "voltage")) {
		return ch_exec_read_attr(channel, ch_out, attr, buf, len, dds_voltage_read_attrtibute_map);
	} else if (NULL != strstr(channel, "altvoltage")) {
		return ch_exec_read_attr(channel, ch_out, attr, buf, len, dds_altvoltage_read_attrtibute_map);
	}

	return -ENOENT;
}

/**
 * set_dds_calibscale
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_dds_calibscale(char *buf, size_t len,
			   const struct channel_info *channel)
{
	float calib= strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	axi_dac_dds_set_calib_scale(ad9361_phy->tx_dac, channel->ch_num, val, val2);

	return len;
}

/**
 * set_dds_calibphase
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_dds_calibphase(char *buf, size_t len,
			   const struct channel_info *channel)
{
	float calib = strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	axi_dac_dds_set_calib_phase(ad9361_phy->tx_dac, channel->ch_num, val, val2);

	return len;
}

/**
 * set_dds_sampling_frequency
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_dds_sampling_frequency(char *buf, size_t len,
				   const struct channel_info *channel)
{
	return -ENODEV;
}

static struct attrtibute_map dds_voltage_write_attrtibute_map[] = {
	{"calibphase", set_dds_calibphase},
	{"calibscale", set_dds_calibscale},
	{"sampling_frequency", set_dds_sampling_frequency},
	{NULL, NULL},
};

/**
 * set_dds_altvoltage_phase
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_dds_altvoltage_phase(char *buf, size_t len,
				 const struct channel_info *channel)
{
	uint32_t phase = read_ul_value(buf);
	axi_dac_dds_set_phase(ad9361_phy->tx_dac, channel->ch_num, phase);

	return len;
}

/**
 * set_dds_altvoltage_scale
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_dds_altvoltage_scale(char *buf, size_t len,
				 const struct channel_info *channel)
{
	float fscale = strtof(buf, NULL);
	int32_t scale = fscale * 1000000;
	axi_dac_dds_set_scale(ad9361_phy->tx_dac, channel->ch_num, scale);

	return len;
}

/**
 * set_dds_altvoltage_frequency
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_dds_altvoltage_frequency(char *buf, size_t len,
				     const struct channel_info *channel)
{
	uint32_t freq = read_ul_value(buf);
	axi_dac_dds_set_frequency(ad9361_phy->tx_dac, channel->ch_num, freq);

	return len;
}

extern int32_t ad9361_auxdac_set(struct ad9361_rf_phy *phy, int32_t dac,
				 int32_t val_mV);
/**
 * set_dds_altvoltage_raw
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_dds_altvoltage_raw(char *buf, size_t len,
			       const struct channel_info *channel)
{
	uint32_t dds_mode = read_ul_value(buf);
	if(dds_mode) { 		/* DDS mode selected */
		axi_dac_set_datasel(ad9361_phy->tx_dac, -1, AXI_DAC_DATA_SEL_DDS);
	} else {				/* DMA mode selected */
		axi_dac_set_datasel(ad9361_phy->tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	}

	return len;
}

/**
 * set_dds_altvoltage_sampling_frequency
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
ssize_t set_dds_altvoltage_sampling_frequency(char *buf, size_t len,
		const struct channel_info *channel)
{
	return -ENODEV;
}

static struct attrtibute_map dds_altvoltage_write_attrtibute_map[] = {
	{"phase", set_dds_altvoltage_phase},
	{"scale", set_dds_altvoltage_scale},
	{"frequency", set_dds_altvoltage_frequency},
	{"raw", set_dds_altvoltage_raw},
	{"sampling_frequency", set_dds_altvoltage_sampling_frequency},
	{NULL, NULL},
};

ssize_t ch_write_dac_attr(const char *channel,
			     bool ch_out, const char *attr, const char *buf, size_t len)
{
	if(channel == strstr(channel, "voltage")) {
		return ch_exec_write_attr(channel, ch_out, attr, buf, len, dds_voltage_write_attrtibute_map);
	} else if(NULL != strstr(channel, "altvoltage")) {
		return ch_exec_write_attr(channel, ch_out, attr, buf, len, dds_altvoltage_write_attrtibute_map);
	}

	return -ENOENT;
}

ssize_t read_dac_attr(const char *attr,
			 char *buf, size_t len, bool debug)
{
	return -ENOENT;
}

ssize_t write_dac_attr(const char *attr,
			  const char *buf, size_t len, bool debug)
{
	return -ENOENT;
}

/**
 * transfer_mem_to_dev write data to DAC
 * @param *device name
 * @param *buff
 * @param bytes_count
 * @return bytes_count
 */
ssize_t transfer_mem_to_dev(const char *device, size_t bytes_count)
{
	ad9361_phy->tx_dmac->flags = DMA_CYCLIC;
	ssize_t ret = axi_dmac_transfer(ad9361_phy->tx_dmac, dac_ddr_baseaddr,
					bytes_count);
	if(ret < 0)
		return ret;
	ret = axi_dac_set_datasel(ad9361_phy->tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
	if(ret < 0)
		return ret;

	return bytes_count;
}

/**
 * write data to RAM
 * @param *device name
 * @param *buff
 * @param *offset in memory, used if some data have been already written
 * @param bytes_count
 * @return bytes_count
 */
ssize_t write_dev(const char *device, const char *buf,
			 size_t offset,  size_t bytes_count)
{
	ssize_t ret = axi_dac_set_buff(ad9361_phy->tx_dac, dac_ddr_baseaddr + offset,
				       (uint16_t *)buf,
				       bytes_count);
	if(ret < 0)
		return ret;

	return bytes_count;
}
