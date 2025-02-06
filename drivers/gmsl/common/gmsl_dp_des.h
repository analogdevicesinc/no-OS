/***************************************************************************//**
 *   @file   gmsl_dp_des.h
 *   @brief  The header file for GMSL DP Deserializer containing data
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
#ifndef GMSL_DP_DES_H
#define GMSL_DP_DES_H

#include <stdbool.h>

/**
 * @enum gmsl_dp_timing_polarity
 * @brief Enumerates DP Timing Polarities.
 */
enum gmsl_dp_timing_polarity {
  GMSL_DP_TIMING_POLARITY_NEGATIVE = 0u,
  GMSL_DP_TIMING_POLARITY_POSITIVE = 1u
};

/**
 * @enum gmsl_dp_link_rate
 * @brief Enumeration for DisplayPort link rate.
 */
enum gmsl_dp_link_rate {
  GMSL_DES_DP_LINK_RATE_1_62 = 0u,
  GMSL_DES_DP_LINK_RATE_2_7 = 1u,
  GMSL_DES_DP_LINK_RATE_5_4 = 2u,
  GMSL_DES_DP_LINK_RATE_8_1 = 3u
};

/**
 * @struct gmsl_dp_cfg
 * @brief Structure for configuring GMSL DP Serializer DPRX settings.
 */
struct gmsl_dp_cfg {
  enum gmsl_dp_link_rate link_rate; /*!<  DP Link rate */
  unsigned int lane_count;          /*!<  DP Lane count: can be set as 1,2,4 */
};

/**
 * @struct gmsl_dp_timing_polarity_cfg
 * @brief Structure for configuring GMSL DP timing polarity settings.
 */
struct gmsl_dp_timing_polarity_cfg {
  enum gmsl_dp_timing_polarity hs; /*!< Horizontal Sync Signal Polarity */
  enum gmsl_dp_timing_polarity vs; /*!< Vertical Sync Signal Polarity */
};

/**
 * @struct gmsl_dp_video_timing
 * @brief Structure for configuring GMSL DP video timing.
 */
struct gmsl_dp_video_timing {
  unsigned int pixel_clock;   /*!< Pixel Clock in KHz */
  unsigned int h_active;      /*!< Horizontal Active Pixels */
  unsigned int h_front_porch; /*!< Horizontal Front Porch */
  unsigned int h_sync_width;  /*!< Horizontal Sync Width */
  unsigned int h_back_porch;  /*!< Horizontal Back Porch */
  unsigned int v_active;      /*!< Vertical Active Lines */
  unsigned int v_front_porch; /*!< Vertical Front Porch */
  unsigned int v_sync_width;  /*!< Vertical Sync Width */
  unsigned int v_back_porch;  /*!< Vertical Back Porch */
};

/**
 * @struct gmsl_dp_des_state
 * @brief Structure for DP Des part specific settings.
 */
struct gmsl_dp_des_state {

};

#endif
