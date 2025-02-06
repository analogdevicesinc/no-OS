

/*******************************************************************************
 *   @file   diag_mngr.h
 *   @brief  Header file for Diagnostic manager for GMSL parts.
 *   @author Automotive Software and Systems team, Bangalore, India
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

#ifndef GMSL_DIAG_MNGR_H
#define GMSL_DIAG_MNGR_H

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "gmsl_common.h"

#define GMSL_DIAG_SUB_TYPE_MAX_COUNT         (GMSL_DIAG_SUB_TYPE_MAX)

#define START_UP                 (0u)
#define INTERVAL_5_SEC           (5u)

/**
*  @enum  gmsl_diag_sub_type
*  @brief Enumeration for GMSL diagnostic sub-types.
*/
enum gmsl_dev_diag_sub_type {
	GMSL_DIAG_PART_DEV_ID = 0,
	GMSL_DIAG_PART_DEV_REV = 1,
	GMSL_DIAG_PART_CFG = 2,
	GMSL_DIAG_LINK_LOCK = 3,
	GMSL_DIAG_LINK_DECODE = 4,
	GMSL_DIAG_LINK_IDLE = 5,
	GMSL_DIAG_LINK_MAX_RETRANS = 6,
	GMSL_DIAG_LINK_LINE_FAULT = 7,
	GMSL_DIAG_LINK_FEC = 8,
	GMSL_DIAG_LINK_EOM = 9,
	GMSL_DIAG_VIDEO_PCLK_LOST = 10,
	GMSL_DIAG_VIDEO_OVERFLOW = 11,
	GMSL_DIAG_VIDEO_DRIFT = 12,
	GMSL_DIAG_VIDEO_LINE_ENABLED = 13,
	GMSL_DIAG_VIDEO_STREAM_ID = 14,
	GMSL_DIAG_VIDEO_PARAMETER = 15,
	GMSL_DIAG_VIDEO_PPS = 16,
	GMSL_DIAG_VIDEO_STREAM_DSC_COMPRESSION = 17,
	GMSL_DIAG_DP_FEC_SUPPORT = 18,
	GMSL_DIAG_HDCP = 19,
	GMSL_DIAG_LINK_TRAINING = 20,
	GMSL_DIAG_VIDEO_CRC_ERROR_STATUS = 21,
	GMSL_DIAG_VIDEO_LOCK = 22,
	GMSL_DIAG_MIPI_STATUS = 23,
	GMSL_DIAG_DP_LINK_RATE = 24,
	GMSL_DIAG_DP_LANE_COUNT = 25,
	GMSL_DIAG_REMOTE_ERROR_FLAG = 26,
	GMSL_DIAG_LMO_STATUS = 27,
	GMSL_DIAG_VID_BLK_LENGTH_ERROR = 28,
	GMSL_DIAG_MIPI_RX = 29,
	GMSL_DIAG_MEM_ECC_2BIT_ERROR = 30,
	GMSL_DIAG_INFOFR_CC = 31,
	GMSL_DIAG_VIDEO_PSM_STATUS = 32,
	GMSL_DIAG_VIDEO_LINE_CRC = 33,
	GMSL_DIAG_VIDEO_ERROR_STATUS = 34,
	GMSL_DIAG_VIDEO_TIMING_DETECT_STATUS = 35,
	GMSL_DIAG_PHY_LOW_POWER_ERR = 36,
	GMSL_DIAG_SUB_TYPE_MAX = 37,
};

typedef void (*fault_call_back)(struct gmsl_dev *dev,
				enum gmsl_dev_diag_sub_type fault_type, void* data, bool pal_err);

/**
 *  @struct gmsl_diag_type_cfg
 *  @brief Structure holding the configuration for a specific GMSL diagnostic type.
 */
struct gmsl_diag_type_cfg {
	enum gmsl_dev_diag_sub_type diag_sub_type;
	uint8_t interval_in_sec;         /*!< If set to Zero, diagnostic check will be done on Init */
	fault_call_back
	call_back;        /*!< Callback function to recieve the fault reporting */
};

/**
 *  @struct gmsl_diag_mngr_cfg
 *  @brief Structure holding the configuration for the GMSL Diagnostics Manager.
 */
struct gmsl_diag_mngr_cfg {
	struct gmsl_diag_type_cfg *diag_type_cfg[GMSL_DIAG_SUB_TYPE_MAX_COUNT];
	struct gmsl_dev *dev;
};

int32_t gmsl_diag_mngr_init(struct gmsl_diag_mngr_cfg **pconfig,
			    uint8_t num_dev, void *pay_load_buffer);
int32_t gmsl_diag_mngr_proc();
int32_t gmsl_diag_mngr_oneshot(struct gmsl_diag_mngr_cfg *mngr_cfg,
			       fault_call_back call_back, enum gmsl_dev_diag_sub_type fault_type);

#endif /*GMSL_DIAG_MNGR_H */