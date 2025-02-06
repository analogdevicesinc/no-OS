/***************************************************************************//**
 *   @file   max96792_diag.h
 *   @brief  Header file of Diagnostics Driver Implementation of max96792 Deserializer.
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

#ifndef MAX96792_DIAG_H
#define MAX96792_DIAG_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "gmsl_common.h"
#include "max96792.h"
/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define NOT_APPLICABLE_FOR_MAX96792 (NULL)          /*!< Not Applicable for max96792 */
#define MAX96792_NUM_OF_MIPI_PKT_READ (4U)          /*!< Number of MIPI packets read */
#define MAX96792_NUM_OF_VIDEO_PIPES (2U)            /*!< Number of video pipes       */
#define MAX96792_NUM_OF_GMSL_LINKS (2U)             /*!< Number of GMSL links       */
#define MAX96792_NUM_OF_CSI_CONTROLLERS (2U)        /*!< Number of MIPI CSI controllers*/
#define MAX96792_NUM_OF_MIPI_PHY (4U)               /*!< Number of MIPI PHYs*/
#define MAX96792_NUM_OF_LINE_FAULT_MONITORS (4U)    /*!< Number of Line Fault Monitors*/
#define MAX96792_MAX_NUM_OF_SRC_DST_DT_VC_MAP (16U) /*!< Maximum number of source-destination data type virtual channel map */

#define MAX96792A_DEV_ID (0xBE)
#define MAX96792F_DEV_ID (0xB8)

#define MAX96792_LINK_IDLE_REG_OFFSET_MULTIPLIER (0x4FF6U)     /*!< Link Idle Register Offset Multiplier */
#define MIPI_REG_OFFSET_MULT (0x40U)                           /*!< MIPI Register Offset Multiplier */
#define VIDEO_PIPE_REG_OFFSET_MULT (0x12U)                     /*!< Video Pipe Register Offset Multiplier */
#define MAX96792_IDLE_ERROR_COUNT_REG_OFFSET_A (0x09u)         /*!< Idle Error Count Register Offset for Link A */
#define MAX96792_IDLE_ERROR_COUNT_REG_OFFSET_B (0x13u)         /*!< Idle Error Count Register Offset for Link B */
#define MAX96792_PIPE_DIAG_CHECK_CRC_ERROR_OFFSET_MULT (0x12U) /*!< Pipe Diagnostic Check CRC Error Offset Multiplier */
#define MAX96792_NUM_OF_PHY_PER_CSI (2U)                       /*!< Number of PHY per CSI controller */
#define MAX96792_BLK_LEN_ERR_OFFSET_MULT (0x12U)               /*!< Block Length Error Offset Multiplier */
#define MAX96792_MAX_STREAM_ID (3U)                            /*!< Maximum Stream ID */
#define MAX96792_STREAM_ID_EXTRACTION_MASK (0x3U)              /*!< Stream ID Extraction Mask */

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct max96792_dev_diag_device_id_status
 * @brief Structure to hold the device ID status for the MAX96792 deserializer.
 */
struct max96792_dev_diag_device_id_status
{
    uint8_t dev_id; /*!< Device ID of the MAX96792 deserializer */
};

/**
 * @struct max96792_dev_diag_dev_rev_status
 * @brief Structure to hold the device revision status of the MAX96792 deserializer.
 */
struct max96792_dev_diag_dev_rev_status
{
    uint8_t dev_rev; /*!< Device Revision of the MAX96792 deserializer */
};

/**
 * @struct max96792_link_diag_link_lock_status
 * @brief Structure to hold the link lock status for MAX96792 deserializer.
 */
struct max96792_link_diag_link_lock_status
{
    bool link_lock_a; /*!< Link Lock Status of Link A */
    bool link_lock_b; /*!< Link Lock Status of Link B */
};

/**
 * @struct max96792_diag_link_decode_err
 * @brief Structure to hold the decoding error status and count for GMSL links.
 */
struct max96792_diag_link_decode_err
{
    enum max96792_link max96792_gmsl_link; /*!< Device GMSL link enumaration*/
    bool decode_err_link_flag;             /*!< Decoding Error Status of Links*/
    uint8_t decode_err_cnt;                /*!< Decoding Error Count of Links*/
};

/**
 * @struct max96792_pipe_diag_decode_err_status
 * @brief Structure to hold the decoding error status for MAX96792 GMSL links.
 */
struct max96792_pipe_diag_decode_err_status
{
    enum max96792_link max96792_gmsl_link[MAX96792_NUM_OF_GMSL_LINKS]; /*!< Device GMSL link enumaration*/
    bool decode_err_link_flag[MAX96792_NUM_OF_GMSL_LINKS];             /*!< Decoding Error Status of Links*/
    uint8_t decode_err_cnt[MAX96792_NUM_OF_GMSL_LINKS];                /*!< Decoding Error (OEN) Count of Links*/
};

/**
 * @enum max96792_link_diag_lmn_status
 * @brief Enumeration for MAX96792 link diagnostic status.
 */
enum max96792_link_diag_lmn_status
{
    MAX96792_LINK_DIAG_LMN_SHORT_TO_BAT = 0u,      /*!< Short to battery */
    MAX96792_LINK_DIAG_LMN_SHORT_TO_GND = 1u,      /*!< Short to GND */
    MAX96792_LINK_DIAG_LMN_NORMAL = 2u,            /*!< Normal operation */
    MAX96792_LINK_DIAG_LMN_LINE_OPEN = 3u,         /*!< Line open */
    MAX96792_LINK_DIAG_LMN_LINE_TO_LINE_SHORT = 4u /*!< Line to line short */
};

/**
 * @enum max96792_link_lmn
 * @brief Enumeration for MAX96792 line fault monitors.
 */
enum max96792_link_lmn
{
    MAX96792_LINK_LMN_0 = 0u, /*!< Line fault monitor 0 */
    MAX96792_LINK_LMN_1 = 1u, /*!< Line fault monitor 1 */
    MAX96792_LINK_LMN_2 = 2u, /*!< Line fault monitor 2 */
    MAX96792_LINK_LMN_3 = 3u, /*!< Line fault monitor 3 */
};

/**
 * @struct max96792_link_diag_line_fault_details
 * @brief Structure to hold the details of line fault diagnostics for MAX96792.
 */
struct max96792_link_diag_line_fault_details
{
    bool lf_int;                                                                            /*!< Line Fault Interrupt status. Asserted when any of line fault monitors indicate a fault status. */
    enum max96792_link_diag_lmn_status lmn_err_status[MAX96792_NUM_OF_LINE_FAULT_MONITORS]; /*!< Line Fault status for each Line Fault Monitors. */
};

/**
 * @enum max96792_dp_pclkdetlost_status
 * @brief Enumeration for the PClk detect lost status in the MAX96792 deserializer.
 */
enum max96792_dp_pclkdetlost_status
{
    MAX96792_DP_PCLKDET_NOT_LOST = 0u, /*!< PClk Detect not lost */
    MAX96792_DP_PCLKDET_LOST = 1u,     /*!< PClk Detect lost */
};

/**
 * @enum max96792_dp_pclkdetlost_assert_event
 * @brief Enumeration for PCLKDET lost assert event configuration.
 */
enum max96792_dp_pclkdetlost_assert_event
{
    MAX96792_DP_PCLKDETLOST_NO_ASSERT_ON_RST = 0u, /*!< Check if video is reset or not and do not assert if it is reset */
    MAX96792_DP_PCLKDETLOST_ASSERT = 1u,           /*!< Do not check if video is reset or not and assert everytime if pclkdet is lost. */
};

/**
 * @enum max96792_csi_packet_stream_id
 * @brief Enumeration for MAX96792 CSI packet stream IDs.
 */
enum max96792_csi_packet_stream_id
{
    MAX96792_CSI_STREAM_ID_0 = 0u, /*!< Packet Stream ID 0 */
    MAX96792_CSI_STREAM_ID_1 = 1u, /*!< Packet Stream ID 1 */
    MAX96792_CSI_STREAM_ID_2 = 2u, /*!< Packet Stream ID 2 */
    MAX96792_CSI_STREAM_ID_3 = 3u  /*!< Packet Stream ID 3 */
};

/**
 * @enum max96792_csi_mipi_phy
 * @brief Enumeration for MAX96792 MIPI CSI PHY interfaces.
 */
enum max96792_csi_mipi_phy
{
    MAX96792_CSI_MIPI_PHY_0 = 0u, /*!< MIPI CSI Phy 0 */
    MAX96792_CSI_MIPI_PHY_1 = 1u, /*!< MIPI CSI Phy 1 */
    MAX96792_CSI_MIPI_PHY_2 = 2u, /*!< MIPI CSI Phy 2 */
    MAX96792_CSI_MIPI_PHY_3 = 3u  /*!< MIPI CSI Phy 3 */

};

/**
 * @enum max96792_dev_csi_mode
 * @brief Enumeration for MAX96792 device CSI modes.
 */
enum max96792_dev_csi_mode
{
    MAX96792_DEV_CSI_PIXEL_MODE = 0u, /*!< Pixel Mode */
    MAX96792_DEV_CSI_TUNNEL_MODE = 1u /*!< Tunnel Mode */
};

/**
 * @enum max96792_dev_lane_count
 * @brief Enumeration for the number of lanes supported by the MAX96792 device.
 */
enum max96792_dev_lane_count
{
    MAX96792_DEV_LANE_COUNT_1 = 0u, /*!< 1 lane supported (lane 0 ) */
    MAX96792_DEV_LANE_COUNT_2 = 1u, /*!< 2 lane supported (lane 0,1 ) */
    MAX96792_DEV_LANE_COUNT_3 = 2u, /*!< 3 lane supported (lane 0,1,2 ) */
    MAX96792_DEV_LANE_COUNT_4 = 3u, /*!< 4 lane supported (lane 0,1,2,3 ) */
};

/**
 * @enum  max96792_dev_csi_lane_frequency
 * @brief Enumerates CSI PHY1 output frequency in multiples of 100Mbps/lane.
 */
enum max96792_dev_csi_lane_frequency
{
    MAX96792_CSI_PER_LANE_FREQ_80Mbps = 0x0u,    /*!< 80Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_100Mbps = 0x1u,   /*!< 100Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_200Mbps = 0x2u,   /*!< 200Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_300Mbps = 0x3u,   /*!< 300Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_400Mbps = 0x4u,   /*!< 400Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_500Mbps = 0x5u,   /*!< 500Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_600Mbps = 0x6u,   /*!< 600Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_700Mbps = 0x7u,   /*!< 700Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_800Mbps = 0x8u,   /*!< 800Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_900Mbps = 0x9u,   /*!< 900Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1000Mbps = 0xAu,  /*!< 1000Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1100Mbps = 0xBu,  /*!< 1100Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1200Mbps = 0xCu,  /*!< 1200Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1300Mbps = 0xDu,  /*!< 1300Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1400Mbps = 0xEu,  /*!< 1400Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1500Mbps = 0xFu,  /*!< 1500Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1600Mbps = 0x10u, /*!< 1600Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1700Mbps = 0x11u, /*!< 1700Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1800Mbps = 0x12u, /*!< 1800Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_1900Mbps = 0x13u, /*!< 1900Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_2000Mbps = 0x14u, /*!< 2000Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_2100Mbps = 0x15u, /*!< 2100Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_2200Mbps = 0x16u, /*!< 2200Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_2300Mbps = 0x17u, /*!< 2300Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_2400Mbps = 0x18u, /*!< 2400Mbps per lane */
    MAX96792_CSI_PER_LANE_FREQ_2500Mbps = 0x19u, /*!< 2500Mbps per lane */
};

/**
 * @struct max96792_pipe_diag_src_dst_dt_vc_map_status
 * @brief Structure to hold the source and destination data type and virtual channel mapping status for MAX96792 pipes.
 */
struct max96792_pipe_diag_src_dst_dt_vc_map_status
{
    uint8_t src_pipe_dt; /*!< Data Type for Source Pipes */
    uint8_t src_pipe_vc; /*!< Virtual Channel for Source Pipes */
    uint8_t dst_pipe_dt; /*!< Data Type for Destination Pipes */
    uint8_t dst_pipe_vc; /*!< Virtual Channel for Destination Pipes */
};

/**
 * @struct max96792_csi_diag_mipi_phy_lane_polarity_map
 * @brief Structure to represent the polarity map of MIPI PHY lanes for the MAX96792 deserializer.
 */
struct max96792_csi_diag_mipi_phy_lane_polarity_map
{
    bool lane_0;   /*!< Lane 0 Polarity */
    bool lane_1;   /*!< Lane 1 Polarity */
    bool clk_lane; /*!< Clock Lane Polarity */
};

/**
 * @struct max96792_dev_diag_part_cfg_status
 * @brief Structure to hold diagnostic configuration status for MAX96792 device.
 */
struct max96792_dev_diag_part_cfg_status
{
    enum max96792_csi_packet_stream_id stream_id[MAX96792_NUM_OF_VIDEO_PIPES];                                                                /*!< Stream ID for pipe */
    enum max96792_dev_csi_mode mode[MAX96792_NUM_OF_CSI_CONTROLLERS];                                                                         /*!< Video Mode */
    struct max96792_pipe_diag_src_dst_dt_vc_map_status src_dst_dt_vc_map[MAX96792_NUM_OF_VIDEO_PIPES][MAX96792_MAX_NUM_OF_SRC_DST_DT_VC_MAP]; /*!< Source Destination Virtual Channel and Data Type Map information */
    uint8_t dphy_dst[MAX96792_NUM_OF_CSI_CONTROLLERS][MAX96792_MAX_NUM_OF_SRC_DST_DT_VC_MAP];                                                 /*!< DPHY Destination Map */
    enum max96792_dev_lane_count lane_count[MAX96792_NUM_OF_MIPI_PHY];                                                                        /*!< Lane count set for the device */
    uint8_t lane_map[MAX96792_NUM_OF_CSI_CONTROLLERS];                                                                                        /*!< Lane Mapping */
    struct max96792_csi_diag_mipi_phy_lane_polarity_map lane_pol[MAX96792_NUM_OF_MIPI_PHY];                                                   /*!< MIPI CSI Phy Lane Polarity Map information */
    bool mipi_out_enabled;                                                                                                                    /*!< Enabled MIPI out */
    enum max96792_dev_csi_lane_frequency csi_lane_freq[MAX96792_NUM_OF_MIPI_PHY];                                                             /*!< CSI PHY output frequency in multiples of 100Mbps/lane */
    bool csi_lane_freq_sw_override[MAX96792_NUM_OF_MIPI_PHY];                                                                                 /*!< CSI PHY software-override disable for frequency fine-tuning */
    bool pll_func_reset;                                                                                                                      /*!< PLL Functional Reset */
};

/**
 * @struct max96792_pipe_diag_video_lock_status
 * @brief Structure to hold the video lock status for MAX96792 deserializer.
 */
struct max96792_pipe_diag_video_lock_status
{
    bool video_lock_enabled[MAX96792_NUM_OF_VIDEO_PIPES]; /*!< Video Lock status for Pipes Y and Z. */
    bool video_lock[MAX96792_NUM_OF_VIDEO_PIPES];         /*!< Video Lock status for Pipes Y and Z. */
};

/**
 * @struct max96792_csi_diag_stream_id_status
 * @brief Structure to hold the status of a CSI diagnostic stream ID for the MAX96792 deserializer.
 */
struct max96792_csi_diag_stream_id_status
{
    enum max96792_csi_packet_stream_id stream_id[MAX96792_NUM_OF_VIDEO_PIPES]; /*!< Stream ID for video pipe. */
};

/**
 * @struct max96792_pipe_diag_idle_error_status
 * @brief Structure to hold the idle error status and count for each GMSL link.
 */
struct max96792_pipe_diag_idle_error_status
{
    bool idle_error_detected[MAX96792_NUM_OF_GMSL_LINKS]; /*!< Idle error status for each link. */
    uint8_t idle_error_count[MAX96792_NUM_OF_GMSL_LINKS]; /*!< Idle error count for each link. */
};

/**
 * @struct max96792_csi_mipi_status
 * @brief Structure to hold the status of the MIPI CSI output.
 */
struct max96792_csi_mipi_status
{
    bool mipi_out_enabled;                                                                /*!< Enabled MIPI out. */
    uint8_t csi2_pkt_cnt[MAX96792_NUM_OF_CSI_CONTROLLERS][MAX96792_NUM_OF_MIPI_PKT_READ]; /*!<  Packet Count sent out of CSI2 Controller 1. */
    uint8_t phy_pkt_cnt[MAX96792_NUM_OF_MIPI_PHY][MAX96792_NUM_OF_MIPI_PKT_READ];         /*!<  Packet Count out of of MIPI PHY1. */
};

/**
 * @struct max96792_pipe_max_retrans_data
 * @brief Structure to hold the maximum retransmission data for various channels in the MAX96792 deserializer.
 */
struct max96792_link_max_retrans_data
{
    bool main_control_channel_err_detected; /*!< Main Control Channel Max Retranmission error status */
    uint8_t main_ctrl_chnl_retrans_count;   /*!< Main Control Channel Retransmission Count */
    bool gpio_chnl_err_detected;            /*!< GPIO Channel Max Retranmission Error Status */
    uint8_t gpio_chnl_retrans_count;        /*!< GPIO Channel Retransmission Count */
    bool pass_thru_chnl1_err_detected;      /*!< Pass Through Channel-1 Max Retransmission error status */
    uint8_t pass_thru_chnl1_retrans_count;  /*!<  Pass Through Channel-1 Retransmission Count */
    bool pass_thru_chnl2_err_detected;      /*!<  Pass Through Channel-2 Max Retransmission error status*/
    uint8_t pass_thru_chnl2_retrans_count;  /*!<  Pass Through Channel-2 Retransmission Count */
};

/**
 * @struct max96792_pipe_diag_max_retransmission_error_status
 * @brief Structure to hold the maximum retransmission error status for the max96792 deserializer.
 */
struct max96792_link_diag_max_retransmission_error_status
{
    bool max_retrans_err_detected[MAX96792_NUM_OF_GMSL_LINKS];                          /*!<  Master Max Retransmission error status */
    struct max96792_link_max_retrans_data max_retrans_data[MAX96792_NUM_OF_GMSL_LINKS]; /*!< Max Retransmission data for each link */
};

/**
 * @struct max96792_pipe_diag_line_mem_ovrflw_status
 * @brief Structure to hold the line memory overflow status of Pipe Y for the max96792 deserializer.
 */
struct max96792_pipe_diag_line_mem_ovrflw_status
{
    bool line_mem_ovrflw_detected[MAX96792_NUM_OF_VIDEO_PIPES]; /*!< Line Memory Overflow status of PIPE Y */
};

/**
 * @struct struct max96792_pipe_diag_video_block_length_status
 * @brief Structure to hold the video Rx block length error status of Pipe Y for the max96792 deserializer.
 */
struct max96792_pipe_diag_video_block_length_status
{
    bool vid_blk_length_err[MAX96792_NUM_OF_VIDEO_PIPES]; /*!< Video Rx block-length error status of PIPE Y */
};

/**
 * @struct struct max96792_link_diag_eom_status
 * @brief Structure to hold the Eye Opening Monitor error status of Link for the max96792 deserializer.
 */
struct max96792_link_diag_eom_status
{
    bool eom_err[MAX96792_NUM_OF_GMSL_LINKS]; /*!< EOM Status of Link */
};

/**
 * @struct struct max96792_dev_diag_rem_err_flg_status
 * @brief Structure to hold the Remote error flag status for the max96792 deserializer.
 */
struct max96792_dev_diag_rem_err_flg_status
{
    bool rem_cc_en;   /*!< Remote Control Channel Enable Status */
    bool rem_err_flg; /*!< Remote Error Flag Status */
};

/**
 * @struct struct max96792_pipe_diag_crc_err_status
 * @brief Structure to hold the Video Line CRC error status for the max96792 deserializer.
 */
struct max96792_pipe_diag_crc_err_status
{
    bool line_crc_en[MAX96792_NUM_OF_GMSL_LINKS];  /*!< Line CRC Enable Status */
    bool line_crc_err[MAX96792_NUM_OF_GMSL_LINKS]; /*!< Line CRC Error Status */
};

/**
 * @struct max96792_link_enabled_status
 * @brief Structure to hold the link enabled status for all GMSL links.
 */
struct max96792_link_enabled_status
{
    bool link_enabled[MAX96792_NUM_OF_GMSL_LINKS]; /*!< Link Enable Status of all links */
};

/**
 * @struct max96792_pipe_enabled_status
 * @brief Structure to hold the pipe enabled status for all video pipes.
 */
struct max96792_pipe_enabled_status
{
    bool pipe_enabled[MAX96792_NUM_OF_VIDEO_PIPES]; /*!< Pipe Enable Status of all video pipes */
};

extern int max96792_link_diag_get_link_lock_status(struct gmsl_dev *dev, enum max96792_link link, bool *locked);
extern int max96792_dev_diag_get_errb_status(struct gmsl_dev *dev, bool *errb);

extern int max96792_pipe_diag_check_video_lock_status(struct gmsl_dev *dev, void *video_lock_status, bool *diag_err);
extern int max96792_link_diag_check_link_lock_status(struct gmsl_dev *dev, void *link_lock_status, bool *diag_err);
extern int max96792_dev_diag_get_device_id(struct gmsl_dev *dev, void *dev_id_status, bool *diag_err);
extern int max96792_link_diag_check_idle_error_status(struct gmsl_dev *dev, void *idle_err_status, bool *diag_err);
extern int max96792_link_diag_get_line_fault_int_status(struct gmsl_dev *dev, uint8_t *line_fault_int);
extern int max96792_link_diag_get_line_fault_enabled_status(struct gmsl_dev *dev, uint8_t *line_fault_enable_status);
extern int max96792_link_diag_get_lmn_status(struct gmsl_dev *dev, enum max96792_link_lmn lmn, enum max96792_link_diag_lmn_status *line_fault_status);
extern int max96792_link_diag_check_line_fault_status(struct gmsl_dev *dev, void *line_fault_status, bool *diag_err);
extern int max96792_link_diag_check_max_retransmission_error_status(struct gmsl_dev *dev, void *max_retransmission_status, bool *diag_err);
extern int max96792_pipe_diag_check_crc_error(struct gmsl_dev *dev, void *vid_line_crc_status, bool *diag_err);
extern int max96792_pipe_diag_check_stream_id(struct gmsl_dev *dev, void *stream_id_status, bool *diag_err);
extern int max96792_pipe_diag_check_video_block_length_error(struct gmsl_dev *dev, void *vid_block_length_status, bool *diag_err);
extern int max96792_pipe_diag_check_line_memory_overflow(struct gmsl_dev *dev, void *line_mem_ovrflw_status, bool *diag_err);
extern int max96792_dev_diag_check_remote_err_flg(struct gmsl_dev *dev, void *remote_err_status, bool *diag_err);
extern int max96792_link_diag_check_eye_open_monitor_error(struct gmsl_dev *dev, void *eye_open_monitor_status, bool *diag_err);
extern int max96792_csi_diag_check_mipi_status(struct gmsl_dev *dev, void *mipi_status, bool *diag_err);
extern int max96792_dev_diag_get_device_revision(struct gmsl_dev *dev, void *dev_revision_status, bool *diag_err);
extern int max96792_link_diag_check_decode_error_status(struct gmsl_dev *dev, void *decode_err_status, bool *diag_err);
extern int max96792_dev_diag_check_part_config(struct gmsl_dev *dev, void *part_config_status, bool *diag_err);

#endif
