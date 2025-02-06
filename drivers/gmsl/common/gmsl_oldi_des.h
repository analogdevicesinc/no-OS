/***************************************************************************//**
 *   @file   gmsl_oldi_des.h
 *   @brief  The header file for GMSL oLDI Deserializer containing data
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
#ifndef GMSL_OLDI_DES_H
#define GMSL_OLDI_DES_H

#include <stdbool.h>

/**
 * @enum gmsl_power_config_mode
 * @brief Enumeration for Power Configuration Modes.
 */
enum gmsl_power_config_mode {
  /*!< Video output disabled, daisy-chain output enabled */
  GMSL_POWER_CONFIG_MODE_REPEATER = 0u, 

  /*!< Video and daisy-chain outputs are enabled */
  GMSL_POWER_CONFIG_MODE_ACTIVE = 1u, 

  /*!< Video output enabled, daisy-chain output disabled */
  GMSL_POWER_CONFIG_MODE_SINK = 2u
};

/**
 * @enum gmsl_oldi_spl_pol
 * @brief Enumeration for OLDI Splitter Polarization.
 */
enum gmsl_oldi_spl_pol {
  GMSL_OLDI_SPL_POL_FALLING = 0u,   /*!< OLDI Splitter Polarization Falling edge */
  GMSL_OLDI_SPL_POL_RISING = 1u     /*!< OLDI Splitter Polarization Rising edge */
};

/**
 * @enum gmsl_oldi_spl_mode
 * @brief Enumeration for OLDI Splitter Mode.
 */
enum gmsl_oldi_spl_mode {
  GMSL_OLDI_SPL_MODE_RANDOM = 0u,   /*!< OLDI Splitter Mode Random */
  GMSL_OLDI_SPL_MODE_HS = 1u,       /*!< OLDI Splitter Mode Split with HS */
  GMSL_OLDI_SPL_MODE_VS = 2u,       /*!< OLDI Splitter Mode Split with VS */
  GMSL_OLDI_SPL_MODE_DE = 3u        /*!< OLDI Splitter Mode Split with DE */
};

/**
 * @enum gmsl_oldi_lane_count
 * @brief Enumeration for OLDI Lane Count.
 */
enum gmsl_oldi_lane_count {
  GMSL_OLDI_LANE_CNT_4 = 0u,    /*!< OLDI Lane Count 4 */
  GMSL_OLDI_LANE_CNT_3 = 1u     /*!< OLDI Lane Count 3 */
};

/**
 * @enum gmsl_oldi_format
 * @brief Enumeration for OLDI Output Format of LVDS port.
 */
enum gmsl_oldi_format {
  GMSL_OLDI_FORMAT_OLDI = 0u,   /*!< OLDI Output Format OLDI */
  GMSL_OLDI_FORMAT_VESA = 1u    /*!< OLDI Output Format VESA */
};

/**
 * @enum gmsl_oldi_output_port
 * @brief Enumeration for OLDI Output Port.
 */
enum gmsl_oldi_output_port {
    GMSL_OLDI_OUTPUT_PORTA = 0u,    /*!< OLDI Output Port A */
    GMSL_OLDI_OUTPUT_PORTB = 1u     /*!< OLDI Output Port B */
};

/**
 * @struct gmsl_oldi_cfg
 * @brief Structure for configuring GMSL OLDI settings.
 */
struct gmsl_oldi_cfg {
    enum gmsl_oldi_spl_pol spl_pol;     /*!< OLDI Splitter Polarization */
    enum gmsl_oldi_spl_mode spl_mode;   /*!< OLDI Splitter Mode */
    bool spl_en;                        /*!< OLDI Splitter Enable */
    bool swap_AB_ports;                 /*!< OLDI Swap A and B Ports */
    enum gmsl_oldi_lane_count lane_cnt; /*!< OLDI Lane Count */
    enum gmsl_oldi_format format;       /*!< OLDI Output Format */
    enum gmsl_oldi_output_port port;    /*!< OLDI Output Port */
};

#endif
