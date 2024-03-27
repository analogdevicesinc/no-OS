/***************************************************************************//**
 *   @file   iio_max2201x.c
 *   @brief  Source file of IIO MAX2201X Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

#include "max2201x.h"
#include "iio_max2201x.h"
#include "iio_types.h"

#define MAX2201X_CHANNEL(_addr)			\
	{					\
		.ch_type = IIO_VOLTAGE,		\
		.indexed = 1,			\
		.channel = _addr,		\
		.address = _addr,		\
		.ch_out = true,			\
		.attributes = max2201x_attrs,	\
	}

#define MAX2201X_IIO_CNFG_MASK(x, y)		NO_OS_BIT((x) + (y))

static const char *const max2201x_step_size_avail[8] = {
	[MAX2201X_STEP_SIZE_512BITS] = "512Bits",
	[MAX2201X_STEP_SIZE_1024BITS] = "1024Bits",
	[MAX2201X_STEP_SIZE_4096BITS] = "4096Bits",
	[MAX2201X_STEP_SIZE_16384BITS] = "16384Bits",
	[MAX2201X_STEP_SIZE_26BITS] = "26Bits",
	[MAX2201X_STEP_SIZE_28BITS] = "28Bits",
	[MAX2201X_STEP_SIZE_31BITS] = "31Bits",
	[MAX2201X_STEP_SIZE_35BITS] = "35Bits"
};

static const char *const max2201x_update_rate_avail[8] = {
	[MAX2201X_UPDATE_RATE_4KHZ_2KHZ] = "4KHz/2KHz",
	[MAX2201X_UPDATE_RATE_64KHZ_32KHZ] = "64KHz/32KHz",
	[MAX2201X_UPDATE_RATE_160KHZ_80KHZ] = "160KHz/80KHz",
	[MAX2201X_UPDATE_RATE_200KHZ_100KHZ] = "200KHz/100KHz",
	[MAX2201X_UPDATE_RATE_12KHZ_6KHZ] = "12KHz/6KHz",
	[MAX2201X_UPDATE_RATE_15KHZ_7KHZ] = "15KHz/7.5KHz",
	[MAX2201X_UPDATE_RATE_20KHZ_10KHZ] = "20KHz/10KHz",
	[MAX2201X_UPDATE_RATE_32KHZ_16KHZ] = "32KHz/16KHz"
};

static const char *const max2201x_range_avail[2] = {
	"Under_50ms", "Over_50ms"
};

static const char *const max2201x_enable_avail[2] = {
	"Disabled", "Enabled"
};

static const char *const max2201x_op_mode_avail[8] = {
	[MAX2201X_HIGH_Z] = "High-Z",
	[MAX2201X_INPUT_VOLTAGE_MODE] = "Input_Voltage_Mode",
	[MAX2201X_INPUT_CURRENT_MODE] = "Input_Current_Mode",
	[MAX2201X_OUTPUT_VOLTAGE_NO_MONITORRING] = "Output_Voltage_no_monitor",
	[MAX2201X_OUTPUT_CURRENT_NO_MONITORRING] = "Output_Current_no_monitor",
	[MAX2201X_OUTPUT_VOLTAGE_MONITORRING] = "Output_Voltage_with_monitor",
	[MAX2201X_OUTPUT_CURRENT_MONITORRING] = "Output_Curren_with_montior",
};

static const char *const max2201x_polarity_avail[2] = {
	[MAX2201X_BIPOLAR] = "BIPOLAR",
	[MAX2201X_UNIPOLAR] = "UNIPOLAR",
};

static const char *const max2201x_cm_sense_avail[2] = {
	[MAX2201X_50_OHM_RESISTOR] = "50ohm",
	[MAX2201X_250_OHM_RESISTOR] = "250ohm",
};

static const char *const max2201x_config_enable_avail[4] = {
	"CH1-Disable", "CH1-Enable", "CH2-Disable", "CH2-Enable"
};

static const char *const max2201x_timeout_avail[16] = {
	[MAX2201X_TIMEOUT_100MS] = "100ms",
	[MAX2201X_TIMEOUT_200MS] = "200ms",
	[MAX2201X_TIMEOUT_300MS] = "300ms",
	[MAX2201X_TIMEOUT_400MS] = "400ms",
	[MAX2201X_TIMEOUT_500MS] = "500ms",
	[MAX2201X_TIMEOUT_600MS] = "600ms",
	[MAX2201X_TIMEOUT_700MS] = "700ms",
	[MAX2201X_TIMEOUT_800MS] = "800ms",
	[MAX2201X_TIMEOUT_900MS] = "900ms",
	[MAX2201X_TIMEOUT_1000MS] = "1s",
	[MAX2201X_TIMEOUT_1100MS] = "1.1s",
	[MAX2201X_TIMEOUT_1200MS] = "1.2s",
	[MAX2201X_TIMEOUT_1300MS] = "1.3s",
	[MAX2201X_TIMEOUT_1400MS] = "1.4s",
	[MAX2201X_TIMEOUT_1500MS] = "1.5s",
	[MAX2201X_TIMEOUT_1600MS] = "1.6s"
};

static int max2201x_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int max2201x_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int max2201x_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static int max2201x_iio_write_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int max2201x_iio_write_gain(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int max2201x_iio_write_slew_rate(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int max2201x_iio_write_op_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);

static int max2201x_iio_write_ao_config(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int max2201x_iio_write_config(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int max2201x_iio_write_timeout(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);

static int max2201x_iio_read_interrupt(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int max2201x_iio_write_int_en(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int max2201x_iio_read_available(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int max2201x_iio_reg_read(struct max2201x_iio_desc *iio_desc,
				 uint32_t reg, uint32_t *readval);

static int max2201x_iio_reg_write(struct max2201x_iio_desc *iio_desc,
				  uint32_t reg, uint32_t writeval);

static struct iio_attribute max2201x_attrs[] = {
	{
		.name = "raw",
		.show = max2201x_iio_read_raw,
		.store = max2201x_iio_write_raw,
	},
	{
		.name = "offset",
		.store = max2201x_iio_write_offset,
		.show = max2201x_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max2201x_debug_attrs[] = {
	{
		.name = "ow_detect",
		.store = max2201x_iio_write_config,
		.priv = MAX2201X_IIO_OW_DETECT,
	},
	{
		.name = "ow_detect_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_CONFIG_ENABLE_AVAILABLE,
	},
	{
		.name = "timeout",
		.store = max2201x_iio_write_timeout,
		.priv = MAX2201X_IIO_TIMEOUT,
	},
	{
		.name = "timeout_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_TIMEOUT_AVAILABLE,
	},
	{
		.name = "timeout_cfg",
		.store = max2201x_iio_write_timeout,
		.priv = MAX2201X_IIO_TIMEOUT_CFG,
	},
	{
		.name = "timeout_cfg_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_CONFIG_ENABLE_AVAILABLE,
	},
	{
		.name = "timeout_en",
		.store = max2201x_iio_write_timeout,
		.priv = MAX2201X_IIO_TIMEOUT_EN,
	},
	{
		.name = "timeout_en_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_CONFIG_ENABLE_AVAILABLE,
	},
	{
		.name = "thermal_shutdown",
		.store = max2201x_iio_write_config,
		.priv = MAX2201X_IIO_TH_SHTDN,
	},
	{
		.name = "thermal_shutdown_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_CONFIG_ENABLE_AVAILABLE,
	},
	{
		.name = "overcurrent_shutdown",
		.store = max2201x_iio_write_config,
		.priv = MAX2201X_IIO_OVC_SHTDN,
	},
	{
		.name = "overcurrent_shutdown_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_CONFIG_ENABLE_AVAILABLE,
	},
	{
		.name = "currrent_limit",
		.store = max2201x_iio_write_config,
		.priv = MAX2201X_IIO_CURR_LIM,
	},
	{
		.name = "current_limit_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_CONFIG_ENABLE_AVAILABLE,
	},
	{
		.name = "dac_ref",
		.store = max2201x_iio_write_config,
		.priv = MAX2201X_IIO_DAC_REF,
	},
	{
		.name = "dac_ref_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_ENABLE_AVAILABLE,
	},
	{
		.name = "interrupts",
		.show = max2201x_iio_read_interrupt
	},
	{
		.name = "interrupts_enable",
		.store = max2201x_iio_write_int_en,
	},
	{
		.name = "ld_ctrl",
		.store = max2201x_iio_write_config,
		.priv = MAX2201X_IIO_LD_CTRL,
	},
	{
		.name = "ld_ctrl_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_ENABLE_AVAILABLE,
	},
	{
		.name = "gain",
		.store = max2201x_iio_write_gain,
	},
	{
		.name = "slew_rate",
		.store = max2201x_iio_write_slew_rate,
		.priv = MAX2201X_IIO_SLEW_RATE,
	},
	{
		.name = "slew_rate_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_ENABLE_AVAILABLE,
	},
	{
		.name = "range",
		.store = max2201x_iio_write_slew_rate,
		.priv = MAX2201X_IIO_RANGE,
	},
	{
		.name = "range_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_RANGE_AVAILABLE,
	},
	{
		.name = "step_size",
		.store = max2201x_iio_write_slew_rate,
		.priv = MAX2201X_IIO_STEP_SIZE,
	},
	{
		.name = "step_size_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_STEP_SIZE_AVAILABLE,
	},
	{
		.name = "update_rate",
		.store = max2201x_iio_write_slew_rate,
		.priv = MAX2201X_IIO_UPDATE_RATE,
	},
	{
		.name = "update_rate_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_UPDATE_RATE_AVAILABLE,
	},
	{
		.name = "operation_mode",
		.store = max2201x_iio_write_op_mode,
	},
	{
		.name = "operation_mode_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_OP_MODE_AVAILABLE,
	},
	{
		.name = "polarity",
		.store = max2201x_iio_write_ao_config,
		.priv = MAX2201X_IIO_POLARITY,
	},
	{
		.name = "polarity_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_POLARITY_AVAILABLE,
	},
	{
		.name = "cm_sense",
		.store = max2201x_iio_write_ao_config,
		.priv = MAX2201X_IIO_CM_SENSE,
	},
	{
		.name = "cm_sense_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_CM_SENSE_AVAILABLE,
	},
	{
		.name = "ldac",
		.store = max2201x_iio_write_ao_config,
		.priv = MAX2201X_IIO_LDAC,
	},
	{
		.name = "ldac_available",
		.show = max2201x_iio_read_available,
		.priv = MAX2201X_IIO_ENABLE_AVAILABLE,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device max2201x_iio_dev = {
	.debug_reg_read = (int32_t (*)())max2201x_iio_reg_read,
	.debug_reg_write = (int32_t (*)())max2201x_iio_reg_write,
	.debug_attributes = max2201x_debug_attrs,
};

int max2201x_iio_read_avail(const char *const *array_avail, char *buf,
			    uint32_t len)
{
	uint32_t i;

	for (i = 0; i < len; i++)
		if (!strcmp(buf, array_avail[i]))
			break;

	if (i == len)
		return -EINVAL;

	return i;
}

static int max2201x_iio_read_available(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	uint32_t i, size;
	uint32_t length = 0;
	char **array_avail;

	switch (priv) {
	case MAX2201X_IIO_ENABLE_AVAILABLE:
		array_avail = (char **)max2201x_enable_avail;
		size = NO_OS_ARRAY_SIZE(max2201x_enable_avail);
		break;
	case MAX2201X_IIO_RANGE_AVAILABLE:
		array_avail = (char **)max2201x_range_avail;
		size = NO_OS_ARRAY_SIZE(max2201x_range_avail);
		break;
	case MAX2201X_IIO_STEP_SIZE_AVAILABLE:
		array_avail = (char **)max2201x_step_size_avail;
		size = NO_OS_ARRAY_SIZE(max2201x_step_size_avail);
		break;
	case MAX2201X_IIO_UPDATE_RATE_AVAILABLE:
		array_avail = (char **)max2201x_update_rate_avail;
		size = NO_OS_ARRAY_SIZE(max2201x_update_rate_avail);
		break;
	case MAX2201X_IIO_OP_MODE_AVAILABLE:
		array_avail = (char **)max2201x_op_mode_avail;
		size = NO_OS_ARRAY_SIZE(max2201x_op_mode_avail);
		break;
	case MAX2201X_IIO_POLARITY_AVAILABLE:
		array_avail = (char **)max2201x_polarity_avail;
		size = NO_OS_ARRAY_SIZE(max2201x_polarity_avail);
		break;
	case MAX2201X_IIO_CM_SENSE_AVAILABLE:
		array_avail = (char **)max2201x_cm_sense_avail;
		size = NO_OS_ARRAY_SIZE(max2201x_cm_sense_avail);
		break;
	case MAX2201X_IIO_CONFIG_ENABLE_AVAILABLE:
		array_avail = (char **)max2201x_config_enable_avail;
		size = NO_OS_ARRAY_SIZE(max2201x_config_enable_avail);
		break;
	case MAX2201X_IIO_TIMEOUT_AVAILABLE:
		array_avail = (char **)max2201x_timeout_avail;
		size = NO_OS_ARRAY_SIZE(max2201x_timeout_avail);
		break;
	default:
		return -EINVAL;
	}

	for (i = 0; i < size; i++)
		length += sprintf(buf + length, "%s ", array_avail[i]);

	return length;
}

static int max2201x_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	enum max2201x_channel ch = channel->address;
	struct max2201x_iio_desc *desc = dev;
	int32_t val;
	int ret;

	ret = max2201x_get_data(desc->max2201x_desc, ch, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int max2201x_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	enum max2201x_channel ch = channel->address;
	struct max2201x_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return max2201x_set_data(desc->max2201x_desc, ch, val);
}

static int max2201x_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	enum max2201x_channel ch = channel->address;
	struct max2201x_iio_desc *desc = dev;
	uint32_t offset_val;
	int ret;

	ret = max2201x_reg_read(desc->max2201x_desc,
				ch ? MAX2201X_AO_OFFSET_CORR_CH2 : MAX2201X_AO_OFFSET_CORR_CH1,
				&offset_val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&offset_val);
}

static int max2201x_iio_write_offset(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	enum max2201x_channel ch = channel->address;
	struct max2201x_iio_desc *desc = dev;
	int16_t val;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return max2201x_offset_calib(desc->max2201x_desc, ch, val);
}

static int max2201x_iio_write_gain(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	enum max2201x_channel ch = channel->address;
	struct max2201x_iio_desc *desc = dev;
	uint16_t val;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return max2201x_gain_calib(desc->max2201x_desc, ch, val);
}

static int max2201x_iio_write_slew_rate(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	enum max2201x_channel ch = channel->address;
	struct max2201x_iio_desc *desc = dev;
	uint32_t val;
	enum max2201x_sr_step_size step_size;
	enum max2201x_sr_update_rate update_rate;
	bool enable, hart;
	int ret;

	ret = max2201x_reg_read(desc->max2201x_desc,
				ch ? MAX2201X_AO_SLEW_RATE_CH2 : MAX2201X_AO_SLEW_RATE_CH1, &val);
	if (ret)
		return ret;

	step_size = no_os_field_get(MAX2201X_SR_STEP_SIZE_MASK, val);
	update_rate = no_os_field_get(MAX2201X_SR_UPDATE_RATE_MASK, val);
	enable = no_os_field_get(MAX2201X_SR_EN_MASK, val);
	hart = no_os_field_get(MAX2201X_SR_SEL_MASK, val);

	switch (priv) {
	case MAX2201X_IIO_SLEW_RATE:
		enable = max2201x_iio_read_avail(max2201x_enable_avail, buf,
						 NO_OS_ARRAY_SIZE(max2201x_enable_avail));
		break;
	case MAX2201X_IIO_RANGE:
		hart = max2201x_iio_read_avail(max2201x_range_avail, buf,
					       NO_OS_ARRAY_SIZE(max2201x_range_avail));
		break;
	case MAX2201X_IIO_STEP_SIZE:
		step_size = max2201x_iio_read_avail(max2201x_step_size_avail, buf,
						    NO_OS_ARRAY_SIZE(max2201x_step_size_avail));
		break;
	case MAX2201X_IIO_UPDATE_RATE:
		update_rate = max2201x_iio_read_avail(max2201x_update_rate_avail, buf,
						      NO_OS_ARRAY_SIZE(max2201x_update_rate_avail));
		break;
	default:
		return -EINVAL;
	}

	return max2201x_slew_rate(desc->max2201x_desc, ch, step_size, update_rate,
				  enable, hart);
}

static int max2201x_iio_write_op_mode(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	enum max2201x_channel ch = channel->address;
	struct max2201x_iio_desc *desc = dev;
	uint32_t i;

	i = (uint32_t)max2201x_iio_read_avail(max2201x_op_mode_avail, buf,
					      NO_OS_ARRAY_SIZE(max2201x_op_mode_avail));

	return max2201x_op_mode(desc->max2201x_desc, ch, i);
}

static int max2201x_iio_write_ao_config(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	enum max2201x_channel ch = channel->address;
	struct max2201x_iio_desc *desc = dev;
	int ret;
	uint32_t val;
	enum max2201x_out_mode out_mode;
	enum max2201x_cm_sense cm_sense;
	bool ldac;

	ret = max2201x_reg_read(desc->max2201x_desc, MAX2201X_AO_CNFG, &val);
	if (ret)
		return ret;

	out_mode = no_os_field_get(MAX2201X_UNI_MASK(ch), val);
	cm_sense = no_os_field_get(MAX2201X_CM_SENSE_MASK(ch), val);
	ldac = no_os_field_get(MAX2201X_LD_CNFG_MASK(ch), val);

	switch (priv) {
	case MAX2201X_IIO_POLARITY:
		out_mode = max2201x_iio_read_avail(max2201x_polarity_avail, buf,
						   NO_OS_ARRAY_SIZE(max2201x_polarity_avail));
		break;
	case MAX2201X_IIO_CM_SENSE:
		cm_sense = max2201x_iio_read_avail(max2201x_cm_sense_avail, buf,
						   NO_OS_ARRAY_SIZE(max2201x_cm_sense_avail));
		break;
	case MAX2201X_IIO_LDAC:
		ldac = max2201x_iio_read_avail(max2201x_enable_avail, buf,
					       NO_OS_ARRAY_SIZE(max2201x_enable_avail));
		break;
	default:
		return -EINVAL;
	}

	return max2201x_config(desc->max2201x_desc, ch, out_mode, cm_sense, ldac);
}

static int max2201x_iio_write_config(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max2201x_iio_desc *desc = dev;
	uint32_t i;

	if (priv == MAX2201X_IIO_DAC_REF) {
		i = max2201x_iio_read_avail(max2201x_enable_avail, buf,
					    NO_OS_ARRAY_SIZE(max2201x_enable_avail));

		return max2201x_reg_update(desc->max2201x_desc, MAX2201X_GEN_CNFG,
					   MAX2201X_EXT_DAC_REF_MASK, no_os_field_prep(MAX2201X_EXT_DAC_REF_MASK, i));
	}

	i = (uint32_t)max2201x_iio_read_avail(max2201x_config_enable_avail, buf,
					      NO_OS_ARRAY_SIZE(max2201x_config_enable_avail));

	if (desc->max2201x_desc->chip_id == ID_MAX22018 && i > 1)
		return -EINVAL;

	if (priv == MAX2201X_IIO_LD_CTRL)
		return max2201x_reg_update(desc->max2201x_desc, MAX2201X_AO_CMD,
					   MAX2201X_LD_CNFG_MASK(i / 2), no_os_field_prep(MAX2201X_LD_CNFG_MASK(i / 2),
							   i % 2));

	return max2201x_reg_update(desc->max2201x_desc, MAX2201X_GEN_CNFG,
				   MAX2201X_IIO_CNFG_MASK(priv, i / 2),
				   no_os_field_prep(MAX2201X_IIO_CNFG_MASK(priv, i / 2), i % 2));
}

static int max2201x_iio_write_timeout(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct max2201x_iio_desc *desc = dev;
	uint32_t i, mask;

	switch (priv) {
	case MAX2201X_IIO_TIMEOUT:
		i = (uint32_t)max2201x_iio_read_avail(max2201x_timeout_avail, buf,
						      NO_OS_ARRAY_SIZE(max2201x_timeout_avail));

		return max2201x_reg_update(desc->max2201x_desc, MAX2201X_GEN_CNFG,
					   MAX2201X_TIMEOUT_MASK, no_os_field_prep(MAX2201X_TIMEOUT_MASK, i));
	case MAX2201X_IIO_TIMEOUT_EN:
		mask = MAX2201X_TIMEOUT_EN_MASK;
		break;
	case MAX2201X_IIO_TIMEOUT_CFG:
		mask = MAX2201X_TIMEOUT_CNFG_MASK;
		break;
	default:
		return -EINVAL;
	}
	i = (uint32_t)max2201x_iio_read_avail(max2201x_enable_avail, buf,
					      NO_OS_ARRAY_SIZE(max2201x_enable_avail));

	return max2201x_reg_update(desc->max2201x_desc, MAX2201X_GEN_CNFG, mask,
				   no_os_field_prep(mask, i));
}

static int max2201x_iio_read_interrupt(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct max2201x_iio_desc *desc = dev;
	int ret;
	uint32_t reg_val;

	ret = max2201x_reg_read(desc->max2201x_desc, MAX2201X_GEN_INT, &reg_val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&reg_val);
}

static int max2201x_iio_write_int_en(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max2201x_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	if (val < 0)
		return -EINVAL;

	return max2201x_reg_write(desc->max2201x_desc, MAX2201X_GEN_INTEN,
				  (uint32_t)val);
}

static int max2201x_iio_reg_read(struct max2201x_iio_desc *iio_desc,
				 uint32_t reg, uint32_t *readval)
{
	return max2201x_reg_read(iio_desc->max2201x_desc, reg, readval);
}

static int max2201x_iio_reg_write(struct max2201x_iio_desc *iio_desc,
				  uint32_t reg, uint32_t writeval)
{
	return max2201x_reg_write(iio_desc->max2201x_desc, reg, writeval);
}

int max2201x_iio_setup_channels(struct max2201x_iio_desc *iio_desc)
{
	struct iio_channel *max2201x_iio_channels;

	max2201x_iio_channels = no_os_calloc(iio_desc->max2201x_desc->chip_id ? 1 : 2,
					     sizeof(*max2201x_iio_channels));

	if (!max2201x_iio_channels)
		return -ENOMEM;

	max2201x_iio_channels[MAX2201X_CH1] = (struct iio_channel)MAX2201X_CHANNEL(
			MAX2201X_CH1);

	if (iio_desc->max2201x_desc->chip_id == ID_MAX22017)
		max2201x_iio_channels[MAX2201X_CH2] = (struct iio_channel)MAX2201X_CHANNEL(
				MAX2201X_CH2);

	iio_desc->iio_dev->channels = max2201x_iio_channels;
	iio_desc->iio_dev->num_ch = iio_desc->max2201x_desc->chip_id ? 1 : 2;

	return 0;
}

int max2201x_iio_init(struct max2201x_iio_desc **iio_desc,
		      struct max2201x_iio_desc_init_param *init_param)
{
	struct max2201x_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->max2201x_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max2201x_init(&descriptor->max2201x_desc,
			    init_param->max2201x_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &max2201x_iio_dev;
	ret = max2201x_iio_setup_channels(descriptor);
	if (ret)
		goto free_desc;

	*iio_desc = descriptor;

	return 0;

free_desc:
	max2201x_iio_remove(descriptor);

	return ret;
}

int max2201x_iio_remove(struct max2201x_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	if (iio_desc->iio_dev->channels)
		no_os_free(iio_desc->iio_dev->channels);
	if (iio_desc->max2201x_desc)
		max2201x_remove(iio_desc->max2201x_desc);

	no_os_free(iio_desc);

	return 0;
}
