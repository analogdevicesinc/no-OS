/***************************************************************************//**
 *   @file   axi_jesd204_rx.c
 *   @brief  Driver for the Analog Devices AXI-JESD204-RX peripheral.
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
#include "delay.h"
#include "util.h"
#include "axi_jesd204_rx.h"
#include "axi_io.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define JESD204_RX_REG_VERSION			0x00
#define JESD204_RX_REG_MAGIC			0x0c

#define JESD204_RX_REG_SYNTH_NUM_LANES			0x10
#define JESD204_RX_REG_SYNTH_DATA_PATH_WIDTH	0x14

#define JESD204_RX_REG_SYNTH_REG_1		0x18
#define JESD204_RX_ENCODER_MASK			GENMASK(9, 8)
#define JESD204_RX_ENCODER_GET(x)	field_get(JESD204_RX_ENCODER_MASK, x)

#define JESD204_RX_REG_LINK_DISABLE		0xc0
#define JESD204_RX_REG_LINK_STATE		0xc4
#define JESD204_RX_REG_LINK_CLK_RATIO	0xc8

#define JESD204_RX_REG_SYSREF_CONF		0x100
#define JESD204_RX_REG_SYSREF_CONF_SYSREF_DISABLE	BIT(0)

#define JESD204_RX_REG_SYSREF_STATUS	0x108

#define JESD204_RX_REG_LINK_CONF0		0x210

#define JESD204_RX_REG_LINK_CONF4		0x21C

#define JESD204_RX_REG_LINK_CONF2		0x240
#define JESD204_RX_LINK_CONF2_BUFFER_EARLY_RELEASE	BIT(16)

#define JESD204_RX_REG_LINK_STATUS		0x280

#define JESD204_RX_REG_LANE_STATUS(x)	(((x) * 32) + 0x300)
#define JESD204_EMB_STATE_MASK		GENMASK(10, 8)
#define JESD204_EMB_STATE_GET(x) \
			field_get(JESD204_EMB_STATE_MASK, x)
#define JESD204_RX_REG_LANE_LATENCY(x)	(((x) * 32) + 0x304)
#define JESD204_RX_REG_LANE_ERRORS(x)	(((x) * 32) + 0x308)
#define JESD204_RX_REG_ILAS(x, y)		(((x) * 32 + (y) * 4) + 0x310)

#define JESD204_TX_REG_ILAS(x, y)		\
	(((x) * 32 + (y) * 4) + 0x310)

#define JESD204_RX_MAGIC				\
	(('2' << 24) | ('0' << 16) | ('4' << 8) | ('R'))

#define JESD204_SYNTH_DATA_PATH_WIDTH_MASK	GENMASK(7, 0)
#define JESD204_SYNTH_DATA_PATH_WIDTH_GET(x)	field_get(JESD204_SYNTH_DATA_PATH_WIDTH_MASK, x)
#define JESD204_TPL_DATA_PATH_WIDTH_MASK	GENMASK(15, 8)
#define JESD204_TPL_DATA_PATH_WIDTH_GET(x)	field_get(JESD204_TPL_DATA_PATH_WIDTH_MASK, x)

#define ADI_AXI_PCORE_VER(major, minor, patch)	\
	(((major) << 16) | ((minor) << 8) | (patch))

#define PCORE_VERSION_MAJOR(x)		((x) >> 16)
#define PCORE_VERSION_MINOR(x)		(((x) >> 8) & 0xff)
#define PCORE_VERSION_PATCH(x)		((x) & 0xff)

enum {
	JESD204_EMB_STATE_INIT = 1,
	JESD204_EMB_STATE_HUNT,
	JESD204_EMB_STATE_LOCK = 4
};

const char *axi_jesd204_rx_link_status_label[] = {
	"RESET",
	"WAIT FOR PHY",
	"CGS",
	"DATA",
};

const char *axi_jesd204_rx_lane_status_label[] = {
	"INIT",
	"CHECK",
	"DATA",
	"UNKNOWN",
};

const char *axi_jesd204_rx_link_status_64b66b_l[] = {
	"RESET",
	"WAIT_BS",
	"BLOCK_SYNC",
	"DATA",
};

const char *axi_jesd204_rx_emb_state_label[] = {
	"INVALID",
	"EMB_INIT",
	"EMB_HUNT",
	"INVALID",
	"EMB_LOCK",
	"INVALID",
	"INVALID",
	"INVALID",
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief axi_jesd204_rx_write
 */
int32_t axi_jesd204_rx_write(struct axi_jesd204_rx *jesd,
			     uint32_t reg_addr, uint32_t reg_val)
{
	axi_io_write(jesd->base, reg_addr, reg_val);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_rx_read
 */
int32_t axi_jesd204_rx_read(struct axi_jesd204_rx *jesd,
			    uint32_t reg_addr, uint32_t *reg_val)
{
	axi_io_read(jesd->base, reg_addr, reg_val);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_rx_lane_clk_enable
 */
int32_t axi_jesd204_rx_lane_clk_enable(struct axi_jesd204_rx *jesd)
{
	axi_jesd204_rx_write(jesd, JESD204_RX_REG_SYSREF_STATUS, 0x3);
	axi_jesd204_rx_write(jesd, JESD204_RX_REG_LINK_DISABLE, 0x0);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_rx_lane_clk_disable
 */
int32_t axi_jesd204_rx_lane_clk_disable(struct axi_jesd204_rx *jesd)
{
	return axi_jesd204_rx_write(jesd, JESD204_RX_REG_LINK_DISABLE, 0x1);
}

/**
 * @brief axi_jesd204_rx_status_read
 */
uint32_t axi_jesd204_rx_status_read(struct axi_jesd204_rx *jesd)
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
	const char *l_status;

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LINK_STATE, &link_disabled);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LINK_STATUS, &link_status);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_SYSREF_STATUS, &sysref_status);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LINK_CLK_RATIO, &clock_ratio);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_SYSREF_CONF, &sysref_config);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LINK_CONF0, &link_config0);

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
		l_status = (jesd->encoder == JESD204_RX_ENCODER_8B10B) ?
			   axi_jesd204_rx_link_status_label[link_status & 0x3] :
			   axi_jesd204_rx_link_status_64b66b_l[link_status & 0x3];

		clock_rate = jesd->lane_clk_khz;
		if (jesd->encoder == JESD204_RX_ENCODER_64B66B) {
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
		       (jesd->encoder == JESD204_RX_ENCODER_8B10B) ? 40 : 66,
		       link_rate / 1000, link_rate % 1000,
		       (jesd->encoder == JESD204_RX_ENCODER_8B10B) ? "LMFC" :
		       "LEMC",
		       lmfc_rate / 1000, lmfc_rate % 1000);

		printf("\tLink status: %s\n"
		       "\tSYSREF captured: %s\n"
		       "\tSYSREF alignment error: %s\n",
		       l_status,
		       (sysref_config & JESD204_RX_REG_SYSREF_CONF_SYSREF_DISABLE) ?
		       "disabled" : (sysref_status & 1) ? "Yes" : "No",
		       (sysref_config & JESD204_RX_REG_SYSREF_CONF_SYSREF_DISABLE) ?
		       "disabled" : (sysref_status & 2) ? "Yes" : "No");
	} else {
		printf("\tExternal reset is %s\n",
		       (link_disabled & 0x2) ? "asserted" : "deasserted");
	}

	return SUCCESS;
}

/**
 * @brief axi_jesd204_rx_get_lane_errors
 */
int32_t axi_jesd204_rx_get_lane_errors(struct axi_jesd204_rx *jesd,
				       uint32_t lane, uint32_t *errors)
{
	return axi_jesd204_rx_read(jesd, JESD204_RX_REG_LANE_ERRORS(lane), errors);
}

/**
 * @brief axi_jesd204_rx_laneinfo_8b10b_read
 */
static int32_t axi_jesd204_rx_laneinfo_8b10b_read(struct axi_jesd204_rx *jesd,
		uint32_t lane,
		uint32_t lane_status)
{
	uint32_t octets_per_multiframe;
	uint32_t lane_latency;
	uint32_t val[4];

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LANE_STATUS(lane), &lane_status);

	printf("\tCGS state: %s\n",
	       axi_jesd204_rx_lane_status_label[lane_status & 0x3]);

	printf("\tInitial Frame Synchronization: %s\n",
	       (lane_status & BIT(4)) ? "Yes" : "No");
	if (!(lane_status & BIT(4)))
		return FAILURE;

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LINK_CONF0, &octets_per_multiframe);
	octets_per_multiframe &= 0xffff;
	octets_per_multiframe += 1;

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LANE_LATENCY(lane), &lane_latency);
	printf("\tLane Latency: %"PRIu32" Multi-frames and %"PRIu32" Octets\n",
	       lane_latency / octets_per_multiframe,
	       lane_latency % octets_per_multiframe);

	printf("\tInitial Lane Alignment Sequence: %s\n",
	       (lane_status & BIT(5)) ? "Yes" : "No");

	if (!(lane_status & BIT(5)))
		return FAILURE;

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_ILAS(lane, 0), &val[0]);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_ILAS(lane, 1), &val[1]);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_ILAS(lane, 2), &val[2]);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_ILAS(lane, 3), &val[3]);

	printf("\tDID: %"PRIu32", BID: %"PRIu32", LID: %"PRIu32", "
	       "L: %"PRIu32", SCR: %"PRIu32", F: %"PRIu32"\n",
	       (val[0] >> 16) & 0xff,
	       (val[0] >> 24) & 0xf,
	       (val[1] >> 0) & 0x1f,
	       ((val[1] >> 8) & 0x1f) + 1,
	       (val[1] >> 15) & 0x1,
	       ((val[1] >> 16) & 0xff) + 1
	      );

	printf("\tK: %"PRIu32", M: %"PRIu32", N: %"PRIu32", CS: %"PRIu32", "
	       "N': %"PRIu32", S: %"PRIu32", HD: %"PRIu32"\n",
	       ((val[1] >> 24) & 0x1f) + 1,
	       ((val[2] >> 0) & 0xff) + 1,
	       ((val[2] >> 8) & 0x1f) + 1,
	       (val[2] >> 14) & 0x3,
	       ((val[2] >> 16) & 0x1f) + 1,
	       ((val[2] >> 24) & 0x1f) + 1,
	       (val[3] >> 7) & 0x1
	      );

	printf("\tFCHK: 0x%"PRIX32", CF: %"PRIu32"\n",
	       (val[3] >> 24) & 0xff,
	       (val[3] >> 0) & 0x1f
	      );

	printf("\tADJCNT: %"PRIu32", PHADJ: %"PRIu32", ADJDIR: %"PRIu32", "
	       "JESDV: %"PRIu32", SUBCLASS: %"PRIu32"\n",
	       (val[0] >> 28) & 0xff,
	       (val[1] >> 5) & 0x1,
	       (val[1] >> 6) & 0x1,
	       (val[2] >> 29) & 0x7,
	       (val[2] >> 21) & 0x7
	      );

	printf("\tFC: %"PRIu32" kHz\n", jesd->lane_clk_khz);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_rx_laneinfo_64b66b_read
 */
static int32_t axi_jesd204_rx_laneinfo_64b66b_read(struct axi_jesd204_rx *jesd,
		uint32_t lane,
		uint32_t lane_status)
{
	uint8_t extend_multiblock;

	extend_multiblock = JESD204_EMB_STATE_GET(lane_status);

	printf("\tState of Extended multiblock alignment: %s\n",
	       axi_jesd204_rx_emb_state_label[extend_multiblock]);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_rx_laneinfo_read
 */
int32_t axi_jesd204_rx_laneinfo_read(struct axi_jesd204_rx *jesd, uint32_t lane)
{
	uint32_t lane_status;
	uint32_t errors;

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LANE_STATUS(lane), &lane_status);

	printf("%s lane %"PRIu32" status:\n", jesd->name, lane);

	if (PCORE_VERSION_MINOR(jesd->version) >= 2) {
		axi_jesd204_rx_get_lane_errors(jesd, lane, &errors);
		printf("Errors: %"PRIu32"\n", errors);
	}

	if (jesd->encoder == JESD204_RX_ENCODER_8B10B)
		axi_jesd204_rx_laneinfo_8b10b_read(jesd, lane, lane_status);
	else if (jesd->encoder == JESD204_RX_ENCODER_64B66B)
		axi_jesd204_rx_laneinfo_64b66b_read(jesd, lane, lane_status);

	return SUCCESS;
}

/**
 * @brief axi_jesd204_rx_check_lane_status
 */
bool axi_jesd204_rx_check_lane_status(struct axi_jesd204_rx *jesd,
				      uint32_t lane)
{
	uint32_t status;
	uint32_t errors;
	char error_str[sizeof(" (4294967295 errors)")];

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LANE_STATUS(lane), &status);

	if (jesd->encoder == JESD204_RX_ENCODER_8B10B) {
		status &= 0x3;
		if (status != 0x0)
			return false;
	} else {
		status = JESD204_EMB_STATE_GET(status);
		if (status > JESD204_EMB_STATE_INIT &&
		    status <= JESD204_EMB_STATE_LOCK)
			return false;
	}

	if (PCORE_VERSION_MINOR(jesd->version) >= 2) {
		axi_jesd204_rx_read(jesd, JESD204_RX_REG_LANE_ERRORS(lane), &errors);
		snprintf(error_str, sizeof(error_str), " (%"PRIu32" errors)", errors);
	}

	printf("%s: Lane %"PRIu32" desynced%s, restarting link\n",
	       jesd->name, lane, error_str);

	return true;
}

/**
 * @brief axi_jesd204_rx_watchdog
 */
int32_t axi_jesd204_rx_watchdog(struct axi_jesd204_rx *jesd)
{
	uint32_t link_disabled;
	uint32_t link_status;
	bool restart = false;
	uint32_t i;

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LINK_STATE, &link_disabled);
	if (link_disabled)
		return SUCCESS;

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_LINK_STATUS, &link_status);
	if (link_status == 3) {
		for (i = 0; i < jesd->num_lanes; i++)
			restart |= axi_jesd204_rx_check_lane_status(jesd, i);

		if (restart) {
			axi_jesd204_rx_write(jesd, JESD204_RX_REG_LINK_DISABLE, 0x1);
			mdelay(100);
			axi_jesd204_rx_write(jesd, JESD204_RX_REG_LINK_DISABLE, 0x0);
		}
	}

	return SUCCESS;
}

/**
 * @brief axi_jesd204_rx_apply_config
 */
int32_t axi_jesd204_rx_apply_config(struct axi_jesd204_rx *jesd,
				    struct jesd204_rx_config *config)
{
	uint32_t octets_per_multiframe;
	uint32_t multiframe_align;
	uint32_t val;

	octets_per_multiframe = config->frames_per_multiframe *
				config->octets_per_frame;

	multiframe_align = jesd->data_path_width;

	if (jesd->encoder == JESD204_RX_ENCODER_64B66B &&
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

	axi_jesd204_rx_write(jesd, JESD204_RX_REG_LINK_CONF0, val);

	if (jesd->version >= ADI_AXI_PCORE_VER(1, 7, 'a')) {
		/* beats per multiframe */
		val = (octets_per_multiframe / jesd->tpl_data_path_width) - 1;
		axi_jesd204_rx_write(jesd, JESD204_RX_REG_LINK_CONF4, val);
	}

	if (config->subclass_version == 0) {
		axi_jesd204_rx_write(jesd, JESD204_RX_REG_SYSREF_CONF,
				     JESD204_RX_REG_SYSREF_CONF_SYSREF_DISABLE);
		axi_jesd204_rx_write(jesd, JESD204_RX_REG_LINK_CONF2,
				     JESD204_RX_LINK_CONF2_BUFFER_EARLY_RELEASE);
	}

	return SUCCESS;
}

/**
 * @brief axi_jesd204_rx_init
 */
int32_t axi_jesd204_rx_init(struct axi_jesd204_rx **jesd204,
			    const struct jesd204_rx_init *init)
{
	struct axi_jesd204_rx *jesd;
	uint32_t synth_1;
	uint32_t magic;
	uint32_t status;
	uint32_t tmp;

	jesd = (struct axi_jesd204_rx *)malloc(sizeof(*jesd));
	if (!jesd)
		return FAILURE;

	jesd->name = init->name;
	jesd->base = init->base;
	jesd->device_clk_khz = init->device_clk_khz;
	jesd->lane_clk_khz = init->lane_clk_khz;

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_MAGIC, &magic);
	if (magic != JESD204_RX_MAGIC) {
		printf("%s: Unexpected peripheral identifier %.08"PRIX32"\n",
		       jesd->name, magic);
		goto err;
	}

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_VERSION, &jesd->version);
	if (PCORE_VERSION_MAJOR(jesd->version) != 1) {
		printf("%s: Unsupported peripheral version %"
		       ""PRIu32".%"PRIu32".%"PRIu32"\n",
		       jesd->name,
		       PCORE_VERSION_MAJOR(jesd->version),
		       PCORE_VERSION_MINOR(jesd->version),
		       PCORE_VERSION_PATCH(jesd->version));
		goto err;
	}

	axi_jesd204_rx_read(jesd, JESD204_RX_REG_SYNTH_NUM_LANES,
			    &jesd->num_lanes);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_SYNTH_DATA_PATH_WIDTH, &tmp);
	jesd->data_path_width = 1 << JESD204_SYNTH_DATA_PATH_WIDTH_GET(tmp);
	jesd->tpl_data_path_width = JESD204_TPL_DATA_PATH_WIDTH_GET(tmp);
	axi_jesd204_rx_read(jesd, JESD204_RX_REG_SYNTH_REG_1,
			    &synth_1);
	jesd->encoder = JESD204_RX_ENCODER_GET(synth_1);

	if (jesd->encoder == JESD204_RX_ENCODER_UNKNOWN)
		jesd->encoder = JESD204_RX_ENCODER_8B10B;
	else if (jesd->encoder >= JESD204_RX_ENCODER_MAX)
		goto err;

	jesd->config.octets_per_frame = init->octets_per_frame;
	jesd->config.frames_per_multiframe = init->frames_per_multiframe;
	jesd->config.subclass_version = init->subclass;

	axi_jesd204_rx_lane_clk_disable(jesd);

	status = axi_jesd204_rx_apply_config(jesd, &jesd->config);
	if (status != SUCCESS)
		goto err;

	*jesd204 = jesd;

	return SUCCESS;

err:
	free(jesd);

	return FAILURE;
}

/**
 * @brief axi_jesd204_rx_remove
 */
int32_t axi_jesd204_rx_remove(struct axi_jesd204_rx *jesd)
{
	free(jesd);

	return SUCCESS;
}
