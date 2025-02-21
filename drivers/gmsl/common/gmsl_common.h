/***************************************************************************/
/**
 *   @file   gmsl_common.h
 *   @brief  Header file of gmsl common interface
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

#ifndef GMSL_COMMON_H
#define GMSL_COMMON_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "gmsl_reg_access.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define FLAG_DIAG_ERR(err_cond, current_diag_err, diag_err) {\
    if(err_cond) {\
        diag_err = true;\
        current_diag_err = true;\
    }\
    else {\
        current_diag_err = false;\
    }\
}

#define VALBITFIELD(reg_val, bitfield) \
    ((reg_val) & bitfield##_MASK) >> bitfield##_POS

#ifndef BIT
#define BIT(x)	(1U << (x))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))
#endif

#ifndef BITS_PER_LONG
#define BITS_PER_LONG (64U)
#endif

#ifndef UL
#define UL(v)   v##UL
#endif

#ifndef GENMASK
#define GENMASK(high, low) (((1 << ((high) - (low) + 1)) - 1) << (low))
#endif

#define MASK_0xFF 0xFFU

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
* @enum  gmsl_link
* @brief Enumerates GMSL Camera Deserializer Phy/Link.
*/
enum gmsl_link{
    GMSL_LINK_A = 0U,                 /*!< GMSL Camera Deserializer Link-A */
    GMSL_LINK_B = 1U,                 /*!< GMSL Camera Deserializer Link-B */
    GMSL_LINK_C = 2U,                 /*!< GMSL Camera Deserializer Link-C */
    GMSL_LINK_D = 3U,                 /*!< GMSL Camera Deserializer Link-D */
};
/**
 * @enum gmsl_lf_enable
 * @brief Enumerates the line fault monitor enable options for the GMSL device.
 */
enum gmsl_lf_enable
{
    GMSL_LF0_ENABLE = 0x1u,             /*!< Enable Line fault monitor 0 */
    GMSL_LF1_ENABLE = 0x2u,             /*!< Enable Line fault monitor 1 */
    GMSL_LF2_ENABLE = 0x4u,             /*!< Enable Line fault monitor 2 */
    GMSL_LF3_ENABLE = 0x8u,             /*!< Enable Line fault monitor 3 */
    GMSL_LF_ALL_ENABLE = 0xFu,          /*!< Enable All Line fault monitors */
};

/**
 * @enum gmsl_pipe
 * @brief Enumerates the video pipes for the GMSL device.
 */

enum gmsl_pipe
{
    GMSL_PIPE_X = 0u,        /*!< Video Pipe X */
    GMSL_PIPE_Y = 1u,        /*!< Video Pipe Y */
    GMSL_PIPE_Z = 2u,        /*!< Video Pipe Z */
    GMSL_PIPE_U = 3u,        /*!< Video Pipe U */
};

/**
 * @enum gmsl_link_phy_controller
 * @brief Enumerates the CSI controllers for the GMSL device.
 */
enum gmsl_link_mipi_csi_controller
{
    GMSL_LINK_MIPI_CSI_CTRLR_0 = 0u,        /*!< CSI Controller 0 */
    GMSL_LINK_MIPI_CSI_CTRLR_1 = 1u,        /*!< CSI Controller 1 */
    GMSL_LINK_MIPI_CSI_CTRLR_2 = 2u,        /*!< CSI Controller 2 */
    GMSL_LINK_MIPI_CSI_CTRLR_3 = 3u,        /*!< CSI Controller 3 */
};

/**
 * @enum gmsl_link_mipi_phy
 * @brief Enumerates the MIPI PHYs for the GMSL device.
 */
enum gmsl_link_mipi_phy
{
    GMSL_LINK_MIPI_CSI_PHY_0 = 0u,        /*!< MIPI PHY 0 */
    GMSL_LINK_MIPI_CSI_PHY_1 = 1u,        /*!< MIPI PHY 1 */
    GMSL_LINK_MIPI_CSI_PHY_2 = 2u,        /*!< MIPI PHY 2 */
    GMSL_LINK_MIPI_CSI_PHY_3 = 3u,        /*!< MIPI PHY 3 */
};

/**
 * @enum gmsl_part_num
 * @brief Enumerates the part numbers for the GMSL device.
 */
enum gmsl_part_num
{
    GMSL_PART_NUM_MAX96724 = 0u,        /*!< MAX96724 */
    GMSL_PART_NUM_MAX96714 = 1u,        /*!< MAX96714 */
    GMSL_PART_NUM_MAX96981 = 2u,        /*!< MAX96981 */
    GMSL_PART_NUM_MAX96793 = 3u,        /*!< MAX96793 */
    GMSL_PART_NUM_MAX96762 = 4u,        /*!< MAX96762 */   
};

/**
 * @enum gmsl_link_cfg
 * @brief Enumerates the link configuration options for the GMSL device.
 */
enum gmsl_link_cfg
{
    GMSL_LINK_A_LINK_CFG = 1,
    GMSL_LINK_B_LINK_CFG = 2,
    GMSL_BOTH_LINKS_CFG = 3,
};

/**
 * @struct gmsl_dev
 * @brief  Structure representing a GMSL device handle, containing configuration parameters and diagnostic operations.
 *         This is common device structure for all gmsl devices. It contains device attributes/capabilities.
 */
struct gmsl_dev {
    uint8_t index;                          /*!< GMSL Serializer device index */
    uint16_t part_id;                        /*!< GMSL device id or part id */
    uint8_t rev_id;                         /*!< GMSL Serializer revision id */
    char dev_name[50];                      /*!< Fixed size string holder for driver name */
    struct no_os_i2c_desc *i2c_desc;        /*!< No-os i2c descriptor with device address */
    struct gmsl_dev *linked_devs[4];        /*!< List of handles for linked devices */
    void *dev_cap;                          /*!< Part specific handle for capability */
    void *dev_config;                       /*!< Part specific handle for configuration */
    void *dev_state;                        /*!< Part specific handle for real-time state variables */
    void *dev_diag;                         /*!< Part specific handle for diagnostis handle */
};

/**
 * @struct gmsl_link_cable_type
 * @brief  Enumerates link cable type indices.
 */
enum gmsl_link_cable_type{
    GMSL_LINK_CABLE_TYPE_STP = 0U,
    GMSL_LINK_CABLE_TYPE_COAX = 1U,
} ;

/**
 * @enum  gmsl_link_phy_rate
 * @brief Enumerates PHY rate of SerDes.
 */
enum gmsl_link_phy_rate{
    GMSL_LINK_PHY_RATE_187M5  = 0U,      /*!< GMSL PHY Rate 187.5 Mbps */
    GMSL_LINK_PHY_RATE_3G = 1U,      	 /*!< GMSL PHY Rate 3 Gbps */
    GMSL_LINK_PHY_RATE_6G = 2U,          /*!< GMSL PHY Rate 6 Gbps */
    GMSL_LINK_PHY_RATE_12G = 3U,         /*!< GMSL PHY Rate 12 Gbps */
};

/** 
 * @enum  gmsl_link_mode
 * @brief Enumerates GMSL link mode.
 */
enum gmsl_link_mode{
    GMSL_LINK_MODE_GMSL2_NRZ = 0U,      		/*!< GMSL Link Mode GMSL2 */
    GMSL_LINK_MODE_GMSL3_PAM4 = 1U,     		/*!< GMSL Link Mode GMSL3 */
};

/**
 * @struct gmsl_link_fec_cfg
 * @brief Structure for configuring GMSL fec settings.
 */
struct gmsl_link_fec_cfg {
    enum gmsl_link link;  /*!< GMSL FEC link */
    bool enable;          /*!< GMSL FEC enable/disable */
    bool crc_enable;      /*!< GMSL CRC enable/disable */
};

/**
 * @struct gmsl_link_cable_type_cfg
 * @brief Structure for configuring GMSL tx-link rate settings.
 */
struct gmsl_link_cable_type_cfg {
    enum gmsl_link link;              /*!< GMSL Link */
    enum gmsl_link_cable_type type;   /*!< GMSL Link Cable Type */
};

/**
 * @struct gmsl_link_rate_cfg
 * @brief Structure for configuring GMSL tx-link rate settings.
 */
struct gmsl_link_rate_cfg {
    enum gmsl_link link;               /*!< GMSL Link */
    enum gmsl_link_phy_rate tx_rate;   /*!< GMSL Link TxRate */
};

/**
 * @struct gmsl_link_mode_cfg
 * @brief Structure for configuring GMSL  settings.
 */
struct gmsl_link_mode_cfg {
    enum gmsl_link link;          /*!< GMSL Link */
    enum gmsl_link_mode mode;     /*!< GMSL Link Mode */
};

/**
 * @struct gmsl_link_enable_cfg
 * @brief Structure for configuring GMSL  settings.
 */
struct gmsl_link_enable_cfg {
    enum gmsl_link link;      /*!< GMSL Link */
    bool enable;              /*!< GMSL Link Mode */
};

/**
 * @struct gmsl_dev_diag_ops
 * @brief A structure that contains diagnostic operation function pointers for a GMSL device.
 * 
 * @details This structure is used to define a set of function pointers for various diagnostic operations
 * that can be performed on a GMSL device. Each function pointer corresponds to a specific diagnostic operation, 
 * allowing for flexible and modular diagnostic implementations. This structure is a super-set of all diagnostic
 * operations and not all operations are supported by each part.
 */
struct gmsl_dev_diag_ops {
    int32_t (*diag_check_device_id)(struct gmsl_dev *dev, void *dev_id, bool *diag_err);                                          /* GMSL_DIAG_PART_DEV_ID */
    int32_t (*diag_check_device_rev)(struct gmsl_dev *dev, void *dev_rev, bool *diag_err);                                        /* GMSL_DIAG_PART_DEV_REV */
    int32_t (*diag_check_part_config)(struct gmsl_dev *dev, void *part_cfg_sts, bool *diag_err);                                  /* GMSL_DIAG_PART_CFG */
    int32_t (*diag_check_link_lock_status)(struct gmsl_dev *dev, void *link_lock_status, bool *diag_err);                         /* GMSL_DIAG_LINK_LOCK */
    int32_t (*diag_check_decode_error_status)(struct gmsl_dev *dev, void *decode_err_status, bool *diag_err);                     /* GMSL_DIAG_LINK_DECODE */
    int32_t (*diag_check_idle_err_status)(struct gmsl_dev *dev, void *idle_err_sts, bool *diag_err);                              /* GMSL_DIAG_LINK_IDLE */
    int32_t (*diag_check_max_retrans_status)(struct gmsl_dev *dev, void *max_retrans_sts, bool *diag_err);                        /* GMSL_DIAG_LINK_MAX_RETRANS */
    int32_t (*diag_check_line_fault_status)(struct gmsl_dev *dev, void *line_fault_status, bool *diag_err);                       /* GMSL_DIAG_LINK_LINE_FAULT */
    int32_t (*diag_check_fec_status)(struct gmsl_dev *dev, void *fec_status, bool *diag_err);                                     /* GMSL_DIAG_LINK_FEC */
    int32_t (*diag_check_eom_status)(struct gmsl_dev *dev, void *eom_status, bool *diag_err);                                     /* GMSL_DIAG_LINK_EOM */
    int32_t (*diag_check_overflow_status)(struct gmsl_dev *dev, void *vid_tx_overflow_sts, bool *diag_err);                       /* GMSL_DIAG_VIDEO_OVERFLOW */
    int32_t (*diag_check_pclk_lost_status)(struct gmsl_dev *dev, void *pclk_det_sts, bool *diag_err);                             /* GMSL_DIAG_VIDEO_PCLK_LOST */
    int32_t (*diag_check_drift_status)(struct gmsl_dev *dev, void *vid_tx_pclk_drift_err_sts, bool *diag_err);                    /* GMSL_DIAG_VIDEO_DRIFT */
    int32_t (*diag_check_get_is_video_line_crc_enabled)(struct gmsl_dev *dev, void *vid_line_crc_enabled_sts, bool *diag_err);    /* GMSL_DIAG_VIDEO_CRC */
    int32_t (*diag_check_stream_id_status)(struct gmsl_dev *dev, void *stream_id_sts, bool *diag_err);                            /* GMSL_DIAG_VIDEO_STREAM_ID */
    int32_t (*diag_check_parameter_status)(struct gmsl_dev *dev, void *vid_param_sts, bool *diag_err);                            /* GMSL_DIAG_VIDEO_PARAMETER */
    int32_t (*diag_check_pps_status)(struct gmsl_dev *dev, void *pps_value_sts, bool *diag_err);                                  /* GMSL_DIAG_VIDEO_PPS */
    int32_t (*diag_check_video_stream_dsc_compression)(struct gmsl_dev *dev, void *vid_stream_dsc_comp_sts, bool *diag_err);      /* GMSL_DIAG_VIDEO_STREAM_DSC_COMPRESSION */
    int32_t (*diag_check_dp_fec_support)(struct gmsl_dev *dev, void *pb_is_fec_running, bool *diag_err);                          /* GMSL_DIAG_DP_FEC_SUPPORT */
    int32_t (*diag_check_hdcp_status)(struct gmsl_dev *dev, void *hdcp_status, bool *diag_err);                                   /* GMSL_DIAG_HDCP_STATUS */
    int32_t (*diag_check_link_training_status)(struct gmsl_dev *dev, void *link_training_sts, bool *diag_err);                    /* GMSL_DIAG_LINK_TRAINING */
    int32_t (*diag_check_mipi_rx_err_status)(struct gmsl_dev *dev, void *mipi_rx_err_status, bool *diag_err);                     /* GMSL_DIAG_MIPI_RX */
    int32_t (*diag_check_mem_ecc_2b_err_status)(struct gmsl_dev *dev, void *mem_ecc_2b_err_status, bool *diag_err);               /* GMSL_DIAG_MEM_ECC_2BIT_ERROR */
    int32_t (*diag_check_get_is_video_line_crc_error_status)(struct gmsl_dev* dev, void* vid_line_crc_error_sts, bool* diag_err); /* GMSL_DIAG_VIDEO_CRC_ERROR_STATUS */
    int32_t (*diag_check_video_error_status)(struct gmsl_dev* dev, void* vid_error_sts, bool* diag_err);                          /* GMSL_VIDEO_ERROR STATUS*/
    int32_t (*diag_check_programmable_state_machine_error_status)(struct gmsl_dev* dev, void* psm_error_sts, bool* diag_err);     /* GMSL_PSM_ERROR STATUS*/
    int32_t (*diag_check_video_lock_status)(struct gmsl_dev *dev, void *video_lock_sts, bool *diag_err);                          /* GMSL_DIAG_VIDEO_LOCK */
    int32_t (*diag_check_mipi_status)(struct gmsl_dev* dev, void* mipi_sts, bool* diag_err);                                      /* GMSL_DIAG_MIPI_STATUS */
    int32_t (*diag_check_phy_low_power_err_status)(struct gmsl_dev* dev, void* lp_err_status, bool* diag_err);                    /* GMSL_DIAG_PHY_LOW_POWER_ERR */
    int32_t (*diag_check_lmo_status)(struct gmsl_dev* dev, void* lmo_sts, bool* diag_err);                                        /* GMSL_DIAG_LMO_STATUS */
    int32_t (*diag_check_video_timing_detect_status)(struct gmsl_dev* dev, void* vid_timing_sts, bool* diag_err);                 /* GMSL_DIAG_VIDEO_TIMING */
    int32_t(*diag_check_get_dp_link_rate)(struct gmsl_dev* dev, void* dp_link_rate, bool* diag_err);                              /* GMSL_DIAG_DP_LINK_RATE */
    int32_t(*diag_check_get_dp_lane_count)(struct gmsl_dev* dev, void* dp_lane_count, bool* diag_err);                            /* GMSL_DIAG_DP_LANE_COUNT */
    int32_t(*diag_check_remote_err)(struct gmsl_dev* dev, void* rom_err_flg, bool* diag_err);                                     /* GMSL_DIAG_REMOTE_ERROR_FLAG */
    int32_t(*diag_check_video_block_length_err)(struct gmsl_dev* dev, void* video_block_length_err, bool* diag_err);              /* GMSL_DIAG_VID_BLK_LENGTH_ERROR */
    int32_t(*diag_check_infofr_cc_status)(struct gmsl_dev* dev, void* infofr_cc_sts, bool* diag_err);                             /* GMSL_DIAG_INFOFR_CC */
};

#endif /* GMSL_COMMON_H */
