/***************************************************************************//**
 *   @file   iio_ltc7871.c
 *   @brief  Source file for the LTC7871 IIO Driver
 *   @author Aldrin Abacan (aldrin.abacan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "ltc7871.h"
#include "iio_ltc7871.h"

static const char *const ltc7871_pin_avail[] = {
	"Low",
	"High",
};

static const char *const ltc7871_enable_avail[] = {
	"Disabled",
	"Enabled",
};

static const char *const ltc7871_ssfm_fsr_avail[] = {
	"+/-12",
	"+/-15",
	"+/-10",
	"+/-8"
} ;

static const char *const ltc7871_ssfm_msf_avail[] = {
	"Controller Switching Frequency/512",
	"Controller Switching Frequency/1024",
	"Controller Switching Frequency/2048",
	"Controller Switching Frequency/4096",
	"Controller Switching Frequency/256",
	"Controller Switching Frequency/128",
	"Controller Switching Frequency/64",
	"Controller Switching Frequency/512"
} ;

static const char *const ltc7871_fault_avail[] = {
	"No_Fault",
	"Fault",
};

static const char *const ltc7871_pgood_avail[] = {
	"Exceeds_+/-10p",
	"Within_+/-10p",
};

static const char *const ltc7871_ss_done_avail[] = {
	"Not_finished",
	"Finished",
};

static const char *const ltc7871_max_current_avail[] = {
	"max_current_not_reached",
	"max_current_reached"
};

static const char *const ltc7871_sercur_warning_avail[] = {
	"Below_1.25V",
	"Above_1.25V",
};

static const char *const ltc7871_drvcc_set_avail[] = {
	"5V_DRVCC",
	"8V_DRVCC",
	"10V_DRVCC"
};

static const char *const ltc7871_ilim_set_avail[] = {
	"Max current sense threshold is programmed to 10mV.",
	"Max current sense threshold is programmed to 20mV.",
	"Max current sense threshold is programmed to 30mV.",
	"Max current sense threshold is programmed to 40mV.",
	"Max current sense threshold is programmed to 50mV.",
	"Max current sense threshold is programmed to 60mV."
};

static const char *const ltc7871_burst_avail[] = {
	"Burst Mode OFF.",
	"Burst Mode ON."
};

static const char *const ltc7871_dcm_avail[] = {
	"DCM Mode OFF.",
	"DCM Mode ON."
};

static const char *const ltc7871_hiz_avail[] = {
	"Hi-Z Mode OFF.",
	"Hi-Z Mode ON."
};

static const char *const ltc7871_sprd_avail[] = {
	"spectrum spread Mode OFF.",
	"spectrum spread Mode ON."
};

static const char *const ltc7871_buck_boost_avail[] = {
	"Boost Mode.",
	"Buck Mode."
};

static const char *const ltc7871_wp_avail[] = {
	"Write allowed on IDAC and MFR_SSM registers.",
	"Write inhibited on IDAC and MFR_SSM registers."
};

enum ltc7871_iio_mfr_fault_attrs {
	LTC7871_VLOW_OV,
	LTC7871_VHIGH_OV,
	LTC7871_VHIGH_UV,
	LTC7871_DRVCC_UV,
	LTC7871_V5_UV,
	LTC7871_VREF_BAD,
	LTC7871_OVER_TEMP
};

enum ltc7871_iio_mfr_oc_fault_attrs {
	LTC7871_OC_FAULT_6,
	LTC7871_OC_FAULT_5,
	LTC7871_OC_FAULT_4,
	LTC7871_OC_FAULT_3,
	LTC7871_OC_FAULT_2,
	LTC7871_OC_FAULT_1,
	LTC7871_OC_FAULT_0
};

enum ltc7871_iio_mfr_noc_fault_attrs {
	LTC7871_NOC_FAULT_6,
	LTC7871_NOC_FAULT_5,
	LTC7871_NOC_FAULT_4,
	LTC7871_NOC_FAULT_3,
	LTC7871_NOC_FAULT_2,
	LTC7871_NOC_FAULT_1,
	LTC7871_NOC_FAULT_0
};

enum ltc7871_iio_mfr_status_attrs {
	LTC7871_PGOOD,
	LTC7871_MAX_CURRENT,
	LTC7871_SS_DONE
};

enum ltc7871_iio_mfr_config1_attrs {
	LTC7871_SERCUR_WARNING,
	LTC7871_DRVCC_SET,
	LTC7871_ILIM_SET
};

enum ltc7871_iio_mfr_config2_attrs {
	LTC7871_BURST,
	LTC7871_DCM,
	LTC7871_HIZ,
	LTC7871_SPRD,
	LTC7871_BUCK_BOOST
};

enum ltc7871_iio_mfr_chip_ctrl_attrs {
	LTC7871_CML,
	LTC7871_RESET,
	LTC7871_WP
};

enum ltc7871_iio_mfr_ssfm_attrs {
	LTC7871_FSR,
	LTC7871_MSF
};
static int ltc7871_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval);
static int ltc7871_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval);
static int ltc7871_iio_read_mfr_fault(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_mfr_oc_fault(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_mfr_noc_fault(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_mfr_status(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_mfr_config1(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_mfr_config2(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_reset(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_clear_pec_fault(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_pec_fault(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_set_write_protect(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_write_protect(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_mfr_idac_vlow(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_write_mfr_idac_vlow(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_mfr_idac_vhigh(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_write_mfr_idac_vhigh(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_read_mfr_idac_setcur(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_write_mfr_idac_setcur(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_get_freq_spread_range(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_set_freq_spread_range(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_get_mod_freq(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_set_mod_freq(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);
static int ltc7871_iio_get_pwmen_pin(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel);
static int ltc7871_iio_set_pwmen_pin(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel);

static struct iio_channel const ltc7871_channels[] = {
	//no available channel
};

static struct iio_attribute ltc7871_debug_attrs[] = {
	{
		.name = "vlow_ov",
		.show = ltc7871_iio_read_mfr_fault,
		.priv = LTC7871_VLOW_OV
	},
	{
		.name = "vhigh_ov",
		.show = ltc7871_iio_read_mfr_fault,
		.priv = LTC7871_VHIGH_OV
	},
	{
		.name = "vhigh_uv",
		.show = ltc7871_iio_read_mfr_fault,
		.priv = LTC7871_VHIGH_UV
	},
	{
		.name = "drvcc_uv",
		.show = ltc7871_iio_read_mfr_fault,
		.priv = LTC7871_DRVCC_UV
	},
	{
		.name = "v5_uv",
		.show = ltc7871_iio_read_mfr_fault,
		.priv = LTC7871_V5_UV
	},
	{
		.name = "vref_bad",
		.show = ltc7871_iio_read_mfr_fault,
		.priv = LTC7871_VREF_BAD
	},
	{
		.name = "over_temp",
		.show = ltc7871_iio_read_mfr_fault,
		.priv = LTC7871_OVER_TEMP
	},
	{
		.name = "oc_fault_1",
		.show = ltc7871_iio_read_mfr_oc_fault,
		.priv = LTC7871_OC_FAULT_1
	},
	{
		.name = "oc_fault_2",
		.show = ltc7871_iio_read_mfr_oc_fault,
		.priv = LTC7871_OC_FAULT_2
	},
	{
		.name = "oc_fault_3",
		.show = ltc7871_iio_read_mfr_oc_fault,
		.priv = LTC7871_OC_FAULT_3
	},
	{
		.name = "oc_fault_4",
		.show = ltc7871_iio_read_mfr_oc_fault,
		.priv = LTC7871_OC_FAULT_4
	},
	{
		.name = "oc_fault_5",
		.show = ltc7871_iio_read_mfr_oc_fault,
		.priv = LTC7871_OC_FAULT_5
	},
	{
		.name = "oc_fault_6",
		.show = ltc7871_iio_read_mfr_oc_fault,
		.priv = LTC7871_OC_FAULT_6
	},
	{
		.name = "noc_fault_1",
		.show = ltc7871_iio_read_mfr_noc_fault,
		.priv = LTC7871_NOC_FAULT_1
	},
	{
		.name = "noc_fault_2",
		.show = ltc7871_iio_read_mfr_noc_fault,
		.priv = LTC7871_NOC_FAULT_2
	},
	{
		.name = "noc_fault_3",
		.show = ltc7871_iio_read_mfr_noc_fault,
		.priv = LTC7871_NOC_FAULT_3
	},
	{
		.name = "noc_fault_4",
		.show = ltc7871_iio_read_mfr_noc_fault,
		.priv = LTC7871_NOC_FAULT_4
	},
	{
		.name = "noc_fault_5",
		.show = ltc7871_iio_read_mfr_noc_fault,
		.priv = LTC7871_NOC_FAULT_5
	},
	{
		.name = "noc_fault_6",
		.show = ltc7871_iio_read_mfr_noc_fault,
		.priv = LTC7871_NOC_FAULT_6
	},
	{
		.name = "ss_done",
		.show = ltc7871_iio_read_mfr_status,
		.priv = LTC7871_SS_DONE
	},
	{
		.name = "max_current",
		.show = ltc7871_iio_read_mfr_status,
		.priv = LTC7871_MAX_CURRENT
	},
	{
		.name = "pgood",
		.show = ltc7871_iio_read_mfr_status,
		.priv = LTC7871_PGOOD
	},
	{
		.name = "sercur_warning",
		.show = ltc7871_iio_read_mfr_config1,
		.priv = LTC7871_SERCUR_WARNING
	},
	{
		.name = "drvcc_set",
		.show = ltc7871_iio_read_mfr_config1,
		.priv = LTC7871_DRVCC_SET
	},
	{
		.name = "ilim_set",
		.show = ltc7871_iio_read_mfr_config1,
		.priv = LTC7871_ILIM_SET
	},
	{
		.name = "burst",
		.show = ltc7871_iio_read_mfr_config2,
		.priv = LTC7871_BURST
	},
	{
		.name = "dcm",
		.show = ltc7871_iio_read_mfr_config2,
		.priv = LTC7871_DCM
	},
	{
		.name = "hiz",
		.show = ltc7871_iio_read_mfr_config2,
		.priv = LTC7871_HIZ
	},
	{
		.name = "sprd",
		.show = ltc7871_iio_read_mfr_config2,
		.priv = LTC7871_SPRD
	},
	{
		.name = "buck_boost",
		.show = ltc7871_iio_read_mfr_config2,
		.priv = LTC7871_BUCK_BOOST
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltc7871_global_attrs[] = {
	{
		.name = "reset",
		.store = ltc7871_iio_reset,
		.priv =  LTC7871_RESET
	},
	{
		.name = "pec_fault",
		.store = ltc7871_iio_clear_pec_fault,
		.show = ltc7871_iio_read_pec_fault,
		.priv =  LTC7871_CML
	},
	{
		.name = "write_protect",
		.store = ltc7871_iio_set_write_protect,
		.show = ltc7871_iio_read_write_protect,
		.priv =  LTC7871_WP
	},
	{
		.name = "idac_vlow",
		.store = ltc7871_iio_write_mfr_idac_vlow,
		.show = ltc7871_iio_read_mfr_idac_vlow
	},
	{
		.name = "idac_vhigh",
		.store = ltc7871_iio_write_mfr_idac_vhigh,
		.show = ltc7871_iio_read_mfr_idac_vhigh
	},
	{
		.name = "idac_setcur",
		.store = ltc7871_iio_write_mfr_idac_setcur,
		.show = ltc7871_iio_read_mfr_idac_setcur
	},
	{
		.name = "freq_spread_range",
		.store = ltc7871_iio_set_freq_spread_range,
		.show = ltc7871_iio_get_freq_spread_range
	},
	{
		.name = "mod_freq",
		.store = ltc7871_iio_set_mod_freq,
		.show = ltc7871_iio_get_mod_freq
	},
	{
		.name = "pwmen",
		.store = ltc7871_iio_set_pwmen_pin,
		.show = ltc7871_iio_get_pwmen_pin
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device ltc7871_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ltc7871_channels),
	.channels = ltc7871_channels,
	.attributes = ltc7871_global_attrs,
	.debug_attributes = ltc7871_debug_attrs,
	.debug_reg_read = (int32_t (*)())ltc7871_iio_reg_read,
	.debug_reg_write = (int32_t (*)())ltc7871_iio_reg_write,
};

/**
 * @brief Get the mask for the MFR_FAULT register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the MFR_FAULT command register.
 */
uint32_t ltc7871_iio_get_mask_mfr_fault(enum ltc7871_iio_mfr_fault_attrs attr)
{
	switch (attr) {
	case LTC7871_VLOW_OV:
		return LTC7871_VLOW_OV_MASK;
	case LTC7871_VHIGH_OV:
		return LTC7871_VHIGH_OV_MASK;
	case LTC7871_VHIGH_UV:
		return LTC7871_VHIGH_UV_MASK;
	case LTC7871_DRVCC_UV:
		return LTC7871_DRVCC_UV_MASK;
	case LTC7871_V5_UV:
		return LTC7871_V5_UV_MASK;
	case LTC7871_VREF_BAD:
		return LTC7871_VREF_BAD_MASK;
	case LTC7871_OVER_TEMP:
		return LTC7871_OVER_TEMP_MASK;
	default:
		return LTC7871_MFR_FAULT_MASK;
	}
}

/**
 * @brief Get the mask for the MFR_OC_FAULT register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the MFR_OC_FAULT command register.
 */
uint32_t ltc7871_iio_get_mask_mfr_oc_fault(enum ltc7871_iio_mfr_oc_fault_attrs
		attr)
{
	switch (attr) {
	case LTC7871_OC_FAULT_6:
		return LTC7871_OC_FAULT_6_MASK;
	case LTC7871_OC_FAULT_5:
		return LTC7871_OC_FAULT_5_MASK;
	case LTC7871_OC_FAULT_4:
		return LTC7871_OC_FAULT_4_MASK;
	case LTC7871_OC_FAULT_3:
		return LTC7871_OC_FAULT_3_MASK;
	case LTC7871_OC_FAULT_2:
		return LTC7871_OC_FAULT_2_MASK;
	case LTC7871_OC_FAULT_1:
		return LTC7871_OC_FAULT_1_MASK;
	default:
		return LTC7871_MFR_OC_FAULT_MASK;
	}
}

/**
 * @brief Get the mask for the MFR_NOC_FAULT register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the MFR_NOC_FAULT command register.
 */
uint32_t ltc7871_iio_get_mask_mfr_noc_fault(enum ltc7871_iio_mfr_noc_fault_attrs
		attr)
{
	switch (attr) {
	case LTC7871_NOC_FAULT_6:
		return LTC7871_NOC_FAULT_6_MASK;
	case LTC7871_NOC_FAULT_5:
		return LTC7871_NOC_FAULT_5_MASK;
	case LTC7871_NOC_FAULT_4:
		return LTC7871_NOC_FAULT_4_MASK;
	case LTC7871_NOC_FAULT_3:
		return LTC7871_NOC_FAULT_3_MASK;
	case LTC7871_NOC_FAULT_2:
		return LTC7871_NOC_FAULT_2_MASK;
	case LTC7871_NOC_FAULT_1:
		return LTC7871_NOC_FAULT_1_MASK;
	default:
		return LTC7871_MFR_NOC_FAULT_MASK;
	}
}

/**
 * @brief Get the mask for the MFR_STATUS register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the MFR_STATUS command register.
 */
uint32_t ltc7871_iio_get_mask_mfr_status(enum ltc7871_iio_mfr_status_attrs attr)
{
	switch (attr) {
	case LTC7871_SS_DONE:
		return LTC7871_SS_DONE_MASK;
	case LTC7871_MAX_CURRENT:
		return LTC7871_MAX_CURRENT_MASK;
	case LTC7871_PGOOD:
		return LTC7871_PGOOD_MASK;
	default:
		return LTC7871_MFR_STATUS_MASK;
	}
}

/**
 * @brief Get the mask for the MFR_CONFIG1 register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the MFR_CONFIG1 command register.
 */
uint32_t ltc7871_iio_get_mask_mfr_config1(enum ltc7871_iio_mfr_config1_attrs
		attr)
{
	switch (attr) {
	case LTC7871_SERCUR_WARNING:
		return LTC7871_SERCUR_WARNING_MASK;
	case LTC7871_DRVCC_SET:
		return LTC7871_DRVCC_SET_MASK;
	case LTC7871_ILIM_SET:
		return LTC7871_ILIM_SET_MASK;
	default:
		return LTC7871_MFR_CONFIG1_MASK;
	}
}

/**
 * @brief Get the mask for the MFR_CONFIG2 register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the MFR_CONFIG2 command register.
 */
uint32_t ltc7871_iio_get_mask_mfr_config2(enum ltc7871_iio_mfr_config2_attrs
		attr)
{
	switch (attr) {
	case LTC7871_BURST:
		return LTC7871_BURST_MASK;
	case LTC7871_DCM:
		return LTC7871_DCM_MASK;
	case LTC7871_HIZ:
		return LTC7871_HIZ_MASK;
	case LTC7871_SPRD:
		return LTC7871_SPRD_MASK;
	case LTC7871_BUCK_BOOST:
		return LTC7871_BUCK_BOOST_MASK;
	default:
		return LTC7871_MFR_CONFIG2_MASK;
	}
}

/**
 * @brief Get the mask for the MFR_CHIP_CTRL register.
 * @param attr - The attribute for which the mask is needed.
 * @return The mask for the MFR_CHIP_CTRL command register.
 */
uint32_t ltc7871_iio_get_mask_mfr_chip_ctrl(enum ltc7871_iio_mfr_chip_ctrl_attrs
		attr)
{
	switch (attr) {
	case LTC7871_CML:
		return LTC7871_CML_MASK;
	case LTC7871_RESET:
		return LTC7871_RESET_MASK;
	case LTC7871_WP:
		return LTC7871_WP_MASK;
	default:
		return LTC7871_MFR_CHIP_CTRL_MASK;
	}
}

/**
 * @brief Wrapper function for reading data from the LTC7871 device.
 * @param desc - The LTC7871 device structure.
 * @param reg - The register address to read from.
 * @param readval - The data read from the register.
 * @return 0 in case of success, an error code otherwise.
 */
static int ltc7871_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev || !readval)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	return ltc7871_reg_read(ltc7871, reg, readval);
}

/**
 * @brief Wrapper function for writing data to the LTC7871 device.
 * @param desc - The LTC7871 device structure.
 * @param reg - The register address to write to.
 * @param writeval - The data to write to the register.
 * @return 0 in case of success, an error code otherwise.
 */
static int ltc7871_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	return ltc7871_reg_write(ltc7871, reg, writeval);
}

/**
 * @brief Handles the read request for MFR_FAULT register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_read_mfr_fault(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t reg_data;
	uint32_t mask = ltc7871_iio_get_mask_mfr_fault(priv);
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mfr_oc_fault(ltc7871, mask, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", ltc7871_fault_avail[value]);
}

/**
 * @brief Handles the read request for MFR_OC_FAULT register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_read_mfr_oc_fault(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t reg_data;
	uint32_t mask = ltc7871_iio_get_mask_mfr_oc_fault(priv);
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mfr_oc_fault(ltc7871, mask, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", ltc7871_fault_avail[value]);
}

/**
 * @brief Handles the read request for MFR_NOC_FAULT register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_read_mfr_noc_fault(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t reg_data;
	uint32_t mask = ltc7871_iio_get_mask_mfr_noc_fault(priv);
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mfr_noc_fault(ltc7871, mask, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", ltc7871_fault_avail[value]);
}

/**
 * @brief Handles the read request for MFR_NOC_FAULT register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_read_mfr_status(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t reg_data;
	uint32_t mask = ltc7871_iio_get_mask_mfr_status(priv);
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mfr_status(ltc7871, mask, &value);
	if (ret)
		return ret;

	switch (priv) {
	case LTC7871_SS_DONE:
		return sprintf(buf, "%s", ltc7871_ss_done_avail[value]);
	case LTC7871_MAX_CURRENT:
		return sprintf(buf, "%s", ltc7871_max_current_avail[value]);
	case LTC7871_PGOOD:
		return sprintf(buf, "%s", ltc7871_pgood_avail[value]);
	}
	return 0;
}

/**
 * @brief Handles the read request for MFR_CONFIG1 register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_read_mfr_config1(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t reg_data;
	uint32_t mask = ltc7871_iio_get_mask_mfr_config1(priv);
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mfr_config1_setting(ltc7871, mask, &value);
	if (ret)
		return ret;

	switch (priv) {
	case LTC7871_SERCUR_WARNING:
		return sprintf(buf, "%s", ltc7871_sercur_warning_avail[value]);
	case LTC7871_DRVCC_SET:
		return sprintf(buf, "%s", ltc7871_drvcc_set_avail[value]);
	case LTC7871_ILIM_SET:
		return sprintf(buf, "%s", ltc7871_ilim_set_avail[value]);
	}
}

/**
 * @brief Handles the read request for MFR_CONFIG2 register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_read_mfr_config2(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t reg_data;
	uint32_t mask = ltc7871_iio_get_mask_mfr_config2(priv);
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mfr_config2_setting(ltc7871, mask, &value);
	if (ret)
		return ret;

	switch (priv) {
	case LTC7871_BURST:
		return sprintf(buf, "%s", ltc7871_burst_avail[value]);
	case LTC7871_DCM:
		return sprintf(buf, "%s", ltc7871_dcm_avail[value]);
	case LTC7871_HIZ:
		return sprintf(buf, "%s", ltc7871_hiz_avail[value]);
	case LTC7871_SPRD:
		return sprintf(buf, "%s", ltc7871_sprd_avail[value]);
	case LTC7871_BUCK_BOOST:
		return sprintf(buf, "%s", ltc7871_buck_boost_avail[value]);
	}
}

/**
 * @brief Handles the reset request.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_reset(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{

	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	return ltc7871_reset(ltc7871);
}

/**
 * @brief Handles the clear PEC request.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_clear_pec_fault(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{

	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	return ltc7871_clear_pec_fault(ltc7871);
}

/**
 * @brief Handles the read request for PEC fault attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_read_pec_fault(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t reg_data;
	uint32_t mask = ltc7871_iio_get_mask_mfr_chip_ctrl(priv);
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_read_pec_fault(ltc7871, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", ltc7871_fault_avail[value]);
}

/**
 * @brief Handles the write request for write protection.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_set_write_protect(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t mask = ltc7871_iio_get_mask_mfr_chip_ctrl(priv);
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value, NULL);
	if (ret)
		return ret;

	return ltc7871_set_write_protect(ltc7871, value);
}

/**
 * @brief Handles the write request for write protection.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_read_write_protect(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;

	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_is_write_protected(ltc7871, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", ltc7871_wp_avail[value]);
}

/**
 * @brief Handles the read request for MFR_IDAC_VLOW register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */

static int ltc7871_iio_read_mfr_idac_vlow(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int8_t value;
	int32_t val_32;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mfr_idac_vlow(ltc7871, &value);
	if (ret)
		return ret;
	val_32 = value;
	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val_32);
}

/**
 * @brief Handles the write request for MFR_IDAC_VLOW register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_write_mfr_idac_vlow(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int8_t value;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value, NULL);
	if (ret)
		return ret;

	return ltc7871_set_mfr_idac_vlow(ltc7871, value);
}

/**
 * @brief Handles the read request for MFR_IDAC_VHIGH register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_read_mfr_idac_vhigh(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int8_t value;
	int32_t val_32;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mfr_idac_vhigh(ltc7871, &value);
	if (ret)
		return ret;

	val_32 = value;
	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val_32);
}

/**
 * @brief Handles the write request for MFR_IDAC_VHIGH register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_write_mfr_idac_vhigh(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int8_t value;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value, NULL);
	if (ret)
		return ret;

	return ltc7871_set_mfr_idac_vhigh(ltc7871, value);
}

/**
 * @brief Handles the read request for MFR_IDAC_SETCUR register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_read_mfr_idac_setcur(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int8_t value;
	int32_t val_32;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mfr_idac_setcur(ltc7871, &value);
	if (ret)
		return ret;

	val_32 = value;
	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val_32);
}

/**
 * @brief Handles the write request for MFR_IDAC_SETCUR register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_write_mfr_idac_setcur(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int8_t value;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value, NULL);
	if (ret)
		return ret;

	return ltc7871_set_mfr_idac_setcur(ltc7871, value);
}

/**
 * @brief Handles the read request for freq_spread_range register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_get_freq_spread_range(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t reg_data;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_freq_spread_range(ltc7871, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", ltc7871_ssfm_fsr_avail[value]);

}

/**
 * @brief Handles the write request for freq_spread_range register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_set_freq_spread_range(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t value;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value, NULL);
	if (ret)
		return ret;

	return ltc7871_set_freq_spread_range(ltc7871, value);
}

/**
 * @brief Handles the read request for mod_freq register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_get_mod_freq(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint8_t value;
	uint8_t reg_data;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = ltc7871_get_mod_freq(ltc7871, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", ltc7871_ssfm_msf_avail[value]);

}

/**
 * @brief Handles the write request for mod_freq register attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_set_mod_freq(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int8_t value;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value, NULL);
	if (ret)
		return ret;

	return ltc7871_set_mod_freq(ltc7871, value);
}

/**
 * @brief Handles the read request for LTC7871 pins.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltc7871_iio_get_pwmen_pin(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel)
{
	int ret;
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	uint8_t value;

	ret = ltc7871_get_pwmen_pin(ltc7871, &value);
	if (ret)
		return ret;

	return sprintf(buf, "%s", ltc7871_pin_avail[value]);
}

/**
 * @brief Handles the write request for LTC7871 pins.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
static int ltc7871_iio_set_pwmen_pin(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel)
{
	int ret;
	uint8_t value;
	uint32_t num_values = NO_OS_ARRAY_SIZE(ltc7871_pin_avail);
	struct ltc7871_iio_dev *iio_ltc7871;
	struct ltc7871_dev *ltc7871;

	if (!dev)
		return -EINVAL;

	iio_ltc7871 = (struct ltc7871_iio_dev *)dev;
	ltc7871 = iio_ltc7871->ltc7871_dev;

	for (value = 0; value < num_values; value++)
		if (!strcmp(buf, ltc7871_pin_avail[value]))
			break;

	if (value == num_values)
		return -EINVAL;

	ret = ltc7871_set_pwmen_pin(ltc7871, value);

	return ret;
}

/**
 * @brief Initializes the LTC7871 IIO descriptor.
 * @param iio_dev - The iio device descriptor.
 * @param init_param - The structure that contains the device
 * 		initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int ltc7871_iio_init(struct ltc7871_iio_dev **iio_dev,
		     struct ltc7871_iio_dev_init_param *init_param)
{
	struct ltc7871_iio_dev *descriptor;
	int ret;

	if (!iio_dev || !init_param || !init_param->ltc7871_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = ltc7871_init(&descriptor->ltc7871_dev,
			   init_param->ltc7871_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &ltc7871_iio_dev;

	*iio_dev = descriptor;

	return 0;

free_desc:
	ltc7871_iio_remove(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int ltc7871_iio_remove(struct ltc7871_iio_dev *desc)
{
	if (!desc)
		return -ENODEV;

	ltc7871_remove(desc->ltc7871_dev);
	no_os_free(desc);

	return 0;
}
