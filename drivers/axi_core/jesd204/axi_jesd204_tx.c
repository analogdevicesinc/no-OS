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
#include "error.h"
#include "util.h"
#include "axi_jesd204_tx.h"
#include "axi_io.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define JESD204_TX_REG_VERSION			0x00
#define JESD204_TX_REG_MAGIC			0x0c

#define JESD204_TX_REG_CONF_NUM_LANES		0x10
#define JESD204_TX_REG_CONF_DATA_PATH_WIDTH	0x14

#define JESD204_TX_REG_SYNTH_REG_1		0x18
#define JESD204_TX_ENCODER_MASK			GENMASK(9, 8)
#define JESD204_TX_ENCODER_GET(x)	field_get(JESD204_TX_ENCODER_MASK, x)

#define JESD204_TX_REG_LINK_DISABLE		0xc0
#define JESD204_TX_REG_LINK_STATE		0xc4
#define JESD204_TX_REG_LINK_CLK_RATIO	0xc8

#define JESD204_TX_REG_SYSREF_CONF		0x100
#define JESD204_TX_REG_SYSREF_CONF_SYSREF_DISABLE	BIT(0)

#define JESD204_TX_REG_SYSREF_STATUS	0x108

#define JESD204_TX_REG_CONF0			0x210

#define JESD204_TX_REG_LINK_CONF4		0x21C

#define JESD204_TX_REG_LINK_STATUS		0x280

#define JESD204_TX_REG_ILAS(x, y)		\
	(((x) * 32 + (y) * 4) + 0x310)

#define JESD204_TX_MAGIC				\
	(('2' << 24) | ('0' << 16) | ('4' << 8) | ('T'))

#define JESD204_SYNTH_DATA_PATH_WIDTH_MASK	GENMASK(7, 0)
#define JESD204_SYNTH_DATA_PATH_WIDTH_GET(x)	field_get(JESD204_SYNTH_DATA_PATH_WIDTH_MASK, x)
#define JESD204_TPL_DATA_PATH_WIDTH_MASK	GENMASK(15, 8)
#define JESD204_TPL_DATA_PATH_WIDTH_GET(x)	field_get(JESD204_TPL_DATA_PATH_WIDTH_MASK, x)
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

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief axi_jesd204_tx_write
 */
int32_t axi_jesd204_tx_write(struct axi_jesd204_tx *jesd,
			     uint32_t reg_addr, uint32_t reg_val)
{
	axi_io_write(jesd->base, reg_addr, reg_val);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_tx_read
 */
int32_t axi_jesd204_tx_read(struct axi_jesd204_tx *jesd,
			    uint32_t reg_addr, uint32_t *reg_val)
{
	axi_io_read(jesd->base, reg_addr, reg_val);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_tx_lane_clk_enable
 */
int32_t axi_jesd204_tx_lane_clk_enable(struct axi_jesd204_tx *jesd)
{
	axi_jesd204_tx_write(jesd, JESD204_TX_REG_SYSREF_STATUS, 0x3);
	axi_jesd204_tx_write(jesd, JESD204_TX_REG_LINK_DISABLE, 0x0);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_tx_lane_clk_disable
 */
int32_t axi_jesd204_tx_lane_clk_disable(struct axi_jesd204_tx *jesd)
{
	return axi_jesd204_tx_write(jesd, JESD204_TX_REG_LINK_DISABLE, 0x1);
}

/**
 * @brief axi_jesd204_tx_status_read
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
		clock_rate = DIV_ROUND_CLOSEST_ULL(100000ULL * clock_ratio,
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
		if (jesd->encoder == JESD204_TX_ENCODER_64B66B) {
			link_rate = DIV_ROUND_CLOSEST(clock_rate, 66);
			lmfc_rate = (clock_rate * 8) /
				    (66 * ((link_config0 & 0xFF) + 1));
		} else {
			link_rate = DIV_ROUND_CLOSEST(clock_rate, 40);
			lmfc_rate = clock_rate /
				    (10 * ((link_config0 & 0xFF) + 1));
		}
		printf("\tLane rate: %"PRIu32".%.3"PRIu32" MHz\n"
		       "\tLane rate / %d: %"PRIu32".%.3"PRIu32" MHz\n"
		       "\t%s rate: %"PRIu32".%.3"PRIu32" MHz\n",
		       clock_rate / 1000, clock_rate % 1000,
		       (jesd->encoder == JESD204_TX_ENCODER_8B10B) ? 40 : 66,
		       link_rate / 1000, link_rate % 1000,
		       (jesd->encoder == JESD204_TX_ENCODER_8B10B) ? "LMFC" :
		       "LEMC",
		       lmfc_rate / 1000, lmfc_rate % 1000);

		printf("%s"
		       "\tLink status: %s\n"
		       "\tSYSREF captured: %s\n"
		       "\tSYSREF alignment error: %s\n",
		       jesd->encoder == JESD204_TX_ENCODER_64B66B ? "" :
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

	return SUCCESS;
}

/**
 * @brief axi_jesd204_tx_calc_ilas_chksum
 */
uint32_t axi_jesd204_tx_calc_ilas_chksum(const struct jesd204_tx_config *config)
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

	return chksum & 0xff;
}

/**
 * @brief axi_jesd204_tx_set_lane_ilas
 */
void axi_jesd204_tx_set_lane_ilas(struct axi_jesd204_tx *jesd,
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
			val |= axi_jesd204_tx_calc_ilas_chksum(config) << 24;
			break;
		}

		axi_jesd204_tx_write(jesd, JESD204_TX_REG_ILAS(lane, i), val);
	}
}

/**
 * @brief axi_jesd204_tx_apply_config
 */
int32_t axi_jesd204_tx_apply_config(struct axi_jesd204_tx *jesd,
				    struct jesd204_tx_config *config)
{
	uint32_t octets_per_multiframe;
	uint32_t multiframe_align;
	uint32_t val;
	uint32_t lane;

	octets_per_multiframe = config->frames_per_multiframe *
				config->octets_per_frame;

	multiframe_align = jesd->data_path_width;

	if (jesd->encoder == JESD204_TX_ENCODER_64B66B &&
	    (octets_per_multiframe % 256) != 0) {
		printf("%s: octets_per_frame * frames_per_multiframe must be a multiple of 256",
		       jesd->name);
		return FAILURE;
	}

	if (octets_per_multiframe % multiframe_align != 0) {
		printf("%s: octets_per_frame * frames_per_multiframe must be a "
		       "multiple of %"PRIu32"\n", jesd->name, multiframe_align);
		return FAILURE;
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
		if (jesd->encoder == JESD204_TX_ENCODER_8B10B)
			axi_jesd204_tx_set_lane_ilas(jesd, config, lane);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_tx_init
 */
int32_t axi_jesd204_tx_init(struct axi_jesd204_tx **jesd204,
			    const struct jesd204_tx_init *init)
{
	struct axi_jesd204_tx *jesd;
	uint32_t synth_1;
	uint32_t magic;
	uint32_t version;
	uint32_t status;
	uint32_t tmp;

	jesd = (struct axi_jesd204_tx *)malloc(sizeof(*jesd));
	if (!jesd)
		return FAILURE;

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
	if (jesd->encoder == JESD204_TX_ENCODER_UNKNOWN)
		jesd->encoder = JESD204_TX_ENCODER_8B10B;
	else if (jesd->encoder >= JESD204_TX_ENCODER_MAX)
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

	status = axi_jesd204_tx_apply_config(jesd, &jesd->config);
	if (status != SUCCESS)
		goto err;

	*jesd204 = jesd;

	return SUCCESS;

err:
	free(jesd);

	return FAILURE;
}

/**
 * @brief axi_jesd204_tx_remove
 */
int32_t axi_jesd204_tx_remove(struct axi_jesd204_tx *jesd)
{
	free(jesd);

	return SUCCESS;
}
