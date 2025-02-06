/*****************************************************************************
 *   @file   max96792.h
 *   @brief  Header file of MAX96792 Driver.
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#ifndef MAX96792_H
#define MAX96792_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "gmsl_common.h"
#include "gmsl_dbg.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "gmsl_cam_des.h"

#define MAX96792_DEV_STATE_MEM_SIZE         (sizeof(struct max96792_state))
#define MAX96792_MAX_DATA_LANES              (4U)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum max96792_link
 * @brief Enumeration for MAX96792 GMSL links.
 */
enum max96792_link {
	MAX96792_LINK_A = 0u, /*!< GMSL link A */
	MAX96792_LINK_B = 1u, /*!< GMSL link B */
};

/**
 * @enum  max96792_video_pipes
 * @brief Enumerates video pipes in the device.
 */
enum max96792_video_pipes {
	MAX96792_VIDEO_PIPE_Y = 0u, /*!< Video pipe Y */
	MAX96792_VIDEO_PIPE_Z = 1u, /*!< Video pipe Z */
};

enum max96792_data_lanes {
	MAX96792_DATA_LANE_0    = 0,
	MAX96792_DATA_LANE_1    = 1,
	MAX96792_DATA_LANE_2    = 2,
	MAX96792_DATA_LANE_3    = 3,
};
/**
 * @enum  MAX96792_DATA_TYPE
 * @brief DATA Type for MAX96792 device.
 */
enum max96792_data_type {
	MAX96792_DATA_TYPE_DEFAULT      = 0x0u,           /*!< Default */
	MAX96792_DATA_TYPE_GENERIC8_A   = 0x10u,          /*!< GENERIC8 */
	MAX96792_DATA_TYPE_GENERIC8_B   = 0x11u,          /*!< GENERIC8 */
	MAX96792_DATA_TYPE_EMB8         = 0x12u,          /*!< EMBEDDED */
	MAX96792_DATA_TYPE_YUV422_8BIT  = 0x1Eu,          /*!< YUV422 8-bit */
	MAX96792_DATA_TYPE_YUV422_10BIT = 0x1Fu,          /*!< YUV422 10-bit */
	MAX96792_DATA_TYPE_RGB565       = 0x22u,          /*!< RGB565 */
	MAX96792_DATA_TYPE_RGB666       = 0x23u,          /*!< RGB666 */
	MAX96792_DATA_TYPE_RGB888       = 0x24u,          /*!< RGB888 */
	MAX96792_DATA_TYPE_RAW8         = 0x2Au,          /*!< RAW8 */
	MAX96792_DATA_TYPE_RAW10        = 0x2Bu,          /*!< RAW10 */
	MAX96792_DATA_TYPE_RAW12        = 0x2Cu,          /*!< RAW12 */
	MAX96792_DATA_TYPE_RAW14        = 0x2Du,          /*!< RAW14 */
	MAX96792_DATA_TYPE_RAW16        = 0x2Eu,          /*!< RAW16 */
	MAX96792_DATA_TYPE_RAW20        = 0x2Fu,          /*!< RAW20 */
	MAX96792_DATA_TYPE_YUV422_12BIT = 0x30u,          /*!< YUV422 12-bit */
	MAX96792_DATA_TYPE_UDP8_A       = 0x31u,          /*!< UDP8 */
	MAX96792_DATA_TYPE_UDP8_B       = 0x32u,          /*!< UDP8 */
	MAX96792_DATA_TYPE_UDP8_C       = 0x33u,          /*!< UDP8 */
	MAX96792_DATA_TYPE_UDP8_D       = 0x34u,          /*!< UDP8 */
	MAX96792_DATA_TYPE_UDP8_E       = 0x35u,          /*!< UDP8 */
	MAX96792_DATA_TYPE_UDP8_F       = 0x36u,          /*!< UDP8 */
	MAX96792_DATA_TYPE_UDP8_G       = 0x37u           /*!< UDP8 */
};

/**
 * @struct max96792_dev_capabilities
 * @brief MAX96792 device capabilities.
 */
struct max96792_dev_capabilities {
	uint8_t max_links_count; /*!< Maximum number of Links in the device */
	uint8_t max_lmn_count;   /*!< Maximum number of Line Fault Monitors in the device */
	uint8_t max_phy_ctrlr;   /*!< Maximum number of Phy controllers in the device */
};

/**
 * @struct max96792_state
 * @brief MAX96792 device state.
 */
struct max96792_state {
	uint8_t dummy;          /*!< MAX96792 state */
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t max96792_init(struct gmsl_dev **device, struct no_os_i2c_desc *i2c_desc,
		      void *init_param);
int32_t max96792_remove(struct gmsl_dev *dev);

#endif /* MAX96792_H */
