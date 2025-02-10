/***************************************************************************//**
 *   @file   gmsl_dp_ser.h
 *   @brief  The header file for GMSL DP Serializer containing data
 *         structures and MACROs
 *   @author Automotive Software and Systems Team, Istanbul, Turkey
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
#ifndef GMSL_DP_SER_H
#define GMSL_DP_SER_H

#include "gmsl_common.h"
#include <stdbool.h>

/**
 * @enum gmsl_dp_ser_dp_version
 * @brief Enumeration for DisplayPort versions.
 */
enum gmsl_dp_ser_dp_version {
    GMSL_DP_SER_DP_VERSION_1_0 = 0x10u,
    GMSL_DP_SER_DP_VERSION_1_1 = 0x11u,
    GMSL_DP_SER_DP_VERSION_1_2 = 0x12u,
    GMSL_DP_SER_DP_VERSION_1_3 = 0x13u,
    GMSL_DP_SER_DP_VERSION_1_4 = 0x14u
};

/**
 * @enum gmsl_dp_edid_mode
 * @brief Enumerates edid modes.
 */
enum gmsl_dp_edid_mode {
    GMSL_DP_EDID_MODE_ROM = 0u,
    GMSL_DP_EDID_MODE_RAM = 1u,
    GMSL_DP_EDID_MODE_EXTERNAL_PT2 = 2u,
    GMSL_DP_EDID_MODE_DISABLED = 3u
};

/**
 * @enum gmsl_dp_ser_vid_link_sel
 * @brief Enumeration for video link select for each pipe.
 */
enum gmsl_dp_ser_vid_link_sel {
    GMSL_DP_SER_VID_LINK_SEL_A = 0u,      /*!<  GMSL Serializer link select A */
    GMSL_DP_SER_VID_LINK_SEL_B = 1u,      /*!<  GMSL Serializer link select B */
    GMSL_DP_SER_VID_LINK_SEL_A_AND_B = 2u /*!<  GMSL Serializer link select A&B */
};

/**
 * @enum gmsl_dp_ser_link_rate
 * @brief Enumeration for DisplayPort link rate.
 */
enum gmsl_dp_ser_link_rate {
    GMSL_DP_SER_LINK_RATE_1_62 = 0x6u,
    GMSL_DP_SER_LINK_RATE_2_7 = 0xAu,
    GMSL_DP_SER_LINK_RATE_5_4 = 0x14u,
    GMSL_DP_SER_LINK_RATE_8_1 = 0x1Eu
};

/**
 * @enum gmsl_dp_aeq_time
 * @brief Set RLMS Adaptation time constants for DPRX HBR2 Link training.
 */
enum gmsl_dp_aeq_time {
    GMSL_DP_AEQ_TIME_NO_DELAY = 0u,
    GMSL_DP_AEQ_TIME_US_100 = 1u,
    GMSL_DP_AEQ_TIME_US_200 = 2u,
    GMSL_DP_AEQ_TIME_US_300 = 3u,
    GMSL_DP_AEQ_TIME_US_400 = 4u,
    GMSL_DP_AEQ_TIME_MS_3 = 5u,
    GMSL_DP_AEQ_TIME_MS_4 = 6u,
    GMSL_DP_AEQ_TIME_MS_6 = 7u,
    GMSL_DP_AEQ_TIME_MS_7 = 8u,
    GMSL_DP_AEQ_TIME_MS_10 = 9u,
    GMSL_DP_AEQ_TIME_MS_11 = 10u,
    GMSL_DP_AEQ_TIME_MS_12 = 12u,
    GMSL_DP_AEQ_TIME_MS_14 = 13u,
    GMSL_DP_AEQ_TIME_MS_15 = 14u,
    GMSL_DP_AEQ_TIME_MS_16 = 15u,
    GMSL_DP_AEQ_TIME_MS_30 = 16u,
    GMSL_DP_AEQ_TIME_MS_31 = 17u,
    GMSL_DP_AEQ_TIME_MS_32 = 18u,
    GMSL_DP_AEQ_TIME_MS_46 = 19u,
    GMSL_DP_AEQ_TIME_MS_47 = 20u,
    GMSL_DP_AEQ_TIME_MS_48 = 21u,
    GMSL_DP_AEQ_TIME_MS_62 = 22u,
    GMSL_DP_AEQ_TIME_MS_63 = 23u,
    GMSL_DP_AEQ_TIME_MS_64 = 24u,
    GMSL_DP_AEQ_TIME_MS_128 = 25u,
    GMSL_DP_AEQ_TIME_MS_192 = 26u,
    GMSL_DP_AEQ_TIME_MS_256 = 27u,
    GMSL_DP_AEQ_TIME_MS_384 = 28u
};

/**
 * @struct gmsl_dp_rx_cfg
 * @brief Structure for configuring GMSL DP Serializer DPRX settings.
 */
struct gmsl_dp_rx_cfg {
    enum gmsl_dp_ser_dp_version dp_version;    /*!<  DisplayPort version */
    enum gmsl_dp_ser_link_rate lane_rate;   /*!<  Lane rate */
    unsigned int lane_count;                /*!<  Lane count: can be set as 1,2,4 */
    bool mst_enable;                        /*!<  MST enable */
};

/** @enum  gmsl_pipe_input_des
 *  @brief Enumerates input video pipe to Deserializer.
 */
enum gmsl_pipe_input_des {
    GMSL_PIPE_INPUT_DES_X = 0u, /*!< GMSL Serdes Input Video PIPE-X */
    GMSL_PIPE_INPUT_DES_Y = 1u, /*!< GMSL Serdes Input Video PIPE-Y */
    GMSL_PIPE_INPUT_DES_Z = 2u, /*!< GMSL Serdes Input Video PIPE-Z */
    GMSL_PIPE_INPUT_DES_U = 3u  /*!< GMSL Serdes Input Video PIPE-U */
};

/**
 * @struct gmsl_dp_ser_vid_pipe_cfg
 * @brief Structure for configuring GMSL DP Serializer Pipe settings.
 */
struct gmsl_dp_ser_vid_pipe_cfg {
    enum gmsl_pipe_input_des pipe;  /*!< GMSL Serializer selected Pipe to configure: can be set as X,Y,Z,U */
    enum gmsl_dp_ser_vid_link_sel vid_link_sel; /*!< GMSL Serializer link select for the configured pipe: can be set as A, B, A&B */
    unsigned int stream_id;         /*!< GMSL Serializer stream id for the configured pipe: can be set as 0,1,2,3 */
    unsigned int mst_id;            /*!< GMSL Serializer mst id for the configured pipe: can be set as 1,2,3,4 */
    bool dsc_enable;                /*!< GMSL Serializer DSC enable/disable */
};

/**
 * @struct gmsl_dp_ser_state
 * @brief Structure for DP part specific settings.
 */
struct gmsl_dp_ser_state {
    bool mst_enabled; /*!<  MST function enabled/disabled */
};
#endif
