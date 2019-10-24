/***************************************************************************//**
 *   @file   tinyiiod_axi_dac.c
 *   @brief  Implementation of tinyiiod_axi_dac
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

#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "iio_axi_dac.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "platform_drivers.h"
#include "util.h"

static const char * const  dac_xml =
	"<device id=\"cf-ad9361-dds-core-lpc\" name=\"cf-ad9361-dds-core-lpc\" >"
	"<channel id=\"voltage0\" type=\"output\" >"
	"<scan-element index=\"0\" format=\"le:S16/16&gt;&gt;0\" />"
	"<attribute name=\"calibscale\" filename=\"out_voltage0_calibscale\" />"
	"<attribute name=\"calibphase\" filename=\"out_voltage0_calibphase\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_voltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"voltage1\" type=\"output\" >"
	"<scan-element index=\"1\" format=\"le:S16/16&gt;&gt;0\" />"
	"<attribute name=\"calibphase\" filename=\"out_voltage1_calibphase\" />"
	"<attribute name=\"calibscale\" filename=\"out_voltage1_calibscale\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_voltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"voltage2\" type=\"output\" >"
	"<scan-element index=\"2\" format=\"le:S16/16&gt;&gt;0\" />"
	"<attribute name=\"calibphase\" filename=\"out_voltage2_calibphase\" />"
	"<attribute name=\"calibscale\" filename=\"out_voltage2_calibscale\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_voltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"voltage3\" type=\"output\" >"
	"<scan-element index=\"3\" format=\"le:S16/16&gt;&gt;0\" />"
	"<attribute name=\"calibphase\" filename=\"out_voltage3_calibphase\" />"
	"<attribute name=\"calibscale\" filename=\"out_voltage3_calibscale\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_voltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"altvoltage3\" name=\"TX1_Q_F2\" type=\"output\" >"
	"<attribute name=\"raw\" filename=\"out_altvoltage3_TX1_Q_F2_raw\" />"
	"<attribute name=\"phase\" filename=\"out_altvoltage3_TX1_Q_F2_phase\" />"
	"<attribute name=\"frequency\" filename=\"out_altvoltage3_TX1_Q_F2_frequency\" />"
	"<attribute name=\"scale\" filename=\"out_altvoltage3_TX1_Q_F2_scale\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_altvoltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"altvoltage1\" name=\"TX1_I_F2\" type=\"output\" >"
	"<attribute name=\"phase\" filename=\"out_altvoltage1_TX1_I_F2_phase\" />"
	"<attribute name=\"scale\" filename=\"out_altvoltage1_TX1_I_F2_scale\" />"
	"<attribute name=\"raw\" filename=\"out_altvoltage1_TX1_I_F2_raw\" />"
	"<attribute name=\"frequency\" filename=\"out_altvoltage1_TX1_I_F2_frequency\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_altvoltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"altvoltage0\" name=\"TX1_I_F1\" type=\"output\" >"
	"<attribute name=\"phase\" filename=\"out_altvoltage0_TX1_I_F1_phase\" />"
	"<attribute name=\"scale\" filename=\"out_altvoltage0_TX1_I_F1_scale\" />"
	"<attribute name=\"frequency\" filename=\"out_altvoltage0_TX1_I_F1_frequency\" />"
	"<attribute name=\"raw\" filename=\"out_altvoltage0_TX1_I_F1_raw\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_altvoltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"altvoltage7\" name=\"TX2_Q_F2\" type=\"output\" >"
	"<attribute name=\"raw\" filename=\"out_altvoltage7_TX2_Q_F2_raw\" />"
	"<attribute name=\"phase\" filename=\"out_altvoltage7_TX2_Q_F2_phase\" />"
	"<attribute name=\"scale\" filename=\"out_altvoltage7_TX2_Q_F2_scale\" />"
	"<attribute name=\"frequency\" filename=\"out_altvoltage7_TX2_Q_F2_frequency\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_altvoltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"altvoltage6\" name=\"TX2_Q_F1\" type=\"output\" >"
	"<attribute name=\"phase\" filename=\"out_altvoltage6_TX2_Q_F1_phase\" />"
	"<attribute name=\"scale\" filename=\"out_altvoltage6_TX2_Q_F1_scale\" />"
	"<attribute name=\"frequency\" filename=\"out_altvoltage6_TX2_Q_F1_frequency\" />"
	"<attribute name=\"raw\" filename=\"out_altvoltage6_TX2_Q_F1_raw\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_altvoltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"altvoltage5\" name=\"TX2_I_F2\" type=\"output\" >"
	"<attribute name=\"raw\" filename=\"out_altvoltage5_TX2_I_F2_raw\" />"
	"<attribute name=\"frequency\" filename=\"out_altvoltage5_TX2_I_F2_frequency\" />"
	"<attribute name=\"phase\" filename=\"out_altvoltage5_TX2_I_F2_phase\" />"
	"<attribute name=\"scale\" filename=\"out_altvoltage5_TX2_I_F2_scale\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_altvoltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"altvoltage2\" name=\"TX1_Q_F1\" type=\"output\" >"
	"<attribute name=\"raw\" filename=\"out_altvoltage2_TX1_Q_F1_raw\" />"
	"<attribute name=\"phase\" filename=\"out_altvoltage2_TX1_Q_F1_phase\" />"
	"<attribute name=\"frequency\" filename=\"out_altvoltage2_TX1_Q_F1_frequency\" />"
	"<attribute name=\"scale\" filename=\"out_altvoltage2_TX1_Q_F1_scale\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_altvoltage_sampling_frequency\" />"
	"</channel>"
	"<channel id=\"altvoltage4\" name=\"TX2_I_F1\" type=\"output\" >"
	"<attribute name=\"frequency\" filename=\"out_altvoltage4_TX2_I_F1_frequency\" />"
	"<attribute name=\"phase\" filename=\"out_altvoltage4_TX2_I_F1_phase\" />"
	"<attribute name=\"raw\" filename=\"out_altvoltage4_TX2_I_F1_raw\" />"
	"<attribute name=\"scale\" filename=\"out_altvoltage4_TX2_I_F1_scale\" />"
	"<attribute name=\"sampling_frequency\" filename=\"out_altvoltage_sampling_frequency\" />"
	"</channel>"
	"<buffer-attribute name=\"watermark\" />"
	"<buffer-attribute name=\"data_available\" />"
	"<debug-attribute name=\"direct_reg_access\" />"
	"</device>";

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

ssize_t tinyiiod_axi_dac_init(struct tinyiiod_dac **tinyiiod_dac,
		struct tinyiiod_dac_init_par *init)
{
	*tinyiiod_dac = calloc(1, sizeof(*tinyiiod_dac));
	if (!(*tinyiiod_dac))
		return FAILURE;
	(*tinyiiod_dac)->dac = init->dac;
	(*tinyiiod_dac)->dmac = init->dmac;
	(*tinyiiod_dac)->dac_ddr_base = init->dac_ddr_base;

	return SUCCESS;
}

ssize_t tinyiiod_axi_dac_remove(struct tinyiiod_dac *tinyiiod_dac)
{

	free(tinyiiod_dac);

	return SUCCESS;
}

/**
 * get_dds_calibscale
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_voltage_calibscale(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel)
{
	int32_t val, val2;
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	ssize_t ret = axi_dac_dds_get_calib_scale(tinyiiod_dac->dac, channel->ch_num,
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
static ssize_t get_voltage_calibphase(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel)
{
	int32_t val, val2;
	int32_t i = 0;
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	ssize_t ret = axi_dac_dds_get_calib_phase(tinyiiod_dac->dac, channel->ch_num,
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
static ssize_t get_voltage_sampling_frequency(void *device, char *buf, size_t len,
		const struct iio_ch_info *channel)
{
	return -ENODEV;
}

/**
 * get_dds_altvoltage_phase
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_altvoltage_phase(void *device, char *buf, size_t len,
					const struct iio_ch_info *channel)
{
	uint32_t phase;
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	axi_dac_dds_get_phase(tinyiiod_dac->dac, channel->ch_num, &phase);
	return snprintf(buf, len, "%"PRIu32"", phase);
}

/**
 * get_dds_altvoltage_scale
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_altvoltage_scale(void *device, char *buf, size_t len,
					const struct iio_ch_info *channel)
{
	int32_t scale;
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	axi_dac_dds_get_scale(tinyiiod_dac->dac, channel->ch_num, &scale);

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
static ssize_t get_altvoltage_frequency(void *device, char *buf, size_t len,
		const struct iio_ch_info *channel)
{
	uint32_t freq;
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	axi_dac_dds_get_frequency(tinyiiod_dac->dac, channel->ch_num, &freq);

	return snprintf(buf, len, "%"PRIi32"", freq);
}

/**
 * get_dds_altvoltage_raw
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_altvoltage_raw(void *device, char *buf, size_t len,
				      const struct iio_ch_info *channel)
{
	return -ENODEV;
}

/**
 * get_dds_altvoltage_sampling_frequency
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_altvoltage_sampling_frequency(void *device, char *buf,
		size_t len,
		const struct iio_ch_info *channel)
{
	return -ENODEV;
}

/**
 * set_dds_calibscale
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_voltage_calibscale(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel)
{
	float calib= strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	axi_dac_dds_set_calib_scale(tinyiiod_dac->dac, channel->ch_num, val, val2);

	return len;
}

/**
 * set_dds_calibphase
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_voltage_calibphase(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel)
{
	float calib = strtof(buf, NULL);
	int32_t val = (int32_t)calib;
	int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	axi_dac_dds_set_calib_phase(tinyiiod_dac->dac, channel->ch_num, val, val2);

	return len;
}

/**
 * set_dds_sampling_frequency
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_voltage_sampling_frequency(void *device, char *buf, size_t len,
		const struct iio_ch_info *channel)
{
	return -ENODEV;
}

/**
 * set_dds_altvoltage_phase
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_altvoltage_phase(void *device, char *buf, size_t len,
					const struct iio_ch_info *channel)
{
	uint32_t phase = read_ul_value(buf);
	struct tinyiiod_dac * iiod_dac = (struct tinyiiod_dac *)device;
	axi_dac_dds_set_phase(iiod_dac->dac, channel->ch_num, phase);

	return len;
}

/**
 * set_dds_altvoltage_scale
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_altvoltage_scale(void *device, char *buf, size_t len,
					const struct iio_ch_info *channel)
{
	float fscale = strtof(buf, NULL);
	int32_t scale = fscale * 1000000;
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	axi_dac_dds_set_scale(tinyiiod_dac->dac, channel->ch_num, scale);

	return len;
}

/**
 * set_dds_altvoltage_frequency
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_altvoltage_frequency(void *device, char *buf, size_t len,
		const struct iio_ch_info *channel)
{
	uint32_t freq = read_ul_value(buf);
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	axi_dac_dds_set_frequency(tinyiiod_dac->dac, channel->ch_num, freq);

	return len;
}

/**
 * set_dds_altvoltage_raw
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_altvoltage_raw(void *device, char *buf, size_t len,
				      const struct iio_ch_info *channel)
{
	uint32_t dds_mode = read_ul_value(buf);
	struct tinyiiod_dac* tinyiiod_dac = (struct tinyiiod_dac*)device;
	if(dds_mode) { 		/* DDS mode selected */
		axi_dac_set_datasel(tinyiiod_dac->dac, -1, AXI_DAC_DATA_SEL_DDS);
	} else {				/* DMA mode selected */
		axi_dac_set_datasel(tinyiiod_dac->dac, -1, AXI_DAC_DATA_SEL_DMA);
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
static ssize_t set_altvoltage_sampling_frequency(void *device, char *buf,
		size_t len,
		const struct iio_ch_info *channel)
{
	return -ENODEV;
}

static struct iio_attribute iio_attr_voltage_calibphase = {
	.name = "calibphase",
	.show = get_voltage_calibphase,
	.store = set_voltage_calibphase,
};

static struct iio_attribute iio_attr_voltage_calibscale = {
	.name = "calibscale",
	.show = get_voltage_calibscale,
	.store = set_voltage_calibscale,
};

static struct iio_attribute iio_attr_voltage_sampling_frequency = {
	.name = "sampling_frequency",
	.show = get_voltage_sampling_frequency,
	.store = set_voltage_sampling_frequency,
};

static struct iio_attribute iio_attr_altvoltage_raw = {
	.name = "raw",
	.show = get_altvoltage_raw,
	.store = set_altvoltage_raw,
};
static struct iio_attribute iio_attr_altvoltage_phase = {
	.name = "phase",
	.show = get_altvoltage_phase,
	.store = set_altvoltage_phase,
};
static struct iio_attribute iio_attr_altvoltage_frequency = {
	.name = "frequency",
	.show = get_altvoltage_frequency,
	.store = set_altvoltage_frequency,
};
static struct iio_attribute iio_attr_altvoltage_scale = {
	.name = "scale",
	.show = get_altvoltage_scale,
	.store = set_altvoltage_scale,
};
static struct iio_attribute iio_attr_altvoltage_sampling_frequency = {
	.name = "sampling_frequency",
	.show = get_altvoltage_sampling_frequency,
	.store = set_altvoltage_sampling_frequency,
};

static struct iio_attribute *iio_voltage_attributes[] = {
	&iio_attr_voltage_calibphase,
	&iio_attr_voltage_calibscale,
	&iio_attr_voltage_sampling_frequency,
	NULL,
};
static struct iio_attribute *iio_altvoltage_attributes[] = {
	&iio_attr_altvoltage_raw,
	&iio_attr_altvoltage_phase,
	&iio_attr_altvoltage_frequency,
	&iio_attr_altvoltage_scale,
	&iio_attr_altvoltage_sampling_frequency,
	NULL,
};
static struct iio_channel iio_channel_voltage0 = {
	.name = "voltage0",
	.attributes = iio_voltage_attributes,
};
static struct iio_channel iio_channel_voltage1 = {
	.name = "voltage1",
	.attributes = iio_voltage_attributes,
};
static struct iio_channel iio_channel_voltage2 = {
	.name = "voltage2",
	.attributes = iio_voltage_attributes,
};
static struct iio_channel iio_channel_voltage3 = {
	.name = "voltage3",
	.attributes = iio_voltage_attributes,
};

static struct iio_channel iio_channel_altvoltage0 = {
	.name = "altvoltage0",
	.attributes = iio_altvoltage_attributes,
};
static struct iio_channel iio_channel_altvoltage1 = {
	.name = "altvoltage1",
	.attributes = iio_altvoltage_attributes,
};
static struct iio_channel iio_channel_altvoltage2 = {
	.name = "altvoltage2",
	.attributes = iio_altvoltage_attributes,
};
static struct iio_channel iio_channel_altvoltage3 = {
	.name = "altvoltage3",
	.attributes = iio_altvoltage_attributes,
};
static struct iio_channel iio_channel_altvoltage4 = {
	.name = "altvoltage4",
	.attributes = iio_altvoltage_attributes,
};
static struct iio_channel iio_channel_altvoltage5 = {
	.name = "altvoltage5",
	.attributes = iio_altvoltage_attributes,
};
static struct iio_channel iio_channel_altvoltage6 = {
	.name = "altvoltage6",
	.attributes = iio_altvoltage_attributes,
};
static struct iio_channel iio_channel_altvoltage7 = {
	.name = "altvoltage7",
	.attributes = iio_altvoltage_attributes,
};

static struct iio_channel *iio_dac_channels[] = {
	&iio_channel_voltage0,
	&iio_channel_voltage1,
	&iio_channel_voltage2,
	&iio_channel_voltage3,
	&iio_channel_altvoltage0,
	&iio_channel_altvoltage1,
	&iio_channel_altvoltage2,
	&iio_channel_altvoltage3,
	&iio_channel_altvoltage4,
	&iio_channel_altvoltage5,
	&iio_channel_altvoltage6,
	&iio_channel_altvoltage7,
	NULL,
};

static struct iio_device *iio_dac_device;

/**
 * get_ch_write_dac_attr_map
 * get map between attribute name and corresponding function
 * @return map
 */
struct iio_device *get_dac_device(const char *device_name)
{
	iio_dac_device = calloc(1, sizeof(struct iio_device));
	if (!iio_dac_device)
		return NULL;
	iio_dac_device->name = device_name;
	iio_dac_device->channels = iio_dac_channels;

	return iio_dac_device;
}

/**
 * transfer_mem_to_dev write data to DAC
 * @param *device name
 * @param *buff
 * @param bytes_count
 * @return bytes_count
 */
ssize_t dac_transfer_mem_to_dev(struct axi_dmac	*tx_dmac,
				uint32_t dac_ddr_baseaddr, size_t bytes_count)
{
	tx_dmac->flags = DMA_CYCLIC;
	ssize_t ret = axi_dmac_transfer(tx_dmac, dac_ddr_baseaddr,
					bytes_count);
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
ssize_t dac_write_dev(struct tinyiiod_dac *iiod_dac, const char *buf,
		      size_t offset,  size_t bytes_count)
{
	ssize_t ret = axi_dac_set_datasel(iiod_dac->dac, -1, AXI_DAC_DATA_SEL_DMA);
	if(ret < 0)
		return ret;

	ret = axi_dac_set_buff(iiod_dac->dac, iiod_dac->dac_ddr_base + offset,
			       (uint16_t *)buf,
			       bytes_count);
	if(ret < 0)
		return ret;

	return bytes_count;
}

ssize_t get_dac_xml(char** xml, const char *device_name, uint8_t ch_no)
{
	*xml = (char *)dac_xml; // todo

	return 0;
}
