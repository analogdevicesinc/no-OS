/***************************************************************************//**
 *   @file   gmsl_cam_des.h
 *   @brief  Header file of gmsl camera deserializer common interface
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

#ifndef GMSL_CAM_DES_H
#define GMSL_CAM_DES_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "gmsl_common.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define GMSL_CAM_DES_CSI2_MAX_DATA_LANES	       (4) 
#define GMSL_CAM_DES_CSI2_MAX_DES_REMAPS_NUM       (16)
#define GMSL_CAM_DES_DT_VC(dt, vc)                 (((vc) & 0x3) << 6 | ((dt) & 0x3f))
#define MAX_NUM_LINKS                              (4)
/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum gmsl_cc_port
 * @brief Enumerates Control Channel Ports A/B of Deserializer.
 */
enum gmsl_cc_port{
    GMSL_CC_PORTA = 0U,               /*!< GMSL Camera Deserializer I2C PORT-A, main I2C Channel */
    GMSL_CC_PORTB = 1U,               /*!< GMSL Camera Deserializer I2C PORT-B, passthrough I2C Channel */
};

/**
 * @enum gmsl_csi_des_port
 * @brief Enumeration for GMSL Camera Deserializer CSI ports.
 */
enum gmsl_csi_des_port{
    GMSL_CSI_DES_PORTA = 0U,                 /*!< GMSL Camera Deserializer CSI PORT-A */
    GMSL_CSI_DES_PORTB = 1U,                 /*!< GMSL Camera Deserializer CSI PORT-B */
};

/**
 * @enum gmsl_dev_des_mipi_lane_cnt
 * @brief Enumerates the MIPI lane counts for the GMSL Camera Deserializer.
 */
enum gmsl_dev_des_mipi_lane_cnt{
    GMSL_DEV_DES_MIPI_LANE_CNT_1 = 0U,            /*!< GMSL Camera Deserializer MIPI Lane Count 1 */
    GMSL_DEV_DES_MIPI_LANE_CNT_2 = 1U,            /*!< GMSL Camera Deserializer MIPI Lane Count 2 */
    GMSL_DEV_DES_MIPI_LANE_CNT_3 = 2U,            /*!< GMSL Camera Deserializer MIPI Lane Count 3 */
    GMSL_DEV_DES_MIPI_LANE_CNT_4 = 3U,            /*!< GMSL Camera Deserializer MIPI Lane Count 4 */
};

/**
 * @enum gmsl_link_des_rx_phy_rate
 * @brief Enumerates the RX PHY rates for the GMSL Camera Deserializer.
 */
enum gmsl_link_des_rx_phy_rate{
    GMSL_LINK_DES_RX_PHY_RATE_3G  = 1U,      /*!< GMSL Camera Deserializer RX PHY Rate 3 Gbps */
    GMSL_LINK_DES_RX_PHY_RATE_6G  = 2U,      /*!< GMSL Camera Deserializer RX PHY Rate 6 Gbps */
    GMSL_LINK_DES_RX_PHY_RATE_12G = 3U,      /*!< GMSL Camera Deserializer RX PHY Rate 12 Gbps */
};

/**
 * @enum gmsl_pipe_des_vc_remap_mode
 * @brief Enumerates the virtual channel remap modes for the GMSL Camera Deserializer.
 */
enum gmsl_pipe_des_vc_remap_mode{
    GMSL_PIPE_DES_VC_REMAP_SRC_DST_MODE = 0U,       /*!< GMSL Camera Deserializer Virtual Channel Remap Mode: SRC_DST_MODE*/
    GMSL_PIPE_DES_VC_REMAP_PIPE_ID_MODE = 1U,       /*!< GMSL Camera Deserializer Virtual Channel Remap Mode: PIPE_ID_MODE*/
    GMSL_PIPE_DES_VC_REMAP_LINK_ID_MODE = 2U,       /*!< GMSL Camera Deserializer Virtual Channel Remap Mode: LINK_ID_MODE*/
};

/**
 * @struct gmsl_csi_mipi_cfg - Configuration for GMSL Camera Deserializer MIPI CSI2
 * @brief: GMSL Camera Deserializer MIPI CSI2 configuration structure
 */
struct gmsl_csi_mipi_cfg {
    unsigned int flags;                                           /*!< GMSL Camera Deserializer MIPI CSI2 flags, used in kernel space*/
    unsigned char data_lanes[GMSL_CAM_DES_CSI2_MAX_DATA_LANES];   /*!< GMSL Camera Deserializer MIPI CSI2 data lanes (1 - 4)*/
    unsigned char clock_lane;                                     /*!< GMSL Camera Deserializer MIPI CSI2 clock lanes (0 - 1)*/
    unsigned char num_data_lanes;                                 /*!< GMSL Camera Deserializer MIPI CSI2 number of data lanes (1 - 4)*/
    bool lane_polarities[GMSL_CAM_DES_CSI2_MAX_DATA_LANES];       /*!< GMSL Camera Deserializer MIPI CSI2 lane polarities -> true/false */
};

/**
 * @struct gmsl_pipe_des_dt_vc_remap
 * @brief A structure that contains data type and virtual channel ID mappings for the Deserializer.
 */
struct gmsl_pipe_des_dt_vc_remap {
    unsigned char from_dt;          /*!< GMSL Camera Deserializer input data type: values to be set from DataType Macros */
    unsigned char from_vc;          /*!< GMSL Camera Deserializer input virtual channel ID (0-15) */
    unsigned char to_dt;            /*!< GMSL Camera Deserializer output data type  values to be set from DataType Macros*/
    unsigned char to_vc;            /*!< GMSL Camera Deserializer output virtual channel ID (0-15)*/
    unsigned char phy;              /*!< GMSL Camera Deserializer phy related to this remap (0 - 3) */
};

/**
 * @struct gmsl_pipe_des_pipe_cfg
 * @brief A structure that contains video pipe configurations for Deserializer.
 */
struct gmsl_pipe_des_pipe_cfg {
    enum gmsl_pipe index;                                                            	/*!< GMSL Camera Deserializer pipe index (0-7)*/
    unsigned int phy_id;                                                           	/*!< GMSL Camera Deserializer phy ID (0-3) */
    unsigned int stream_id;                                                        	/*!< GMSL Camera Deserializer stream ID (0-3) */
    uint8_t link_id;                                                               	/*!< GMSL Camera Deserializer link ID (0-3) */
    struct gmsl_pipe_des_dt_vc_remap remaps[GMSL_CAM_DES_CSI2_MAX_DES_REMAPS_NUM];  /*!< GMSL Camera Deserializer video streams (remaps) */
    unsigned int num_remaps;                                                       	/*!< GMSL Camera Deserializer number of video streams (remaps) per pipe (0-15)*/
    unsigned int con_dest;                                                         	/*!< GMSL Camera Deserializer controller destination */
    enum gmsl_pipe input_pipe;                                       				/*!< GMSL Camera Deserializer video input pipe */
    enum gmsl_pipe_des_vc_remap_mode vc_remap_mode;                            		/*!< GMSL Camera Deserializer virtual channel remap mode */
    unsigned int ser_cphy_lane_cnt;                                                	/*!< GMSL Camera Deserializer serealizer cphy lane count */
    bool ser_cphy_mode;                                                            	/*!< GMSL Camera Deserializer serealizer cphy mode: true -> cphy, false -> dphy */
    bool dbl8;                                                                     	/*!< GMSL Camera Deserializer double8, true->selected */
    bool dbl10;                                                                    	/*!< GMSL Camera Deserializer double10, true->selected */
    bool dbl12;                                                                    	/*!< GMSL Camera Deserializer double12, true->selected */
    bool dbl8mode;                                                                 	/*!< GMSL Camera Deserializer double mode 8 enabled/disabled */
    bool dbl10mode;                                                                	/*!< GMSL Camera Deserializer double mode 10 enabled/disabled */
    bool enabled;                                                                  	/*!< GMSL Camera Deserializer PIPEx enabled/disabled */
};

/**
 * @struct gmsl_link_des_phy_cfg
 * @brief A structure that contains PHY configurations for the Deserializer.
 */
struct gmsl_link_des_phy_cfg {
unsigned int index;                                    		/*!< GMSL Camera Deserializer phy index (0-3) */
    unsigned long long link_frequency;                     	/*!< GMSL Camera Deserializer link frequency */
    unsigned int mipi_clk;                                 	/*!< GMSL Camera Deserializer mipi clock (80Mbps - 4500Mbps)*/
    struct gmsl_csi_mipi_cfg mipi;                         	/*!< GMSL Camera Deserializer mipi configuration */
    enum gmsl_link_des_rx_phy_rate phy_rate;             	/*!< GMSL Camera Deserializer phy rate */
    bool alt_mem_map8;                                     	/*!< GMSL Camera Deserializer alt memory map for 8-bit DT */
    bool alt2_mem_map8;                                    	/*!< GMSL Camera Deserializer alt2 memory map for 8-bit DT */
    bool alt_mem_map10;                                    	/*!< GMSL Camera Deserializer alt memory map for 10-bit DT */
    bool alt_mem_map12;                                    	/*!< GMSL Camera Deserializer alt memory map for 12-bit DT */
    bool enabled;                                          	/*!< GMSL Camera Deserializer PHYx enabled/disabled */
    bool cphy_enabled;                                     	/*!< GMSL Camera Deserializer CPHY enabled/disabled */
};

/**
 * @struct gmsl_dev_csi_des_init_param
 * @brief GMSL Deserializer driver initailization structure
 */
struct gmsl_dev_csi_des_init_param {
    char *dev_name;										/*!< GMSL Camera Serializer custom device name */
    uint8_t index;										/*!< GMSL devices index */
    bool tunnel_mode;                					/*!< GMSL Camera Deserializer tunnel mode */
    struct gmsl_cam_des_phy *phys;   					/*!< GMSL Camera Deserializer PHYs */
    struct gmsl_cam_des_pipe *pipes; 					/*!< GMSL Camera Deserializer PIPES */
    struct gmsl_cam_des_link *links; 					/*!< GMSL Camera Deserializer LINKs */
    enum gmsl_link_des_rx_phy_rate link_rx_rate[MAX_NUM_LINKS]; /*!< GMSL Camera Deserializer link rates */

	/** @todo: Implement memory allocation from application for drivers */
    void *dev_instance_mem;								/*!< GMSL Camera Serializer device instance memory */
    void *dev_state_mem;								/*!< GMSL Camera Serializer device state memory */
}; 

#endif // GMSL_CAM_DES_H