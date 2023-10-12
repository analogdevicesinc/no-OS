/***************************************************************************//**
 *   @file   axi_jesd204_tx.c
 *   @brief  Driver for the Analog Devices AXI-JESD204-TX peripheral.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "no_os_clk.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "axi_jesd204_tx.h"
#include "no_os_axi_io.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define JESD204_TX_REG_VERSION			0x00
#define JESD204_TX_REG_MAGIC			0x0c

#define JESD204_TX_REG_CONF_NUM_LANES		0x10
#define JESD204_TX_REG_CONF_DATA_PATH_WIDTH	0x14

#define JESD204_TX_REG_SYNTH_REG_1		0x18
#define JESD204_TX_ENCODER_MASK			NO_OS_GENMASK(9, 8)
#define JESD204_TX_ENCODER_GET(x)	no_os_field_get(JESD204_TX_ENCODER_MASK, x)

#define JESD204_TX_REG_LINK_DISABLE		0xc0
#define JESD204_TX_REG_LINK_STATE		0xc4
#define JESD204_TX_REG_LINK_CLK_RATIO	0xc8

#define JESD204_TX_REG_SYSREF_CONF		0x100
#define JESD204_TX_REG_SYSREF_CONF_SYSREF_DISABLE	NO_OS_BIT(0)

#define JESD204_TX_REG_SYSREF_LMFC_OFFSET	0x104

#define JESD204_TX_REG_SYSREF_STATUS	0x108

#define JESD204_TX_REG_CONF0			0x210

#define JESD204_TX_REG_LINK_CONF4		0x21C

#define JESD204_TX_REG_LINK_STATUS		0x280
#define JESD204_LINK_STATUS_DATA		3

#define JESD204_TX_REG_ILAS(x, y)		\
	(((x) * 32 + (y) * 4) + 0x310)

#define JESD204_TX_MAGIC				\
	(('2' << 24) | ('0' << 16) | ('4' << 8) | ('T'))

#define JESD204_SYNTH_DATA_PATH_WIDTH_MASK	NO_OS_GENMASK(7, 0)
#define JESD204_SYNTH_DATA_PATH_WIDTH_GET(x)	no_os_field_get(JESD204_SYNTH_DATA_PATH_WIDTH_MASK, x)
#define JESD204_TPL_DATA_PATH_WIDTH_MASK	NO_OS_GENMASK(15, 8)
#define JESD204_TPL_DATA_PATH_WIDTH_GET(x)	no_os_field_get(JESD204_TPL_DATA_PATH_WIDTH_MASK, x)
#define ADI_AXI_PCORE_VER(major, minor, patch)	\
	(((major) << 16) | ((minor) << 8) | (patch))

#define PCORE_VERSION_MAJOR(x)		((x) >> 16)
#define PCORE_VERSION_MINOR(x)		(((x) >> 8) & 0xff)
#define PCORE_VERSION_PATCH(x)		((x) & 0xff)

const char *axi_jesd204_tx_link_status_label[] = {
	"WAIT",
	"CGS",
	"ILAS",
	"DATA"
};

struct axi_jesd204_tx_jesd204_priv {
	struct axi_jesd204_tx *jesd;
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief JESD204 TX AXI Data Write.
 * @param jesd - The device structure.
 * @param reg_addr - The register address.
 * @param reg_val - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_jesd204_tx_write(struct axi_jesd204_tx *jesd,
			     uint32_t reg_addr, uint32_t reg_val)
{
	no_os_axi_io_write(jesd->base, reg_addr, reg_val);

	return 0;
}

/**
 * @brief JESD204 TX AXI Data Read.
 * @param jesd - The device structure.
 * @param reg_addr - The register address.
 * @param reg_val - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_jesd204_tx_read(struct axi_jesd204_tx *jesd,
			    uint32_t reg_addr, uint32_t *reg_val)
{
	no_os_axi_io_read(jesd->base, reg_addr, reg_val);

	return 0;
}

/**
 * @brief JESD204 TX Lane Clock Enable.
 * @param jesd - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_jesd204_tx_lane_clk_enable(struct axi_jesd204_tx *jesd)
{
	axi_jesd204_tx_write(jesd, JESD204_TX_REG_SYSREF_STATUS, 0x3);
	axi_jesd204_tx_write(jesd, JESD204_TX_REG_LINK_DISABLE, 0x0);

	return 0;
}

/**
 * @brief JESD204 TX Lane Clock Disable.
 * @param jesd - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_jesd204_tx_lane_clk_disable(struct axi_jesd204_tx *jesd)
{
	return axi_jesd204_tx_write(jesd, JESD204_TX_REG_LINK_DISABLE, 0x1);
}

/**
 * @brief Read status of the JESD204 Transmit Peripherial
 * @param jesd - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
uint32_t axi_jesd204_tx_status_read(struct axi_jesd204_tx *jesd)
{
	uint32_t link_disabled;
	uint32_t link_status;
	uint32_t sysref_status;
	uint32_t clock_ratio;
	uint32_t clock_rate;
	uint32_t link_rate;
	uint32_t sysref_config;
	uint32_t link_config0;
	uint32_t lmfc_rate;
	const char *status;

	axi_jesd204_tx_read(jesd, JESD204_TX_REG_LINK_STATE, &link_disabled);
	axi_jesd204_tx_read(jesd, JESD204_TX_REG_LINK_STATUS, &link_status);
	axi_jesd204_tx_read(jesd, JESD204_TX_REG_SYSREF_STATUS, &sysref_status);
	axi_jesd204_tx_read(jesd, JESD204_TX_REG_LINK_CLK_RATIO, &clock_ratio);
	axi_jesd204_tx_read(jesd, JESD204_TX_REG_SYSREF_CONF, &sysref_config);
	axi_jesd204_tx_read(jesd, JESD204_TX_REG_CONF0, &link_config0);

	printf("%s status:\n", jesd->name);

	printf("\tLink is %s\n", (link_disabled & 0x1) ? "disabled" : "enabled");

	if (clock_ratio == 0) {
		printf("\tMeasured Link Clock: off\n");
	} else {
		clock_rate = NO_OS_DIV_ROUND_CLOSEST_ULL(100000ULL * clock_ratio,
				1ULL << 16);
		printf("\tMeasured Link Clock: %"PRIu32".%.3"PRIu32" MHz\n",\
		       clock_rate / 1000, clock_rate % 1000);
	}

	clock_rate = jesd->device_clk_khz;
	printf("\tReported Link Clock: %"PRIu32".%.3"PRIu32" MHz\n",
	       clock_rate / 1000, clock_rate % 1000);

	if (!link_disabled) {
		status = (link_status & 0x10) ?
			 "\tSYNC~: deasserted\n" : "\tSYNC~: asserted\n";

		clock_rate = jesd->lane_clk_khz;
		if (jesd->encoder == JESD204_ENCODER_64B66B) {
			link_rate = NO_OS_DIV_ROUND_CLOSEST(clock_rate, 66);
			lmfc_rate = (clock_rate * 8) /
				    (66 * ((link_config0 & 0xFF) + 1));
		} else {
			link_rate = NO_OS_DIV_ROUND_CLOSEST(clock_rate, 40);
			lmfc_rate = clock_rate /
				    (10 * ((link_config0 & 0xFF) + 1));
		}
		printf("\tLane rate: %"PRIu32".%.3"PRIu32" MHz\n"
		       "\tLane rate / %d: %"PRIu32".%.3"PRIu32" MHz\n"
		       "\t%s rate: %"PRIu32".%.3"PRIu32" MHz\n",
		       clock_rate / 1000, clock_rate % 1000,
		       (jesd->encoder == JESD204_ENCODER_8B10B) ? 40 : 66,
		       link_rate / 1000, link_rate % 1000,
		       (jesd->encoder == JESD204_ENCODER_8B10B) ? "LMFC" :
		       "LEMC",
		       lmfc_rate / 1000, lmfc_rate % 1000);

		printf("%s"
		       "\tLink status: %s\n"
		       "\tSYSREF captured: %s\n"
		       "\tSYSREF alignment error: %s\n",
		       jesd->encoder == JESD204_ENCODER_64B66B ? "" :
		       status,
		       axi_jesd204_tx_link_status_label[link_status & 0x3],
		       (sysref_config & JESD204_TX_REG_SYSREF_CONF_SYSREF_DISABLE) ?
		       "disabled" : (sysref_status & 1) ? "Yes" : "No",
		       (sysref_config & JESD204_TX_REG_SYSREF_CONF_SYSREF_DISABLE) ?
		       "disabled" : (sysref_status & 2) ? "Yes" : "No");
	} else {
		printf("\tExternal reset is %s\n",
		       (link_disabled & 0x2) ? "asserted" : "deasserted");
	}

	return 0;
}

/**
 * @brief Compute the JESD204 TX ILAS checksum.
 * @param config - The JESD TX configuration
 * @return Returns 0 in case of success or negative error code otherwise.
 */
static unsigned int axi_jesd204_tx_calc_ilas_chksum(
	const struct jesd204_link *config,
	unsigned int lane_id)
{
	unsigned int chksum;

	chksum = config->device_id;
	chksum += config->bank_id;
	chksum += lane_id;
	chksum += config->num_lanes - 1;
	chksum += config->scrambling;
	chksum += config->octets_per_frame - 1;
	chksum += config->frames_per_multiframe - 1;
	chksum += config->num_converters - 1;
	chksum += config->ctrl_bits_per_sample;
	chksum += config->converter_resolution - 1;
	chksum += config->bits_per_sample - 1;
	chksum += config->subclass;
	chksum += config->samples_per_conv_frame ? config->samples_per_conv_frame - 1 :
		  0;
	chksum += config->jesd_version;
	chksum += config->high_density;

	return chksum & NO_OS_GENMASK(7, 0);
}

/**
 * @brief Compute the JESD204 TX ILAS checksum.
 * @param config - The JESD TX configuration
 * @return Returns 0 in case of success or negative error code otherwise.
 */
uint32_t axi_jesd204_tx_calc_ilas_chksum_legacy(const struct jesd204_tx_config
		*config)
{
	uint32_t chksum;

	chksum = config->device_id;
	chksum += config->bank_id;
	chksum += config->lane_id;
	chksum += config->lanes_per_device - 1;
	chksum += config->enable_scrambling;
	chksum += config->octets_per_frame - 1;
	chksum += config->frames_per_multiframe - 1;
	chksum += config->converters_per_device - 1;
	chksum += config->control_bits_per_sample;
	chksum += config->resolution - 1;
	chksum += config->bits_per_sample - 1;
	chksum += config->subclass_version;
	chksum += config->samples_per_frame  - 1;
	chksum += config->jesd_version;
	chksum += config->high_density;

	return chksum & NO_OS_GENMASK(7, 0);
}

/**
 * @brief Set the JESD204 TX lane ILAS.
 * @param jesd - The device structure.
 * @param config - The JESD TX configuration.
 * @param lane - The lane ID.
 */
static void axi_jesd204_tx_set_lane_ilas(struct axi_jesd204_tx *jesd,
		struct jesd204_link *config, unsigned int lane_id, unsigned int lane)
{
	unsigned int i;
	unsigned int val;

	for (i = 0; i < 4; i++) {
		switch (i) {
		case 0:
			val = config->device_id << 8;
			val |= config->bank_id << 24;
			break;
		case 1:
			val = lane_id;
			val |= (config->num_lanes - 1) << 8;
			val |= config->scrambling << 15;
			val |= (config->octets_per_frame - 1) << 16;
			val |= (config->frames_per_multiframe - 1) << 24;
			break;
		case 2:
			val = config->num_converters - 1;
			val |= (config->converter_resolution - 1) << 8;
			val |= config->ctrl_bits_per_sample << 14;
			val |= (config->bits_per_sample - 1) << 16;
			val |= config->subclass << 21;
			val |= (config->samples_per_conv_frame ? config->samples_per_conv_frame  - 1 :
				0) << 24;
			val |= config->jesd_version << 29;
			break;
		case 3:
			val = config->high_density << 7;
			val |= axi_jesd204_tx_calc_ilas_chksum(config, lane_id) << 24;
			break;
		}

		axi_jesd204_tx_write(jesd, JESD204_TX_REG_ILAS(lane, i), val);
	}
}

/**
 * @brief Set the JESD204 TX lane ILAS.
 * @param jesd - The device structure.
 * @param config - The JESD TX configuration.
 * @param lane - The lane ID.
 */
void axi_jesd204_tx_set_lane_ilas_legacy(struct axi_jesd204_tx *jesd,
		struct jesd204_tx_config *config, uint32_t lane)
{
	uint32_t i;
	uint32_t val;

	config->lane_id = lane;

	for (i = 0; i < 4; i++) {
		switch (i) {
		case 0:
			val = config->device_id << 8;
			val |= config->bank_id << 24;
			break;
		case 1:
			val = config->lane_id;
			val |= (config->lanes_per_device - 1) << 8;
			val |= config->enable_scrambling << 15;
			val |= (config->octets_per_frame - 1) << 16;
			val |= (config->frames_per_multiframe - 1) << 24;
			break;
		case 2:
			val = (config->converters_per_device - 1);
			val |= (config->resolution - 1) << 8;
			val |= config->control_bits_per_sample << 14;
			val |= (config->bits_per_sample - 1) << 16;
			val |= config->subclass_version << 21;
			val |= (config->samples_per_frame - 1) << 24;
			val |= config->jesd_version << 29;
			break;
		case 3:
			val = config->high_density << 7;
			val |= axi_jesd204_tx_calc_ilas_chksum_legacy(config) << 24;
			break;
		}

		axi_jesd204_tx_write(jesd, JESD204_TX_REG_ILAS(lane, i), val);
	}
}

/**
 * @brief Apply the JESD204 TX configuration.
 * @param jesd - The device structure.
 * @param config - The JESD TX configuration.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_jesd204_tx_apply_config(struct axi_jesd204_tx *jesd,
				    struct jesd204_link *config)
{
	unsigned int octets_per_multiframe;
	unsigned int multiframe_align;
	unsigned int val;
	unsigned int lane, i;

	octets_per_multiframe = config->frames_per_multiframe *
				config->octets_per_frame;

	multiframe_align = jesd->data_path_width;

	if (jesd->encoder == JESD204_ENCODER_64B66B &&
	    (octets_per_multiframe % 256) != 0) {
		pr_err("octets_per_frame * frames_per_multiframe must be a multiple of 256\n");
		return -EINVAL;
	}

	if (octets_per_multiframe % multiframe_align != 0) {
		pr_err("octets_per_frame * frames_per_multiframe must be a "
		       "multiple of %d\n", multiframe_align);
		return -EINVAL;
	}

	val = (octets_per_multiframe - 1);
	val |= (config->octets_per_frame - 1) << 16;

	if (config->subclass == JESD204_SUBCLASS_0)
		axi_jesd204_tx_write(jesd, JESD204_TX_REG_SYSREF_CONF,
				     JESD204_TX_REG_SYSREF_CONF_SYSREF_DISABLE);

	axi_jesd204_tx_write(jesd, JESD204_TX_REG_CONF0, val);

	if (jesd->config.version >= ADI_AXI_PCORE_VER(1, 6, 'a')) {
		val = octets_per_multiframe / jesd->tpl_data_path_width - 1;
		axi_jesd204_tx_write(jesd, JESD204_TX_REG_LINK_CONF4, val);
	}

	if (jesd->encoder == JESD204_ENCODER_8B10B) {
		for (i = 0, lane = 0; lane < jesd->num_lanes; lane++) {
			unsigned int lane_id;

			if (i >= config->num_lanes)
				i = 0;

			lane_id = config->lane_ids[i++];
			axi_jesd204_tx_set_lane_ilas(jesd, config, lane_id, lane);
		}
	}

	if (config->sysref.lmfc_offset != JESD204_LMFC_OFFSET_UNINITIALIZED)
		axi_jesd204_tx_write(jesd, JESD204_TX_REG_SYSREF_LMFC_OFFSET,
				     config->sysref.lmfc_offset);

	return 0;
}

/**
 * @brief Apply the JESD204 TX configuration.
 * @param jesd - The device structure.
 * @param config - The JESD TX configuration.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_jesd204_tx_apply_config_legacy(struct axi_jesd204_tx *jesd,
		struct jesd204_tx_config *config)
{
	uint32_t octets_per_multiframe;
	uint32_t multiframe_align;
	uint32_t val;
	uint32_t lane;

	octets_per_multiframe = config->frames_per_multiframe *
				config->octets_per_frame;

	multiframe_align = jesd->data_path_width;

	if (jesd->encoder == JESD204_ENCODER_64B66B &&
	    (octets_per_multiframe % 256) != 0) {
		printf("%s: octets_per_frame * frames_per_multiframe must be a multiple of 256",
		       jesd->name);
		return -1;
	}

	if (octets_per_multiframe % multiframe_align != 0) {
		printf("%s: octets_per_frame * frames_per_multiframe must be a "
		       "multiple of %"PRIu32"\n", jesd->name, multiframe_align);
		return -1;
	}

	val = (octets_per_multiframe - 1);
	val |= (config->octets_per_frame - 1) << 16;

	if (config->subclass_version == 0)
		axi_jesd204_tx_write(jesd, JESD204_TX_REG_SYSREF_CONF,
				     JESD204_TX_REG_SYSREF_CONF_SYSREF_DISABLE);

	axi_jesd204_tx_write(jesd, JESD204_TX_REG_CONF0, val);

	if (jesd->config.version >= ADI_AXI_PCORE_VER(1, 6, 'a')) {
		/* beats per multiframe */
		val = (octets_per_multiframe / jesd->tpl_data_path_width) - 1;
		axi_jesd204_tx_write(jesd, JESD204_TX_REG_LINK_CONF4, val);
	}

	for (lane = 0; lane < jesd->num_lanes; lane++)
		if (jesd->encoder == JESD204_ENCODER_8B10B)
			axi_jesd204_tx_set_lane_ilas_legacy(jesd, config, lane);

	return 0;
}

static unsigned long axi_jesd204_tx_calc_device_clk(struct axi_jesd204_tx *jesd,
		unsigned long link_rate)
{
	return no_os_div_u64((uint64_t) link_rate * jesd->data_path_width,
			     jesd->tpl_data_path_width);
}

static int axi_jesd204_tx_jesd204_link_pre_setup(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct axi_jesd204_tx_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct axi_jesd204_tx *jesd = priv->jesd;
	unsigned long link_rate, device_rate, lane_rate;
	int ret;

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	case JESD204_STATE_OP_REASON_UNINIT:
		return JESD204_STATE_CHANGE_DONE;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__, lnk->link_id,
		 jesd204_state_op_reason_str(reason));

	ret = jesd204_link_get_device_clock(lnk, &link_rate);
	pr_debug("%s: Link%u device clock rate %lu (%d)\n",
		 __func__, lnk->link_id, link_rate, ret);
	if (ret) {
		pr_err("%s: Link%u get device clock rate failed (%d)\n",
		       __func__, lnk->link_id, ret);
		return ret;
	}

	ret = jesd204_link_get_rate_khz(lnk, &lane_rate);
	pr_debug("%s: Link%u lane rate %lu (%d)\n",
		 __func__, lnk->link_id, lane_rate, ret);
	if (ret) {
		pr_err("%s: Link%u get rate failed (%d)\n",
		       __func__, lnk->link_id, ret);
		return ret;
	}

	device_rate = axi_jesd204_tx_calc_device_clk(jesd, link_rate);
	pr_debug("%s: Link%u set device clock rate %lu Hz\n",
		 __func__, lnk->link_id, device_rate);
	pr_debug("%s: Link%u set link clock rate %lu Hz\n",
		 __func__, lnk->link_id, link_rate);

	pr_debug("%s: Link%u set lane rate %lu kHz\n",
		 __func__, lnk->link_id, lane_rate);

	ret = no_os_clk_set_rate(jesd->lane_clk, lane_rate);
	if (ret) {
		pr_err("%s: Link%u set lane rate %lu kHz failed (%d)\n",
		       __func__, lnk->link_id, lane_rate, ret);
		return ret;
	} else {
		pr_debug("%s: Link%u set lane rate %lu kHz\n",
			 __func__, lnk->link_id, lane_rate);
	}

	return JESD204_STATE_CHANGE_DONE;
}


static int axi_jesd204_tx_jesd204_link_setup(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct axi_jesd204_tx_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct axi_jesd204_tx *jesd = priv->jesd;
	int ret;

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	case JESD204_STATE_OP_REASON_UNINIT:
		return JESD204_STATE_CHANGE_DONE;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (jesd->num_lanes != lnk->num_lanes)
		pr_debug("Possible instantiation for multiple chips; HDL lanes %u, Link[%u] lanes %u\n",
			 jesd->num_lanes, lnk->link_id, lnk->num_lanes);

	ret = axi_jesd204_tx_apply_config(jesd, lnk);
	if (ret) {
		pr_err("%s: Apply config Link%u failed (%d)\n",
		       __func__, lnk->link_id, ret);
		return ret;
	}

	ret = no_os_clk_enable(jesd->lane_clk);
	if (ret) {
		pr_err("%s: Link%u enable lane clock failed (%d)\n",
		       __func__, lnk->link_id, ret);
		return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int axi_jesd204_tx_jesd204_clks_enable(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct axi_jesd204_tx_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct axi_jesd204_tx *jesd = priv->jesd;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__, lnk->link_id,
		 jesd204_state_op_reason_str(reason));

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	axi_jesd204_tx_write(jesd, JESD204_TX_REG_LINK_DISABLE, 0x1);
	no_os_udelay(1);
	axi_jesd204_tx_write(jesd, JESD204_TX_REG_SYSREF_STATUS, 0x3);
	axi_jesd204_tx_write(jesd, JESD204_TX_REG_LINK_DISABLE, 0x0);

	return JESD204_STATE_CHANGE_DONE;
}

static int axi_jesd204_tx_jesd204_link_enable(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct axi_jesd204_tx_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct axi_jesd204_tx *jesd = priv->jesd;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__, lnk->link_id,
		 jesd204_state_op_reason_str(reason));

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		return JESD204_STATE_CHANGE_DONE;
	case JESD204_STATE_OP_REASON_UNINIT:
		axi_jesd204_tx_write(jesd, JESD204_TX_REG_LINK_DISABLE, 0x1);
		return JESD204_STATE_CHANGE_DONE;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}
}

static int axi_jesd204_tx_jesd204_link_running(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason,
		struct jesd204_link *lnk)
{
	struct axi_jesd204_tx_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct axi_jesd204_tx *jesd = priv->jesd;
	unsigned int link_status;
	int retry = 20;

	pr_debug("%s:%d link_num %u reason %s\n", __func__, __LINE__,
		 lnk->link_id, jesd204_state_op_reason_str(reason));

	if (reason == JESD204_STATE_OP_REASON_INIT) {
		do {
			no_os_mdelay(4);
			axi_jesd204_tx_read(jesd, JESD204_TX_REG_LINK_STATUS, &link_status);
			link_status &= 0x3;
		} while (link_status != JESD204_LINK_STATUS_DATA && retry--);

		if (link_status != JESD204_LINK_STATUS_DATA) {
			pr_err("%s: Link%u status failed (%s)\n",
			       __func__, lnk->link_id,
			       axi_jesd204_tx_link_status_label[link_status]);

			return JESD204_STATE_CHANGE_ERROR;
		}
	}

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_axi_jesd204_tx_init = {
	.state_ops = {
		[JESD204_OP_LINK_PRE_SETUP] = {
			.per_link = axi_jesd204_tx_jesd204_link_pre_setup,
		},
		[JESD204_OP_LINK_SETUP] = {
			.per_link = axi_jesd204_tx_jesd204_link_setup,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = axi_jesd204_tx_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = axi_jesd204_tx_jesd204_link_enable,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = axi_jesd204_tx_jesd204_link_running,
		},
	},
	.sizeof_priv = sizeof(struct axi_jesd204_tx_jesd204_priv),
};

/**
 * @brief Device initialization.
 * @param jesd204 - The device structure.
 * @param init - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_jesd204_tx_init_legacy(struct axi_jesd204_tx **jesd204,
				   const struct jesd204_tx_init *init)
{
	struct axi_jesd204_tx *jesd;
	uint32_t synth_1;
	uint32_t magic;
	uint32_t version;
	uint32_t status;
	uint32_t tmp;

	jesd = (struct axi_jesd204_tx *)no_os_malloc(sizeof(*jesd));
	if (!jesd)
		return -1;

	jesd->base = init->base;
	jesd->name = init->name;
	jesd->device_clk_khz = init->device_clk_khz;
	jesd->lane_clk_khz = init->lane_clk_khz;

	axi_jesd204_tx_read(jesd, JESD204_TX_REG_MAGIC, &magic);
	if (magic != JESD204_TX_MAGIC) {
		printf("%s: Unexpected peripheral identifier %.08"PRIX32"\n",
		       jesd->name, magic);
		goto err;
	}

	axi_jesd204_tx_read(jesd, JESD204_TX_REG_VERSION, &version);
	if (PCORE_VERSION_MAJOR(version) != 1) {
		printf("%s: Unsupported peripheral version %"
		       ""PRIu32".%"PRIu32".%"PRIu32"\n",
		       jesd->name,
		       PCORE_VERSION_MAJOR(version),
		       PCORE_VERSION_MINOR(version),
		       PCORE_VERSION_PATCH(version));
		goto err;
	}

	axi_jesd204_tx_read(jesd, JESD204_TX_REG_CONF_NUM_LANES,
			    &jesd->num_lanes);
	axi_jesd204_tx_read(jesd, JESD204_TX_REG_CONF_DATA_PATH_WIDTH, &tmp);
	jesd->data_path_width = 1 << JESD204_SYNTH_DATA_PATH_WIDTH_GET(tmp);
	jesd->tpl_data_path_width = JESD204_TPL_DATA_PATH_WIDTH_GET(tmp);

	axi_jesd204_tx_read(jesd, JESD204_TX_REG_SYNTH_REG_1,
			    &synth_1);
	jesd->encoder = JESD204_TX_ENCODER_GET(synth_1);
	if (jesd->encoder == JESD204_ENCODER_UNKNOWN)
		jesd->encoder = JESD204_ENCODER_8B10B;
	else if (jesd->encoder >= JESD204_ENCODER_MAX)
		goto err;

	jesd->config.version = version;
	jesd->config.device_id = 0;
	jesd->config.bank_id = 0;
	jesd->config.enable_scrambling = true;
	jesd->config.lanes_per_device = jesd->num_lanes;
	jesd->config.jesd_version = 1;
	jesd->config.samples_per_frame = 1;

	jesd->config.octets_per_frame = init->octets_per_frame;
	jesd->config.frames_per_multiframe = init->frames_per_multiframe;
	jesd->config.converters_per_device = init->converters_per_device;
	jesd->config.resolution = init->converter_resolution;
	jesd->config.bits_per_sample = init->bits_per_sample;
	jesd->config.high_density = init->high_density;
	jesd->config.control_bits_per_sample = init->control_bits_per_sample;
	jesd->config.subclass_version = init->subclass;

	axi_jesd204_tx_lane_clk_disable(jesd);

	status = axi_jesd204_tx_apply_config_legacy(jesd, &jesd->config);
	if (status != 0)
		goto err;

	*jesd204 = jesd;

	return 0;

err:
	no_os_free(jesd);

	return -1;
}

/**
 * @brief Device initialization.
 * @param jesd204 - The device structure.
 * @param init - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_jesd204_tx_init(struct axi_jesd204_tx **jesd204,
			    const struct jesd204_tx_init *init)
{
	struct axi_jesd204_tx_jesd204_priv *priv;
	struct axi_jesd204_tx *jesd;
	uint32_t synth_1;
	uint32_t magic;
	uint32_t version;
	uint32_t tmp;
	int ret = -ENODEV;

	jesd = (struct axi_jesd204_tx *)no_os_calloc(1, sizeof(*jesd));
	if (!jesd)
		return -ENOMEM;

	jesd->base = init->base;
	jesd->name = init->name;
	jesd->device_clk_khz = init->device_clk_khz;
	jesd->lane_clk_khz = init->lane_clk_khz;

	axi_jesd204_tx_read(jesd, JESD204_TX_REG_MAGIC, &magic);
	if (magic != JESD204_TX_MAGIC) {
		printf("%s: Unexpected peripheral identifier %.08"PRIX32"\n",
		       jesd->name, magic);
		goto err;
	}

	axi_jesd204_tx_read(jesd, JESD204_TX_REG_VERSION, &version);
	if (PCORE_VERSION_MAJOR(version) != 1) {
		printf("%s: Unsupported peripheral version %"
		       ""PRIu32".%"PRIu32".%"PRIu32"\n",
		       jesd->name,
		       PCORE_VERSION_MAJOR(version),
		       PCORE_VERSION_MINOR(version),
		       PCORE_VERSION_PATCH(version));
		goto err;
	}

	axi_jesd204_tx_read(jesd, JESD204_TX_REG_CONF_NUM_LANES,
			    &jesd->num_lanes);
	axi_jesd204_tx_read(jesd, JESD204_TX_REG_CONF_DATA_PATH_WIDTH, &tmp);
	jesd->data_path_width = 1 << JESD204_SYNTH_DATA_PATH_WIDTH_GET(tmp);
	jesd->tpl_data_path_width = JESD204_TPL_DATA_PATH_WIDTH_GET(tmp);

	axi_jesd204_tx_read(jesd, JESD204_TX_REG_SYNTH_REG_1,
			    &synth_1);
	jesd->encoder = JESD204_TX_ENCODER_GET(synth_1);
	if (jesd->encoder == JESD204_ENCODER_UNKNOWN)
		jesd->encoder = JESD204_ENCODER_8B10B;
	else if (jesd->encoder >= JESD204_ENCODER_MAX)
		goto err;

	jesd->config.version = version;
	jesd->config.device_id = 0;
	jesd->config.bank_id = 0;
	jesd->config.enable_scrambling = true;
	jesd->config.lanes_per_device = jesd->num_lanes;
	jesd->config.jesd_version = 1;
	jesd->config.samples_per_frame = 1;

	jesd->config.octets_per_frame = init->octets_per_frame;
	jesd->config.frames_per_multiframe = init->frames_per_multiframe;
	jesd->config.converters_per_device = init->converters_per_device;
	jesd->config.resolution = init->converter_resolution;
	jesd->config.bits_per_sample = init->bits_per_sample;
	jesd->config.high_density = init->high_density;
	jesd->config.control_bits_per_sample = init->control_bits_per_sample;
	jesd->config.subclass_version = init->subclass;

	jesd->lane_clk = init->lane_clk;

	ret = jesd204_dev_register(&jesd->jdev, &jesd204_axi_jesd204_tx_init);
	if (ret)
		goto err;

	priv = jesd204_dev_priv(jesd->jdev);
	priv->jesd = jesd;

	*jesd204 = jesd;

	return 0;

err:
	no_os_free(jesd);

	return ret;
}

/**
 * @brief Free resoulces allocated for JESD204 TX peripheral.
 * @param jesd - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t axi_jesd204_tx_remove(struct axi_jesd204_tx *jesd)
{
	no_os_free(jesd);

	return 0;
}
