/***************************************************************************//**
 *   @file   max96793_diag.h
 *   @brief  Header file of Diagnostics Driver Implementation of MAX96793 Serializer.
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

#ifndef MAX96793_DIAG_H
#define MAX96793_DIAG_H

#include "max96793.h"
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "gmsl_common.h"
/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MAX96793_CSI_DIAG_NUM_OF_MIPI_PKT_READ              (4U)    /*!< Number of iterations to read MIPI packets */
#define MAX96793_CSI_DIAG_MIPI_PKT_READ_DELAY               (10U)   /*!< Delay in milliseconds to read the next MIPI packet */
#define MAX96793_MAX_LANE_COUNT                             (4U)    /*!< Maximum number of lanes */
#define MAX96793_MAX_VIDEO_STREAMS                          (16U)   /*!< Maximum number of video streams */
#define MAX96793_MAX_VIRTUAL_CHANNEL                        (16U)   /*!< Maximum number of virtual channels */

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 *  @enum  max96793_link_diag_lmn_status
 *  @brief Line fault monitor status.
 */
enum max96793_link_diag_lmn_status {
	MAX96793_LINK_DIAG_LMN_SHORT_TO_BAT = 0u,        /*!< Short to battery */
	MAX96793_LINK_DIAG_LMN_SHORT_TO_GND = 1u,        /*!< Short to GND */
	MAX96793_LINK_DIAG_LMN_NORMAL = 2u,              /*!< Normal operation */
	MAX96793_LINK_DIAG_LMN_LINE_OPEN = 3u,           /*!< Line open */
	MAX96793_LINK_DIAG_LMN_LINE_TO_LINE_SHORT = 4u   /*!< Line to line short */
};

/**
 *  @enum  max96793_link_diag_lmn
 *  @brief Line fault monitor number.
 */
enum max96793_link_diag_lmn {
	MAX96793_LINK_LMN_0 = 0u,             /*!< Line fault monitor 0 */
	MAX96793_LINK_LMN_1 = 1u,             /*!< Line fault monitor 1 */
};

/**
 *  @enum  max96793_sensor_lane
 *  @brief Sensor lane number.
 */
enum max96793_sensor_lane {
	MAX96793_SENSOR_LANE_0 = 0u,          /*!< Sensor lane 0 */
	MAX96793_SENSOR_LANE_1 = 1u,          /*!< Sensor lane 1 */
	MAX96793_SENSOR_LANE_2 = 2u,          /*!< Sensor lane 2 */
	MAX96793_SENSOR_LANE_3 = 3u           /*!< Sensor lane 3 */
};

/**
 *  @enum  max96793_serializer_lane
 *  @brief Serializer lane number.
 */
enum max96793_serializer_lane {
	MAX96793_SERIALIZER_LANE_0 = 0u,      /*!< Serializer lane 0 */
	MAX96793_SERIALIZER_LANE_1 = 1u,      /*!< Serializer lane 1 */
	MAX96793_SERIALIZER_LANE_2 = 2u,      /*!< Serializer lane 2 */
	MAX96793_SERIALIZER_LANE_3 = 3u       /*!< Serializer lane 3 */
};

/**
 *  @enum  max96793_lane_polarity
 *  @brief Lane polarity.
 */
enum max96793_lane_polarity {
	MAX96793_LANE_POLARITY_NORMAL = 0u,   /*!< Lane polarity normal */
	MAX96793_LANE_POLARITY_INVERSE = 1u   /*!< Lane polarity inverse */
};

/**
 * @enum  MAX96793_DATA_TYPE
 * @brief DATA Type for MAX96793 device.
 */
enum max96793_data_type {
	MAX96793_DATA_TYPE_DEFAULT      = 0x0u,           /*!< Default */
	MAX96793_DATA_TYPE_GENERIC8_A   = 0x10u,          /*!< GENERIC8 */
	MAX96793_DATA_TYPE_GENERIC8_B   = 0x11u,          /*!< GENERIC8 */
	MAX96793_DATA_TYPE_EMB8         = 0x12u,          /*!< EMBEDDED */
	MAX96793_DATA_TYPE_YUV422_8BIT  = 0x1Eu,          /*!< YUV422 8-bit */
	MAX96793_DATA_TYPE_YUV422_10BIT = 0x1Fu,          /*!< YUV422 10-bit */
	MAX96793_DATA_TYPE_RGB565       = 0x22u,          /*!< RGB565 */
	MAX96793_DATA_TYPE_RGB666       = 0x23u,          /*!< RGB666 */
	MAX96793_DATA_TYPE_RGB888       = 0x24u,          /*!< RGB888 */
	MAX96793_DATA_TYPE_RAW8         = 0x2Au,          /*!< RAW8 */
	MAX96793_DATA_TYPE_RAW10        = 0x2Bu,          /*!< RAW10 */
	MAX96793_DATA_TYPE_RAW12        = 0x2Cu,          /*!< RAW12 */
	MAX96793_DATA_TYPE_RAW14        = 0x2Du,          /*!< RAW14 */
	MAX96793_DATA_TYPE_RAW16        = 0x2Eu,          /*!< RAW16 */
	MAX96793_DATA_TYPE_RAW20        = 0x2Fu,          /*!< RAW20 */
	MAX96793_DATA_TYPE_YUV422_12BIT = 0x30u,          /*!< YUV422 12-bit */
	MAX96793_DATA_TYPE_UDP8_A       = 0x31u,          /*!< UDP8 */
	MAX96793_DATA_TYPE_UDP8_B       = 0x32u,          /*!< UDP8 */
	MAX96793_DATA_TYPE_UDP8_C       = 0x33u,          /*!< UDP8 */
	MAX96793_DATA_TYPE_UDP8_D       = 0x34u,          /*!< UDP8 */
	MAX96793_DATA_TYPE_UDP8_E       = 0x35u,          /*!< UDP8 */
	MAX96793_DATA_TYPE_UDP8_F       = 0x36u,          /*!< UDP8 */
	MAX96793_DATA_TYPE_UDP8_G       = 0x37u           /*!< UDP8 */
};


/**
 * @enum  max96793_video_stream
 * @brief Enumerates video streams for MAX96793 device.
 */
enum max96793_video_stream {
	MAX96793_VID_STREAM_0 = 0u,                    /*!< Video Stream index 0 */
	MAX96793_VID_STREAM_1 = 1u,                    /*!< Video Stream index 1 */
	MAX96793_VID_STREAM_2 = 2u,                    /*!< Video Stream index 2 */
	MAX96793_VID_STREAM_3 = 3u,                    /*!< Video Stream index 3 */
	MAX96793_VID_STREAM_4 = 4u,                    /*!< Video Stream index 4 */
	MAX96793_VID_STREAM_5 = 5u,                    /*!< Video Stream index 5 */
	MAX96793_VID_STREAM_6 = 6u,                    /*!< Video Stream index 6 */
	MAX96793_VID_STREAM_7 = 7u,                    /*!< Video Stream index 7 */
	MAX96793_VID_STREAM_8 = 8u,                    /*!< Video Stream index 8 */
	MAX96793_VID_STREAM_9 = 9u,                    /*!< Video Stream index 9 */
	MAX96793_VID_STREAM_10 = 10u,                  /*!< Video Stream index 10 */
	MAX96793_VID_STREAM_11 = 11u,                  /*!< Video Stream index 11 */
	MAX96793_VID_STREAM_12 = 12u,                  /*!< Video Stream index 12 */
	MAX96793_VID_STREAM_13 = 13u,                  /*!< Video Stream index 13 */
	MAX96793_VID_STREAM_14 = 14u,                  /*!< Video Stream index 14 */
	MAX96793_VID_STREAM_15 = 15u                   /*!< Video Stream index 15 */
};

/**
 * @enum  max96793_port
 * @brief Enumerates ports for MAX96793 device.
 */
enum max96793_port {
	MAX96793_PORT_B = 1u            /*!< Port B */
};

/**
 * @enum  max96793_data_type_size
 * @brief Enumerates data type size for MAX96793 device.
 */
enum max96793_data_type_size {
	MAX96793_DT_SIZE_0      = 0U,   /*!< Data type size 0 */
	MAX96793_DT_SIZE_8      = 8U,   /*!< Data type size 8 */
	MAX96793_DT_SIZE_10     = 10U,  /*!< Data type size 10 */
	MAX96793_DT_SIZE_12     = 12U,  /*!< Data type size 12 */
	MAX96793_DT_SIZE_14     = 14U,  /*!< Data type size 14 */
	MAX96793_DT_SIZE_16     = 16U,  /*!< Data type size 16 */
	MAX96793_DT_SIZE_18     = 18U,  /*!< Data type size 18 */
	MAX96793_DT_SIZE_20     = 20U,  /*!< Data type size 20 */
	MAX96793_DT_SIZE_24     = 24U,  /*!< Data type size 24 */
};

/**
 * @struct max96793_dev_diag_device_id_status
 * @brief Structure holding the device ID status of the MAX96793 device.
 */
struct max96793_dev_diag_device_id_status {
	uint8_t dev_id;             /*!< Device ID */
};

/**
 * @struct max96793_dev_diag_device_revision_status
 * @brief Structure holding the device revision status of the MAX96793 device.
 */
struct max96793_dev_diag_device_revision_status {
	uint8_t dev_rev;            /*!< Device revision */
};

/**
 * @struct max96793_pipe_diag_decode_error_status
 * @brief Structure holding the decode error status of the MAX96793 device.
 */
struct max96793_pipe_diag_decode_error_status {
	bool decode_err_link;        /*!< Decode error status */
	uint8_t decode_err_cnt;      /*!< Decode error count */
};

/**
 * @struct max96793_pipe_diag_idle_error_status
 * @brief Structure holding the idle error status of the MAX96793 device.
 */
struct max96793_pipe_diag_idle_error_status {
	bool idle_err_detected;      /*!< Idle error status */
	uint8_t idle_err_cnt;        /*!< Idle error count */
};

/**
 * @struct max96793_link_diag_link_lock_status
 * @brief Structure holding the link lock status of the MAX96793 device.
 */
struct max96793_link_diag_link_lock_status {
	bool link_locked;           /*!< Link A lock status */
};

/**
 * @struct max96793_pipe_diag_max_retrans_data
 * @brief Structure holding the Max Retransmission Data.
 */
struct max96793_pipe_max_retrans_data {
	bool spi_chnl_retrans_err_detected;         /*!< SPI channel Max Retransmission error status */
	uint8_t spi_chnl_retrans_count;             /*!< SPI channel Max Retransmission count */
	bool gpio_chnl_retrans_err_detected;        /*!< GPIO channel Max Retransmission error status */
	uint8_t gpio_chnl_retrans_count;            /*!< GPIO channel Max Retransmission count */
	bool pass_thru_chnl1_retrans_err_detected;  /*!< Pass through channel 1 Max Retransmission error status */
	uint8_t pass_thru_chnl1_retrans_count;      /*!< Pass through channel 1 Max Retransmission count */
	bool pass_thru_chnl2_retrans_err_detected;  /*!< Pass through channel 2 Max Retransmission error status */
	uint8_t pass_thru_chnl2_retrans_count;      /*!< Pass through channel 2 Max Retransmission count */
};

/**
 * @struct max96793_pipe_diag_max_retransmission_error_status
 * @brief Structure holding the Max Retransmission Error Status.
 */
struct max96793_pipe_diag_max_retransmission_error_status {
	bool max_retrans_err_detected;                                        /*!< Master Max Retransmission error status */
	struct max96793_pipe_max_retrans_data
		max_retrans_data;               /*!< Max Retransmission data for link A */
};

/**
 * @struct max96793_link_diag_line_fault_status
 * @brief Structure holding the line fault status of the MAX96793 device.
 */
struct max96793_link_diag_line_fault_status {
	bool lf_int;                                                           /*!< Line fault interrupt status */
	enum max96793_link_diag_lmn_status
	lmn_err_status[MAX96793_LMN_COUNT]; /*!< Line fault monitor status */
};

/**
 * @struct max96793_pipe_diag_vid_tx_overflow_status
 * @brief Structure holding the video transmit overflow status of the MAX96793 device.
 */
struct max96793_csi_diag_pclkdet_status {
	bool pclk_detected;                               /*!< Pixel clock detected status */
};

/**
 * @struct max96793_pipe_diag_vid_tx_overflow_status
 * @brief Structure holding the video transmit overflow status of the MAX96793 device.
 */
struct max96793_pipe_diag_vid_tx_overflow_status {
	bool vid_tx_overflowed;                           /*!< Video transmit overflow status */
};

/**
 * @struct max96793_pipe_diag_vid_tx_pclk_drift_err_status
 * @brief Structure holding the video transmit pixel clock drift error status of the MAX96793 device.
 */
struct max96793_pipe_diag_vid_tx_pclk_drift_err_status {
	bool vid_tx_pclk_drift_err_detected;              /*!< Video transmit pixel clock drift error status */
};

/**
 * @struct max96793_mem_diag_uncorrectable_2b_mem_ecc_err_status
 * @brief Structure holding the uncorrectable 2-bit memory ECC error status of the MAX96793 device.
 */
struct max96793_mem_diag_uncorrectable_2b_mem_ecc_err_status {
	bool uncorrectable_2b_mem_ecc_err_detected;       /*!< Uncorrectable 2-bit memory ECC error status */
};

/**
 * @struct max96793_csi_diag_mipi_phy_err_details
 * @brief Structure holding the MIPI PHY error details of the MAX96793 device.
 */
struct max96793_csi_diag_mipi_phy_err_details {
	bool phy_hsync_lane0_1b_err_detected;               /*!< PHY HSYNC Lane 0 1-bit error status */
	bool phy_hsync_lane1_1b_err_detected;               /*!< PHY HSYNC Lane 1 1-bit error status */
	bool phy_hsync_lane0_2b_err_detected;               /*!< PHY HSYNC Lane 0 2-bit error status */
	bool phy_hsync_lane1_2b_err_detected;               /*!< PHY HSYNC Lane 1 2-bit error status */
	bool phy_hispeed_rx_skew_calib_lane0_err_detected;  /*!< PHY High Speed RX Skew Calibration Lane 0 error status */
	bool phy_hispeed_rx_skew_calib_lane1_err_detected;  /*!< PHY High Speed RX Skew Calibration Lane 1 error status */
};

/**
 * @struct max96793_csi_diag_mipi_ctrl_err_details
 * @brief Structure holding the MIPI controller error details of the MAX96793 device.
 */
struct max96793_csi_diag_mipi_ctrl_err_details {
	bool csi_ctrl_1b_ecc_err_detected;                   /*!< CSI controller 1-bit ECC error status */
	bool csi_ctrl_2b_ecc_err_detected;                   /*!< CSI controller 2-bit ECC error status */
	bool csi_ctrl_crc_err_detected;                      /*!< CSI controller CRC error status */
	bool csi_ctrl_pkt_terminated_early_err_detected;     /*!< CSI controller packet terminated early error status */
	bool csi_ctrl_frame_cnt_err_detected;                /*!< CSI controller frame count error status */
};

/**
 * @struct max96793_csi_diag_mipi_rx_err_status
 * @brief Structure holding the MIPI RX error status of the MAX96793 device.
 */
struct max96793_csi_diag_mipi_rx_err_status {
	bool mipi_rx_err_detected;                                                                   /*!< MIPI RX error status */
	struct max96793_csi_diag_mipi_phy_err_details
		mipi_phy_err_details[MAX96793_MIPI_PHY_COUNT]; /*!< MIPI PHY error details */
	struct max96793_csi_diag_mipi_ctrl_err_details
		mipi_ctrl_err_details;                        /*!< MIPI controller error details */
};

/**
 * @struct max96793_link_diag_phy_low_power_err_status_details
 * @brief Structure holding the low power error status details of the MAX96793 device.
 */
struct max96793_link_diag_phy_low_power_err_status_details {
	bool unrecgnizd_esc_cmd_rcvd_on_d0;     /*!< Unrecognized escape command received on D0 */
	bool unrecgnizd_esc_cmd_rcvd_on_clk;    /*!< Unrecognized escape command received on CLK */
	bool invalid_line_seq_on_d0;            /*!< Invalid line sequence detected on D0 */
	bool invalid_line_seq_on_d1;            /*!< Invalid line sequence detected on D1 */
	bool invalid_line_seq_on_clk;           /*!< Invalid line sequence detected on CLK */
};

/**
 * @struct max96793_link_diag_phy_low_power_err_status
 * @brief Structure holding the low power error status of the MAX96793 device.
 */
struct max96793_link_diag_phy_low_power_err_status {
	struct max96793_link_diag_phy_low_power_err_status_details
		lp_err_sts_details[MAX96793_MIPI_PHY_COUNT]; /*!< Low power error status details */
};

/**
 * @struct max96793_csi_diag_mipi_pkt_cnt_status
 * @brief Structure holding the MIPI packet count status of the MAX96793 device.
 */
struct max96793_csi_diag_mipi_pkt_cnt_status {
	bool mipi_pkt_cnt_err_detected;                                 /*!< MIPI packet count error status */
	bool tunnel_mode_enabled;                                       /*!< Tunnel mode enabled status */
	uint8_t phy_pkt_cnt[MAX96793_CSI_DIAG_NUM_OF_MIPI_PKT_READ];    /*!< MIPI PHY packet count */
	uint8_t csi1_pkt_cnt[MAX96793_CSI_DIAG_NUM_OF_MIPI_PKT_READ];   /*!< MIPI CSI1 packet count */
	uint8_t tun_pkt_cnt[MAX96793_CSI_DIAG_NUM_OF_MIPI_PKT_READ];    /*!< MIPI tunnel mde packet count */
	uint8_t phy_clk_cnt[MAX96793_CSI_DIAG_NUM_OF_MIPI_PKT_READ];    /*!< MIPI clock lane packet count */
};

/**
 * @struct max96793_pipe_diag_stream_id_status
 * @brief Structure holding the stream ID status of the MAX96793 device.
 */
struct max96793_pipe_diag_stream_id_status {
	bool pipe_enabled;        /*!< Pipe enabled status */
	uint8_t stream_id;        /*!< Stream ID for video pipe. */
};

/**
 * @struct max96793_link_diag_eom_status
 * @brief Structure holding the Eye Open Monitor (EOM) status of the MAX96793 device.
 */
struct max96793_link_diag_eom_status {
	bool eom_error;         /*!< EOM error status */
};

/**
 * @struct struct max96793_dev_diag_rem_err_flg_status
 * @brief Structure to hold the Remote error flag status for the MAX96793 device.
 */
struct max96793_dev_diag_rem_err_flg_status {
	bool is_remote_cc_disabled;                            /*!< Remote Control Channel Disabled Status */
	bool remote_error;                                     /*!< Remote Error Status */
};

/**
 * @struct max96793_csi_diag_fec_active_status
 * @brief Structure to hold the Forward Error Correction (FEC) active status for the MAX96793 device.
 */
struct max96793_csi_diag_fec_active_status {
	bool is_remote_cc_disabled;                            /*!< Remote Control Channel Disabled Status */
	bool is_fec_enabled;                                   /*!< Forward Error Correction (FEC) Enabled Status */
	bool is_fec_active;                                    /*!< Remote Error Status */
};

/**
 * @struct max96793_csi_video_stream_config
 * @brief Structure holding the video stream configuration of the MAX96793 device.
 */
struct max96793_csi_video_stream_config {
	bool is_virtual_channel_enabled;        /*!< Virtual channel enabled status */
	bool is_data_type_enabled;              /*!< Datatype enabled status*/
	enum max96793_data_type
	data_type;      /*!< Datatype selected for virtual channel */
};

/**
 * @struct max96793_lane_config_status
 * @brief Structure holding the lane configuration status of the MAX96793 device.
 */
struct max96793_lane_config_status {
	uint8_t lane_count;                     /*!< Lane Count */
	enum max96793_sensor_lane
	sensor_lane[MAX96793_MAX_LANE_COUNT];               /*!< Sensor Lane mapped to each serializer lane */
	enum max96793_lane_polarity
	data_lane_polarity[MAX96793_MAX_LANE_COUNT];      /*!< Lane Polarity of each data lane of serializer */
	enum max96793_lane_polarity
	clk_lane_polarity;                                /*!< Lane Polarity of clock lane of serializer */
};

/**
 * @struct max96793_pipe_diag_config_status
 * @brief Structure holding the pipe configuration status of the MAX96793 device.
 */
struct max96793_pipe_diag_config_status {
	uint8_t phy_config;                                                  /*!< PHY Configuration */
	enum max96793_port
	port_selected;                                    /*!< Port selected */
	bool is_port_enabled;                                                /*!< Port enabled status */
	bool is_video_started_in_port;                                       /*!< Video started in port status */
	struct max96793_lane_config_status
		lane_config_status;               /*!< Lane Configuration Status */
};

/**
 * @struct max96793_csi_diag_part_config_status
 * @brief Structure holding the CSI configuration status of the MAX96793 device.
 */
struct max96793_csi_diag_part_config_status {
	bool tunnel_mode_enabled;                                                                    /*!< Tunnel Mode Enabled Status */
	enum max96793_data_type_size
	soft_bpp_override;                                              /*!< Soft BPP override value */
	bool is_soft_bpp_enabled;                                                                    /*!< Soft BPP enabled status */
	bool is_pipe_enabled;                                                                        /*!< Pipe enabled status */
	struct max96793_csi_video_stream_config
		video_stream_config[MAX96793_MAX_VIDEO_STREAMS];     /*!< Configuration for each video stream */
	struct max96793_pipe_diag_stream_id_status
		stream_id_status;                                 /*!< Stream ID Status */
	bool is_independent_mode_enabled;                                                            /*!< Independent Video Stream Mode Enabled Status */
	bool is_pixel_doubling_enabled;                                                              /*!< Pixel Doubling Enabled Status */
};

/**
 * @struct max96793_dev_diag_part_config_status
 * @brief Structure holding the device configuration status of the MAX96793 device.
 */
struct max96793_dev_diag_part_config_status {
	struct max96793_pipe_diag_config_status
		pipe_config_status;                          /*!< Pipe Configuration Status */
	struct max96793_csi_diag_part_config_status
		csi_config_status;                       /*!< CSI Configuration Status */
};

extern int max96793_dev_diag_get_device_id(struct gmsl_dev *dev,
		void *dev_id_status, bool *diag_err);
extern int max96793_dev_diag_check_device_rev(struct gmsl_dev *dev,
		void *dev_revision_status, bool *diag_err);
extern int max96793_dev_diag_check_part_config_status(struct gmsl_dev *dev,
		void *part_config_status, bool *diag_err);
extern int max96793_link_diag_check_decode_error_status(struct gmsl_dev *dev,
		void *decode_err_status, bool *diag_err);
extern int max96793_link_diag_check_idle_error_status(struct gmsl_dev *dev,
		void *idle_err_status, bool *diag_err);
extern int max96793_link_diag_check_link_lock_status(struct gmsl_dev *dev,
		void *link_lock_status, bool *diag_err);
extern int max96793_link_diag_check_max_retransmission_error_status(
	struct gmsl_dev *dev, void *max_retransmission_status, bool *diag_err);
extern int max96793_csi_diag_check_mipi_rx_err_status(struct gmsl_dev *dev,
		void *mipi_rx_err_status, bool *diag_err);
extern int max96793_csi_diag_check_vid_tx_pclk_drift_err_status(
	struct gmsl_dev *dev, void *vid_tx_pclk_drift_status, bool *diag_err);
extern int max96793_link_diag_check_line_fault_status(struct gmsl_dev *dev,
		void *line_fault_status, bool *diag_err);
extern int max96793_csi_diag_check_pclk_detect_status(struct gmsl_dev *dev,
		void *pclk_detect_status, bool *diag_err);
extern int max96793_pipe_diag_check_vid_tx_overflow_status(struct gmsl_dev *dev,
		void *vid_tx_ovrflw_status, bool *diag_err);
extern int max96793_mem_diag_check_uncorrectable_2b_mem_ecc_err_status(
	struct gmsl_dev *dev, void *uncorrectable_2b_mem_ecc_err_status,
	bool *diag_err);
extern int max96793_link_diag_check_phy_low_power_state_error_status(
	struct gmsl_dev *dev, void *lp_err_status, bool *diag_err);
extern int max96793_csi_diag_check_mipi_phy_pkt_cnt_status(struct gmsl_dev *dev,
		void *mipi_pkt_cnt_err_status, bool *diag_err);
extern int max96793_pipe_diag_check_stream_id(struct gmsl_dev *dev,
		void *stream_id_status, bool *diag_err);
extern int max96793_link_diag_check_eye_open_monitor_status(
	struct gmsl_dev *dev, void* eom_status, bool *diag_err);
extern int max96793_csi_diag_check_fec_status(struct gmsl_dev *dev,
		void *fec_active_status, bool *diag_err);
extern int max96793_dev_diag_check_remote_error_status(struct gmsl_dev *dev,
		void* rem_err_status, bool *diag_err);
extern int max96793_link_diag_get_link_lock_status(struct gmsl_dev *dev,
		bool *locked);
extern int max96793_dev_diag_get_errb_status(struct gmsl_dev *dev, bool *errb);
#endif

