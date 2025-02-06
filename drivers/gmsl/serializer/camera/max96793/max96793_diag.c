/***************************************************************************//**
 *   @file   max96793_diag.c
 *   @brief  Diagnostics Driver Implementation of MAX96793 GMSL3 Serializer.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "max96793_diag.h"
#include "gmsl_dbg.h"
#include "gmsl_cam_ser.h"
#include "gmsl_reg_access.h"
#include "no_os_error.h"
#include "max96793_regs.h"
#include "max96793.h"
#include "no_os_delay.h"

/** \addtogroup ADI_GMSL_CAM_SERIALIZER GMSL Camera Serializers
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_SER2 MAX96793
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_SER2_DIAG_API Diagnostic APIs
 *  @{
 */

#define ASSERT_DETECT_STR(b)   (b == 1 ? "Detected" : "Not Detected")

#define MAX96793_PHY_1x4_MODE   (0x0u)
#define MAX96793_PORT_B         (0x1u)

#ifndef ADI_DISABLE_DOCUMENTATION

/*================================== STATIC ==================================*/

/**
 * @brief Get the enabled status of line fault detection for the MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param line_fault_enable_status Pointer to a variable where the line fault enable status will be stored.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_link_diag_get_line_fault_enabled_status(struct gmsl_dev *dev, bool* line_fault_enable_status)
{
    uint8_t ret;
    uint8_t data;

    REG_READ_RETURN_ON_ERR(ret, dev->i2c_desc, DEV_REG5_ADDR, (PU_LF0_DEV_REG5_MASK | PU_LF1_DEV_REG5_MASK), &data);

    line_fault_enable_status[MAX96793_LINK_LMN_0] = ((data & PU_LF0_DEV_REG5_MASK) >> PU_LF0_DEV_REG5_POS);
    line_fault_enable_status[MAX96793_LINK_LMN_1] = ((data & PU_LF1_DEV_REG5_MASK) >> PU_LF1_DEV_REG5_POS);

    return 0;
}

/**
 * @brief Get the link diagnostic status for a specified LMN.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param lmn The LMN link to get the diagnostic status for.
 * @param line_fault_status Pointer to store the retrieved diagnostic status.
 *
 * @return 0 on success, negative error code in case of PAL errors.
 */
static int max96793_link_diag_get_line_fault_status(struct gmsl_dev *dev, enum max96793_link_diag_lmn_status *lmn_status)
{
    int ret;
    uint8_t data;

    REG_READ_RETURN_ON_ERR(ret, dev->i2c_desc, LF_0_DEV_REG26_ADDR, (LF_0_DEV_REG26_MASK | LF_1_DEV_REG26_MASK), &data);

    lmn_status[MAX96793_LINK_LMN_0] = ((data & LF_0_DEV_REG26_MASK) >> LF_0_DEV_REG26_POS);
    lmn_status[MAX96793_LINK_LMN_1] = ((data & LF_1_DEV_REG26_MASK) >> LF_1_DEV_REG26_POS);

    return ret;
}

/**
 * @brief Get MIPI error details for a given phy in MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param phy The phy for which the MIPI error details are to be retrieved.
 * @param mipi_phy_err_details Pointer to the structure where the MIPI error details will be stored.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_csi_diag_get_mipi_phy_err_details(struct gmsl_dev *dev, enum max96793_phy phy, struct max96793_csi_diag_mipi_phy_err_details *mipi_phy_err_details)
{
    int ret;
    uint8_t phy_err_data = 0;

    switch(phy)
    {
        case MAX96793_PHY_1:
            REG_READ_RETURN_ON_ERR(ret, dev->i2c_desc, PHY1_HS_ERR_MIPI_RX_MIPI_RX12_ADDR, PHY1_HS_ERR_MIPI_RX_MIPI_RX12_MASK, &phy_err_data);
            mipi_phy_err_details->phy_hsync_lane0_1b_err_detected = (phy_err_data & PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE0_1B_ERR_MASK);
            mipi_phy_err_details->phy_hsync_lane1_1b_err_detected = (phy_err_data & PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE1_1B_ERR_MASK) >> PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE1_1B_ERR_POS;
            mipi_phy_err_details->phy_hsync_lane0_2b_err_detected = (phy_err_data & PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE0_2B_ERR_MASK) >> PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE0_2B_ERR_POS;
            mipi_phy_err_details->phy_hsync_lane1_2b_err_detected = (phy_err_data & PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE1_2B_ERR_MASK) >> PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE1_2B_ERR_POS;
            mipi_phy_err_details->phy_hispeed_rx_skew_calib_lane1_err_detected = (phy_err_data & PHY1_HS_ERR_MIPI_RX_MIPI_RX12_SKEW_CALIB_LANE1_ERR_MASK) >> PHY1_HS_ERR_MIPI_RX_MIPI_RX12_SKEW_CALIB_LANE1_ERR_POS;
            mipi_phy_err_details->phy_hispeed_rx_skew_calib_lane0_err_detected = (phy_err_data & PHY1_HS_ERR_MIPI_RX_MIPI_RX12_SKEW_CALIB_LANE0_ERR_MASK) >> PHY1_HS_ERR_MIPI_RX_MIPI_RX12_SKEW_CALIB_LANE0_ERR_POS;
            break;

        case MAX96793_PHY_2:
            REG_READ_RETURN_ON_ERR(ret, dev->i2c_desc, PHY2_HS_ERR_MIPI_RX_MIPI_RX14_ADDR, PHY2_HS_ERR_MIPI_RX_MIPI_RX14_MASK, &phy_err_data);
            mipi_phy_err_details->phy_hsync_lane0_2b_err_detected = (phy_err_data & PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE0_2B_ERR_MASK);
            mipi_phy_err_details->phy_hsync_lane1_2b_err_detected = (phy_err_data & PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE1_2B_ERR_MASK) >> PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE1_2B_ERR_POS;
            mipi_phy_err_details->phy_hsync_lane0_1b_err_detected = (phy_err_data & PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE0_1B_ERR_MASK) >> PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE0_1B_ERR_POS;
            mipi_phy_err_details->phy_hsync_lane1_1b_err_detected = (phy_err_data & PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE1_1B_ERR_MASK) >> PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE1_1B_ERR_POS;
            mipi_phy_err_details->phy_hispeed_rx_skew_calib_lane1_err_detected = (phy_err_data & PHY2_HS_ERR_MIPI_RX_MIPI_RX14_SKEW_CALIB_LANE1_ERR_MASK) >> PHY2_HS_ERR_MIPI_RX_MIPI_RX14_SKEW_CALIB_LANE1_ERR_POS;
            mipi_phy_err_details->phy_hispeed_rx_skew_calib_lane0_err_detected = (phy_err_data & PHY2_HS_ERR_MIPI_RX_MIPI_RX14_SKEW_CALIB_LANE0_ERR_MASK) >> PHY2_HS_ERR_MIPI_RX_MIPI_RX14_SKEW_CALIB_LANE0_ERR_POS;
            break;

        default:
            ret = -ENOTSUP;
            break;
    }

    return ret;
}

/**
 * @brief Get CSI-2 controller error details for a given phy in MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param mipi_ctrl_err_details Pointer to the structure where the CSI-2 controller error details will be stored.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_csi_diag_get_mipi_ctrl_err_details(struct gmsl_dev *dev, struct max96793_csi_diag_mipi_ctrl_err_details *mipi_ctrl_err_details)
{
    int ret;
    uint8_t ctrl_csi_err_data_l = 0, ctrl_csi_err_data_h = 0;

    REG_READ_RETURN_ON_ERR(ret, dev->i2c_desc, CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_ADDR, CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_MASK, (uint8_t *)&ctrl_csi_err_data_l);

    mipi_ctrl_err_details->csi_ctrl_1b_ecc_err_detected = (ctrl_csi_err_data_l & CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_1B_ECC_ERR_MASK);
    mipi_ctrl_err_details->csi_ctrl_2b_ecc_err_detected = (ctrl_csi_err_data_l & CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_2B_ECC_ERR_MASK) >> CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_2B_ECC_ERR_POS;
    mipi_ctrl_err_details->csi_ctrl_crc_err_detected = (ctrl_csi_err_data_l & CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_CRC_ERR_MASK) >> CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_CRC_ERR_POS;

    REG_READ_RETURN_ON_ERR(ret, dev->i2c_desc, CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_ADDR, CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_MASK, (uint8_t *)&ctrl_csi_err_data_h);

    mipi_ctrl_err_details->csi_ctrl_pkt_terminated_early_err_detected = (ctrl_csi_err_data_h & CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_PKT_TERM_EARLY_ERR_MASK);
    mipi_ctrl_err_details->csi_ctrl_frame_cnt_err_detected = (ctrl_csi_err_data_h & CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_FRAME_CNT_ERR_MASK) >> CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_FRAME_CNT_ERR_POS;

    return ret;
}

/**
 * @brief Get the Low power state error details of a given phy in MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param phy Phy for which the low power state error details are to be retrieved.
 * @param lp_err_sts_details Pointer to the structure where the low power state error details will be stored.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_link_diag_check_low_power_state_error_details(struct gmsl_dev *dev, enum max96793_phy phy,struct max96793_link_diag_phy_low_power_err_status_details *lp_err_sts_details)
{
    int ret;
    uint8_t data = 0;

    switch(phy)
    {
        case MAX96793_PHY_1:
        {
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY1_LP_ERR_MIPI_RX_MIPI_RX11_ADDR, PHY1_LP_ERR_MIPI_RX_MIPI_RX11_MASK, &data);

            lp_err_sts_details->unrecgnizd_esc_cmd_rcvd_on_d0 = (data & PHY1_LP_ERR_MIPI_RX_MIPI_RX11_UNRECOGNIZED_ESC_CMD_RCVD_ON_D0_MASK) >> PHY1_LP_ERR_MIPI_RX_MIPI_RX11_UNRECOGNIZED_ESC_CMD_RCVD_ON_D0_POS;
            lp_err_sts_details->unrecgnizd_esc_cmd_rcvd_on_clk = (data & PHY1_LP_ERR_MIPI_RX_MIPI_RX11_UNRECOGNIZED_ESC_CMD_RCVD_ON_CLK_MASK) >> PHY1_LP_ERR_MIPI_RX_MIPI_RX11_UNRECOGNIZED_ESC_CMD_RCVD_ON_CLK_POS;
            lp_err_sts_details->invalid_line_seq_on_d0 = (data & PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_D0_MASK) >> PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_D0_POS;
            lp_err_sts_details->invalid_line_seq_on_d1 = (data & PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_D1_MASK) >> PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_D1_POS;
            lp_err_sts_details->invalid_line_seq_on_clk = (data & PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_CLK_MASK) >> PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_CLK_POS;

            break;
        }
        case MAX96793_PHY_2:
        {
            REG_READ_BREAK_ON_ERR(ret,dev->i2c_desc, PHY2_LP_ERR_MIPI_RX_MIPI_RX13_ADDR, PHY2_LP_ERR_MIPI_RX_MIPI_RX13_MASK, &data);

            lp_err_sts_details->unrecgnizd_esc_cmd_rcvd_on_d0 = (data & PHY2_LP_ERR_MIPI_RX_MIPI_RX13_UNRECOGNIZED_ESC_CMD_RCVD_ON_D0_MASK) >> PHY2_LP_ERR_MIPI_RX_MIPI_RX13_UNRECOGNIZED_ESC_CMD_RCVD_ON_D0_POS;
            lp_err_sts_details->unrecgnizd_esc_cmd_rcvd_on_clk = (data & PHY2_LP_ERR_MIPI_RX_MIPI_RX13_UNRECOGNIZED_ESC_CMD_RCVD_ON_CLK_MASK) >> PHY2_LP_ERR_MIPI_RX_MIPI_RX13_UNRECOGNIZED_ESC_CMD_RCVD_ON_CLK_POS;
            lp_err_sts_details->invalid_line_seq_on_d0 = (data & PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_D0_MASK) >> PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_D0_POS;
            lp_err_sts_details->invalid_line_seq_on_d1 = (data & PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_D1_MASK) >> PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_D1_POS;
            lp_err_sts_details->invalid_line_seq_on_clk = (data & PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_CLK_MASK) >> PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_CLK_POS;

            break;
        }
        default:
        {
            ret = -ENOTSUP;
            break;
        }
    }

    return ret;
}

/**
 * @brief Get the status of pipe enabled in the MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param enabled Pointer to store the pipe enabled status.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_pipe_diag_get_pipe_enabled_status(struct gmsl_dev *dev, bool *enabled)
{
    return gmsl_reg_read(dev->i2c_desc, VID_TX_EN_Z_DEV_REG2_ADDR, VID_TX_EN_Z_DEV_REG2_MASK,(uint8_t *)enabled);
}

/**
 * @brief Get the status of tunnel mode enabled in the MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param enabled Pointer to store the tunnel mode enabled status.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_pipe_diag_get_tunnel_mode_enabled_status(struct gmsl_dev *dev, bool *enabled)
{
    int ret;

    do
    {
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, TUN_MODE_MIPI_RX_EXT_EXT11_ADDR, TUN_MODE_MIPI_RX_EXT_EXT11_MASK,(uint8_t *)enabled);

#ifdef _DEBUG
        if(*enabled)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Tunnel mode is enabled");
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Tunnel mode is disabled");
        }
#endif /* _DEBUG */

    } while(false);

    return ret;
}

/**
 * @brief Get the status of remote control channel disabled in the MAX96793 device.
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param is_remote_cc_disabled Pointer to store the remote control channel disabled status.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_dev_get_remote_cc_disabled_status(struct gmsl_dev *dev, bool *is_remote_cc_disabled)
{
    return gmsl_reg_read(dev->i2c_desc, DIS_REM_CC_DEV_REG1_ADDR, DIS_REM_CC_DEV_REG1_MASK, (uint8_t *)is_remote_cc_disabled);
}

/**
 * @brief Get the status of lane configuration of the MAX96793 device.
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param lane_config_sts Pointer to store the lane configuration status.
 * @param diag_err Pointer to store the diagnostic error status.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_pipe_diag_check_lane_config_status(struct gmsl_dev *dev, struct max96793_lane_config_status *lane_config_sts, bool *diag_err)
{
    int ret;
    uint8_t sensor_lane_data;
    uint8_t lane_polarity_data;
    *diag_err = false;
    do
    {
        /* Get Lane Count */
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, CTRL1_NUM_LANES_MIPI_RX_MIPI_RX1_ADDR, CTRL1_NUM_LANES_MIPI_RX_MIPI_RX1_MASK, &lane_config_sts->lane_count);
        /* ctrl1_num_lanes bitfield value of 0 corresponds to lane count of 1. Hence incrementing lane_count by 1. */
        lane_config_sts->lane_count +=1u;

        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Lane Count: %d", lane_config_sts->lane_count);

        /* Get polarity for data lanes 0 and 1 and clock lane */
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY2_POL_MAP_MIPI_RX_MIPI_RX5_ADDR, PHY2_POL_MAP_MIPI_RX_MIPI_RX5_MASK, &lane_polarity_data);
        lane_config_sts->clk_lane_polarity = ((lane_polarity_data & PHY2_POL_MAP_CLK_LANE_MIPI_RX_MIPI_RX5_MASK) >> PHY2_POL_MAP_CLK_LANE_MIPI_RX_MIPI_RX5_POS);
        
#ifdef _DEBUG
        if(lane_config_sts->clk_lane_polarity == MAX96793_LANE_POLARITY_NORMAL)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Clock Lane Polarity: Normal");
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Clock Lane Polarity: Inverse");
        }
#endif /* _DEBUG */

        for(uint8_t ser_lane_index = 0; ser_lane_index < lane_config_sts->lane_count; ser_lane_index++)
        {
            switch(ser_lane_index)
            {
                case MAX96793_SERIALIZER_LANE_0:
                {
                    /* Get sensor lane mapping for serializer lane 0 and 1 */
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY2_LANE_MAP_MIPI_RX_MIPI_RX3_ADDR, PHY2_LANE_MAP_MIPI_RX_MIPI_RX3_MASK, &sensor_lane_data);

                    /* data_lane_polarity for lane 0 was read along with clk_lane_polarity. Hence not reading here again */

                    lane_config_sts->sensor_lane[ser_lane_index] = ((sensor_lane_data & PHY2_LANE_MAP_SER_LANE_0_MIPI_RX_MIPI_RX3_MASK) >> PHY2_LANE_MAP_SER_LANE_0_MIPI_RX_MIPI_RX3_POS);
                    lane_config_sts->data_lane_polarity[ser_lane_index] = ((lane_polarity_data & PHY2_POL_MAP_DATA_LANE_0_MIPI_RX_MIPI_RX5_MASK) >> PHY2_POL_MAP_DATA_LANE_0_MIPI_RX_MIPI_RX5_POS);

                    break;
                }
                case MAX96793_SERIALIZER_LANE_1:
                {
                    /* sensor_lane for lane 1 was read along with lane 0. Hence not reading here again */
                    /* data_lane_polarity for lane 1 was read along with clk_lane_polarity. Hence not reading here again */

                    lane_config_sts->sensor_lane[ser_lane_index] = ((sensor_lane_data & PHY2_LANE_MAP_SER_LANE_1_MIPI_RX_MIPI_RX3_MASK) >> PHY2_LANE_MAP_SER_LANE_1_MIPI_RX_MIPI_RX3_POS);
                    lane_config_sts->data_lane_polarity[ser_lane_index] = ((lane_polarity_data & PHY2_POL_MAP_DATA_LANE_1_MIPI_RX_MIPI_RX5_MASK) >> PHY2_POL_MAP_DATA_LANE_1_MIPI_RX_MIPI_RX5_POS);

                    break;
                }
                case MAX96793_SERIALIZER_LANE_2:
                {
                    /* Get sensor lane mapping for serializer lane 2 and 3 */
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY1_LANE_MAP_MIPI_RX_MIPI_RX2_ADDR, PHY1_LANE_MAP_MIPI_RX_MIPI_RX2_ALL_MASK, &sensor_lane_data);

                    /* Get polarity for data lanes 2 and 3 */
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY1_POL_MAP_MIPI_RX_MIPI_RX4_ADDR, PHY1_POL_MAP_MIPI_RX_MIPI_RX4_ALL_MASK, &lane_polarity_data);

                    lane_config_sts->sensor_lane[ser_lane_index] = ((sensor_lane_data & PHY1_LANE_MAP_SER_LANE_2_MIPI_RX_MIPI_RX2_MASK) >> PHY1_LANE_MAP_SER_LANE_2_MIPI_RX_MIPI_RX2_POS);
                    lane_config_sts->data_lane_polarity[ser_lane_index] = ((lane_polarity_data & PHY1_POL_MAP_DATA_LANE_2_MIPI_RX_MIPI_RX5_MASK) >> PHY1_POL_MAP_DATA_LANE_2_MIPI_RX_MIPI_RX5_POS);

                    break;
                }
                case MAX96793_SERIALIZER_LANE_3:
                {
                    /* sensor_lane and data_lane_polarity for lane 3 were read along with lane 2. Hence not reading here again */

                    lane_config_sts->sensor_lane[ser_lane_index] = ((sensor_lane_data & PHY1_LANE_MAP_SER_LANE_3_MIPI_RX_MIPI_RX2_MASK) >> PHY1_LANE_MAP_SER_LANE_3_MIPI_RX_MIPI_RX2_POS);
                    lane_config_sts->data_lane_polarity[ser_lane_index] = ((lane_polarity_data & PHY1_POL_MAP_DATA_LANE_3_MIPI_RX_MIPI_RX5_MASK) >> PHY1_POL_MAP_DATA_LANE_3_MIPI_RX_MIPI_RX5_POS);

                    break;
                }
                default:
                {
                    ret = -ENOTSUP;
                    break;
                }
            }

#ifdef _DEBUG
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Sensor lane %d is mapped to serializer lane %d", lane_config_sts->sensor_lane[ser_lane_index], ser_lane_index);
            if(lane_config_sts->data_lane_polarity[ser_lane_index] == MAX96793_LANE_POLARITY_NORMAL)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Polarity of lane %d: Normal", ser_lane_index);
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Polarity of lane %d: Inverse", ser_lane_index);
            }
#endif /* _DEBUG */

        }
    } while(false);

    return ret;
}

/**
 * @brief Get the data type selected for given video stream id of the MAX96793 device.
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param vs_id Video stream id for which the data type is to be retrieved.
 * @param vid_stream_config Pointer to store the data type selected for the video stream.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_csi_get_mem_data_type_selected(struct gmsl_dev *dev, enum max96793_video_stream vs_id, struct max96793_csi_video_stream_config *vid_stream_config)
{
    int ret;
    uint8_t data;

    /** @todo: Fix needed for Video stream 3 to 6. Need to implement for video streams 0, 9 to 15. */

    switch(vs_id)
    {
        case MAX96793_VID_STREAM_1:
        {
            ret = gmsl_reg_read(dev->i2c_desc, MEM_DT1_SELZ_FRONTTOP_FRONTTOP_16_ADDR, MEM_DT1_SELZ_FRONTTOP_FRONTTOP_16_MASK, &data);
            break;
        }
        case MAX96793_VID_STREAM_2:
        {
            ret = gmsl_reg_read(dev->i2c_desc, MEM_DT2_SELZ_FRONTTOP_FRONTTOP_17_ADDR, MEM_DT2_SELZ_FRONTTOP_FRONTTOP_17_MASK, &data);
            break;
        }
        case MAX96793_VID_STREAM_3:
        {
            ret = gmsl_reg_read(dev->i2c_desc, MEM_DT5_SELZ_FRONTTOP_EXT_FRONTTOP_EXT10_ADDR, MEM_DT5_SELZ_FRONTTOP_EXT_FRONTTOP_EXT10_MASK, &data);
            break;
        }
        case MAX96793_VID_STREAM_4:
        {
            ret = gmsl_reg_read(dev->i2c_desc, MEM_DT6_SELZ_FRONTTOP_EXT_FRONTTOP_EXT11_ADDR, MEM_DT6_SELZ_FRONTTOP_EXT_FRONTTOP_EXT11_MASK, &data);
            break;
        }
        case MAX96793_VID_STREAM_5:
        {
            ret = gmsl_reg_read(dev->i2c_desc, FRONTTOP_EXT12_ADDR, FRONTTOP_EXT12_MASK, &data);
            break;
        }
        case MAX96793_VID_STREAM_6:
        {
            ret = gmsl_reg_read(dev->i2c_desc, FRONTTOP_EXT13_ADDR, FRONTTOP_EXT13_MASK, &data);
            break;
        }
        case MAX96793_VID_STREAM_7:
        {
            ret = gmsl_reg_read(dev->i2c_desc, MEM_DT7_SELZ_MIPI_RX_EXT2_EXTA_ADDR, MEM_DT7_SELZ_MIPI_RX_EXT2_EXTA_MASK, &data);
            break;
        }
        case MAX96793_VID_STREAM_8:
        {
            ret = gmsl_reg_read(dev->i2c_desc, MEM_DT8_SELZ_MIPI_RX_EXT2_EXTB_ADDR, MEM_DT8_SELZ_MIPI_RX_EXT2_EXTB_MASK, &data);
            break;
        }
        default:
        {
            /* Do not return -ENOTSUP here */
            break;
        }
    }

    vid_stream_config->data_type = (enum max96793_data_type)(data & DATA_TYPE_MASK);
    vid_stream_config->is_data_type_enabled = (bool)((data & DATA_TYPE_ENABLE_MASK) >> DATA_TYPE_ENABLE_POS);

#ifdef _DEBUG
    if(vid_stream_config->is_data_type_enabled)
    {
        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Data type selected for video stream %d is %d", vs_id, vid_stream_config->data_type);
    }
    else
    {
        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Data type is not enabled for video stream %d", vs_id);
    }
#endif /* _DEBUG */

    return ret;
}

/**
 * @brief Get the Bits Per Pixel (bpp) of a given data type in the MAX96793 device.
 * 
 * @param data_type Data type for which the bpp is to be retrieved.
 * 
 * @return bpp of the given data type.
 */
static enum max96793_data_type_size max96793_csi_get_bpp_for_data_type(enum max96793_data_type data_type)
{
    enum max96793_data_type_size bpp = MAX96793_DT_SIZE_0;

    switch(data_type)
    {
        case MAX96793_DATA_TYPE_RAW8:
        case MAX96793_DATA_TYPE_GENERIC8_A:
        case MAX96793_DATA_TYPE_GENERIC8_B:
        case MAX96793_DATA_TYPE_EMB8:
        case MAX96793_DATA_TYPE_UDP8_A:
        case MAX96793_DATA_TYPE_UDP8_B:
        case MAX96793_DATA_TYPE_UDP8_C:
        case MAX96793_DATA_TYPE_UDP8_D:
        case MAX96793_DATA_TYPE_UDP8_E:
        case MAX96793_DATA_TYPE_UDP8_F:
        case MAX96793_DATA_TYPE_UDP8_G:
        case MAX96793_DATA_TYPE_YUV422_8BIT:
        {
            bpp = MAX96793_DT_SIZE_8;
            break;
        }

        case MAX96793_DATA_TYPE_RAW10:
        case MAX96793_DATA_TYPE_YUV422_10BIT:
        {
            bpp = MAX96793_DT_SIZE_10;
            break;
        }

        case MAX96793_DATA_TYPE_RAW12:
        case MAX96793_DATA_TYPE_YUV422_12BIT:
        {
            bpp = MAX96793_DT_SIZE_12;
            break;
        }

        case MAX96793_DATA_TYPE_RAW14:
        {
            bpp = MAX96793_DT_SIZE_14;
            break;
        }

        case MAX96793_DATA_TYPE_RAW16:
        case MAX96793_DATA_TYPE_RGB565:
        {
            bpp = MAX96793_DT_SIZE_16;
            break;
        }

        case MAX96793_DATA_TYPE_RGB666:
        {
            bpp = MAX96793_DT_SIZE_18;
            break;
        }

        case MAX96793_DATA_TYPE_RAW20:
        {
            bpp = MAX96793_DT_SIZE_20;
            break;
        }

        case MAX96793_DATA_TYPE_RGB888:
        {
            bpp = MAX96793_DT_SIZE_24;
            break;
        }

        default:
        {
            /* Not Supported */
            break;
        }
    }

    return bpp;
}

/**
 * @brief Get the pixel doubling enabled status of the MAX96793 device.
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param smallest_bpp Smallest value of bits per pixel of all data types configured.
 * @param enabled_status Pointer to store the pixel doubling enabled status.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_csi_get_pixel_doubling_enabled_status(struct gmsl_dev *dev, enum max96793_data_type_size smallest_bpp, bool *enabled_status)
{
    int ret;
    *enabled_status = false;

    switch(smallest_bpp)
    {
        case MAX96793_DT_SIZE_8:
        {
            ret = gmsl_reg_read(dev->i2c_desc, BPP8DBLZ_FRONTTOP_FRONTTOP_10_ADDR, BPP8DBLZ_FRONTTOP_FRONTTOP_10_MASK, (uint8_t *)enabled_status);

            break;
        }

        case MAX96793_DT_SIZE_10:
        {
            ret = gmsl_reg_read(dev->i2c_desc, BPP10DBLZ_FRONTTOP_FRONTTOP_11_ADDR, BPP10DBLZ_FRONTTOP_FRONTTOP_11_MASK, (uint8_t *)enabled_status);

            break;
        }

        case MAX96793_DT_SIZE_12:
        {
            ret = gmsl_reg_read(dev->i2c_desc, BPP12DBLZ_FRONTTOP_FRONTTOP_11_ADDR, BPP12DBLZ_FRONTTOP_FRONTTOP_11_MASK, (uint8_t *)enabled_status);

            break;
        }

        default:
        {
            /* Do Nothing */
            break;
        }
    }

    return ret;
}

/**
 * @brief Get the CSI configuration status of the MAX96793 device.
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param csi_config_status Pointer to store the CSI configuration status.
 * @param diag_err Pointer to store the diagnostic error status.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_csi_diag_check_config_status(struct gmsl_dev *dev, struct max96793_csi_diag_part_config_status *csi_config_status, bool *diag_err)
{
    int ret;
    uint8_t virtual_channel_selected_low;
    uint8_t virtual_channel_selected_high;
    uint16_t virtual_channel_selected;
    enum max96793_data_type smallest_data_type = MAX96793_DATA_TYPE_RGB888; /* Initially assigning the largest datatype */
    enum max96793_data_type_size smallest_data_type_size = MAX96793_DT_SIZE_24; /* Initially assigning the largest value */
    enum max96793_data_type_size selected_data_type_size;
    enum max96793_data_type_size smallest_bpp_to_be_doubled = 0U;
    bool can_dt_be_doubled;
    enum max96793_data_type_size soft_bpp_expected;
    uint8_t data;
    bool stream_id_diag_err;
    *diag_err = false;
    do
    {
        /* Get tunnel mode enabled status */
        ret = max96793_pipe_diag_get_tunnel_mode_enabled_status(dev, &csi_config_status->tunnel_mode_enabled);
        BREAK_ON_ERR(ret);

        if(csi_config_status->tunnel_mode_enabled)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pixel mode parameters are not checked as Tunnel Mode is enabled");
        }
        else
        {
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, VC_SELZ_L_FRONTTOP_FRONTTOP_5_ADDR, VC_SELZ_L_FRONTTOP_FRONTTOP_5_MASK, &virtual_channel_selected_low);

            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, VC_SELZ_H_FRONTTOP_FRONTTOP_6_ADDR, VC_SELZ_H_FRONTTOP_FRONTTOP_6_MASK, &virtual_channel_selected_high);

            virtual_channel_selected = (uint16_t)((virtual_channel_selected_high << 8) | virtual_channel_selected_low);

            for(uint8_t vc_index = 0; vc_index < MAX96793_MAX_VIRTUAL_CHANNEL; vc_index++)
            {
                if(((virtual_channel_selected & (1u << vc_index)) != 0U))
                {
                    csi_config_status->video_stream_config[vc_index].is_virtual_channel_enabled = true;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Virtual Channel %d is enabled", vc_index);

                    ret = max96793_csi_get_mem_data_type_selected(dev, (enum max96793_video_stream)(vc_index +1u), &csi_config_status->video_stream_config[vc_index]);
                    BREAK_ON_ERR(ret);

                    selected_data_type_size = max96793_csi_get_bpp_for_data_type(csi_config_status->video_stream_config[vc_index].data_type);
                    /* Get the smallest data type */
                    if(selected_data_type_size < smallest_data_type_size)
                    {
                        smallest_data_type_size = selected_data_type_size;
                        smallest_data_type = csi_config_status->video_stream_config[vc_index].data_type;
                    }
                }
                else
                {
                    csi_config_status->video_stream_config[vc_index].is_virtual_channel_enabled = false;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Virtual Channel %d is disabled. Data type status is not checked.", vc_index);
                }
            }

            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Smallest data type selected: %d", smallest_data_type);

            /* Get independent_vs_mode enabled status */
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, INDEPENDENT_VS_FRONTTOP_13_ADDR, INDEPENDENT_VS_FRONTTOP_13_MASK, (uint8_t *)&csi_config_status->is_independent_mode_enabled);

#ifdef _DEBUG
            if(csi_config_status->is_independent_mode_enabled)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Independent Video Stream Mode is enabled");
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Independent Video Stream Mode is disabled");
            }
#endif /* _DEBUG */

            smallest_bpp_to_be_doubled = max96793_csi_get_bpp_for_data_type(smallest_data_type);
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "BPP for smallest data type: %d", smallest_bpp_to_be_doubled);


            can_dt_be_doubled = (smallest_bpp_to_be_doubled <= MAX96793_DT_SIZE_12);

            /* Get pixel doubling status */
            ret = max96793_csi_get_pixel_doubling_enabled_status(dev, smallest_bpp_to_be_doubled, &csi_config_status->is_pixel_doubling_enabled);
            BREAK_ON_ERR(ret);

            if(csi_config_status->is_pixel_doubling_enabled == can_dt_be_doubled)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pixel Doubling enabled status is %d as expected", csi_config_status->is_pixel_doubling_enabled);
            }
            else
            {
                *diag_err = true;
                GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Pixel Doubling enabled status is not as expected. Expected: %d, Actual: %d", can_dt_be_doubled, csi_config_status->is_pixel_doubling_enabled);
            }

            if(can_dt_be_doubled)
            {
                switch(smallest_bpp_to_be_doubled)
                {
                    case MAX96793_DT_SIZE_8:
                    {
                        soft_bpp_expected = MAX96793_DT_SIZE_16;
                        break;
                    }

                    case MAX96793_DT_SIZE_10:
                    {
                        soft_bpp_expected = MAX96793_DT_SIZE_20;
                        break;
                    }

                    case MAX96793_DT_SIZE_12:
                    {
                        soft_bpp_expected = MAX96793_DT_SIZE_24;
                        break;
                    }

                    default :
                    {
                        /* Do Nothing */
                        break;
                    }
                }

                /* Get soft bpp enabled status and soft bpp over ride value */
                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, SOFT_BPPZ_FRONTTOP_FRONTTOP_22_ADDR, (SOFT_BPPZ_FRONTTOP_FRONTTOP_22_MASK | SOFT_BPPZ_EN_FRONTTOP_FRONTTOP_22_MASK), &data);

                if((data & SOFT_BPPZ_EN_FRONTTOP_FRONTTOP_22_MASK) != 0u)
                {
                    csi_config_status->is_soft_bpp_enabled = true;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Soft BPP is enabled");

                    csi_config_status->soft_bpp_override = (enum max96793_data_type_size)(data & SOFT_BPPZ_FRONTTOP_FRONTTOP_22_MASK);
                    if(soft_bpp_expected != csi_config_status->soft_bpp_override)
                    {
                        *diag_err = true;
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Soft BPP override value is not as expected. Expected: %d, Actual: %d", soft_bpp_expected, csi_config_status->soft_bpp_override);
                    }
                    else
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Soft BPP override value is %d as expected.", soft_bpp_expected);
                    }
                }
                else
                {
                    csi_config_status->is_soft_bpp_enabled = false;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Soft BPP is disabled. Not checking the soft BPP override value");
                }
            }
        }

        /* Using stream_id_diag_err and not passing diag_err directly because any previously reported error in diag_err will
           get cleared if max96793_pipe_diag_check_stream_id() has no error */
        ret = max96793_pipe_diag_check_stream_id(dev, (void *)&(csi_config_status->stream_id_status), &stream_id_diag_err);
        BREAK_ON_ERR(ret);

        if(stream_id_diag_err)
        {
            *diag_err = true;
        }
        else
        {
            /* Do nothing */
        }


    } while(false);

    return ret;
}

/**
 * @brief Get the pipe configuration status of the MAX96793 device.
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param pipe_config_status Pointer to store the pipe configuration status.
 * @param diag_err Pointer to store the diagnostic error status.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96793_pipe_diag_check_config_status(struct gmsl_dev *dev, struct max96793_pipe_diag_config_status *pipe_config_status, bool *diag_err)
{
    int ret;
    uint8_t data;
    *diag_err = false;
    do
    {
        /* Get Port Configuration */
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_CONFIG_MIPI_RX_MIPI_RX0_ADDR, PHY_CONFIG_MIPI_RX_MIPI_RX0_MASK, &pipe_config_status->phy_config);

        if(MAX96793_PHY_1x4_MODE == pipe_config_status->phy_config)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "PHY Configuration: 1x4 Mode");
        }
        else
        {
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Incorrect PHY Configuration value: %d", pipe_config_status->phy_config);
            *diag_err = true;
        }

        ret = max96793_pipe_diag_check_lane_config_status(dev, &pipe_config_status->lane_config_status, diag_err);
        BREAK_ON_ERR(ret);

        /* Get CSI port selected and START_PORTB status */
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, FRONTTOP_FRONTTOP_0_ADDR, FRONTTOP_FRONTTOP_0_ALL_MASK, &data);

        pipe_config_status->port_selected = (enum max96793_port)((data & CLK_SELZ_FRONTTOP_FRONTTOP_0_MASK) >> CLK_SELZ_FRONTTOP_FRONTTOP_0_POS);
        pipe_config_status->is_port_enabled = (bool)((data & START_PORTB_FRONTTOP_FRONTTOP_0_MASK) >> START_PORTB_FRONTTOP_FRONTTOP_0_POS);

        if(MAX96793_PORT_B == pipe_config_status->port_selected)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "CSI Port selected: Port B");
        }
        else
        {
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "CSI Port selected: Port A");
        }

        if(pipe_config_status->is_port_enabled)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "CSI Port is enabled");

            /* Get START_PORTBZ status */
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, START_PORTBZ_FRONTTOP_FRONTTOP_9_ADDR, START_PORTBZ_FRONTTOP_FRONTTOP_9_MASK, (uint8_t *)&pipe_config_status->is_video_started_in_port);

            if(pipe_config_status->is_video_started_in_port)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video is started in CSI Port");
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video is not started in CSI Port");
            }
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "CSI Port is disabled. Not checking the video started status");
        }
    } while (false);
    
    return ret;
}

#endif /*ADI_DISABLE_DOCUMENTATION */

/******************************************************************************/
/************************* Functions Definition *******************************/
/******************************************************************************/

/**
 * @brief Retrieve the device ID of the MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param dev_id_status Pointer to the structure where the device ID status will be stored.
 * @param diag_err Pointer to a boolean that will be set to true if a diagnostic error occurs.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_dev_diag_get_device_id(struct gmsl_dev *dev, void *dev_id_status, bool *diag_err)
{
    int ret;
    struct max96793_dev_diag_device_id_status *dev_id_sts = (struct max96793_dev_diag_device_id_status *)dev_id_status;
    *diag_err = false;

    ret = gmsl_reg_read(dev->i2c_desc, DEV_REG13_ADDR, DEV_ID_DEV_REG13_MASK, &dev_id_sts->dev_id);
    if(ret)
        goto error;

    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Device ID: %d", dev_id_sts->dev_id);

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Retrieve the device revision status of the MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param dev_revision_status Pointer to the structure where the device revision status will be stored.
 * @param diag_err Pointer to a boolean that will be set to true if a diagnostic error occurs.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_dev_diag_check_device_rev(struct gmsl_dev *dev, void *dev_revision_status, bool *diag_err)
{
    int ret;
    struct max96793_dev_diag_device_revision_status *dev_rev_sts = (struct max96793_dev_diag_device_revision_status *)dev_revision_status;
    *diag_err = false;

    ret = gmsl_reg_read(dev->i2c_desc, DEV_REG14_ADDR, DEV_REV_DEV_REG14_MASK, &dev_rev_sts->dev_rev);
    if(ret)
        goto error;

    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Device revision: %d", dev_rev_sts->dev_rev);

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check decode error status for MAX96793 link diagnostics.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param decode_err_status Pointer to the structure to store the decode error status.
 * @param diag_err Pointer to a boolean to store the diagnostic error status.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_link_diag_check_decode_error_status(struct gmsl_dev *dev, void *decode_err_status, bool *diag_err)
{
    int ret;
    struct max96793_pipe_diag_decode_error_status *decode_err_sts = (struct max96793_pipe_diag_decode_error_status *)decode_err_status;
    bool current_diag_err;
    *diag_err = false;

    /* Clear initially */
    decode_err_sts->decode_err_link = false;
    decode_err_sts->decode_err_cnt = 0;

    ret = gmsl_reg_read(dev->i2c_desc, DEC_ERR_FLAG_A_TCTRL_INTR3_ADDR, DEC_ERR_FLAG_A_TCTRL_INTR3_MASK, (uint8_t *)&decode_err_sts->decode_err_link);
    if(ret)
        goto error;

    FLAG_DIAG_ERR(decode_err_sts->decode_err_link, current_diag_err, *diag_err);

    if(current_diag_err)
    {
        ret = gmsl_reg_read(dev->i2c_desc,DEC_ERR_A_TCTRL_CNT0_ADDR, DEC_ERR_A_TCTRL_CNT0_MASK, (uint8_t *)&decode_err_sts->decode_err_cnt);
        if(ret)
            goto error;

        GMSL_LOG_DIAG(current_diag_err, dev->dev_name, dev->i2c_desc->slave_address, "Decoding Error reported for Link A with count - %u", decode_err_sts->decode_err_cnt);
    }
    else
    {
        GMSL_LOG_DIAG(current_diag_err, dev->dev_name, dev->i2c_desc->slave_address, "No Decoding Error reported for Link A");
    }

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check the idle error status of the MAX96793 link.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param idle_err_status Pointer to the structure to store idle error status.
 * @param diag_err Pointer to a boolean to store the diagnostic error status.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_link_diag_check_idle_error_status(struct gmsl_dev *dev, void *idle_err_status, bool *diag_err)
{
    int ret;
    struct max96793_pipe_diag_idle_error_status *idle_err_sts = (struct max96793_pipe_diag_idle_error_status *)idle_err_status;
    bool current_diag_err;
    *diag_err = false;

    /* Clear initially */
    idle_err_sts->idle_err_detected = false;
    idle_err_sts->idle_err_cnt = 0;

    ret = gmsl_reg_read(dev->i2c_desc, IDLE_ERR_FLAG_TCTRL_INTR3_ADDR, IDLE_ERR_FLAG_TCTRL_INTR3_MASK, (uint8_t *)&idle_err_sts->idle_err_detected);
    if(ret)
        goto error;

    FLAG_DIAG_ERR(idle_err_sts->idle_err_detected, current_diag_err, *diag_err);

    if(current_diag_err)
    {
        ret = gmsl_reg_read(dev->i2c_desc, IDLE_ERR_TCTRL_CNT2_ADDR, IDLE_ERR_TCTRL_CNT2_MASK, (uint8_t *)&idle_err_sts->idle_err_cnt);
        if(ret)
            goto error;

        GMSL_LOG_DIAG(current_diag_err, dev->dev_name, dev->i2c_desc->slave_address, "Idle Error reported for Link A with count - %u", idle_err_sts->idle_err_cnt);
    }
    else
    {
        GMSL_LOG_DIAG(current_diag_err, dev->dev_name, dev->i2c_desc->slave_address, "No Idle Error reported for Link A");
    }

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check the link lock status of the MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param link_lock_status Pointer to the structure where the link lock status will be stored.
 * @param diag_err Pointer to a boolean that will be set to true if a diagnostic error is detected.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_link_diag_check_link_lock_status(struct gmsl_dev *dev, void *link_lock_status, bool *diag_err)
{
    int ret;
    struct max96793_link_diag_link_lock_status *link_lock_sts = (struct max96793_link_diag_link_lock_status *)link_lock_status;
    *diag_err = false;

    ret = gmsl_reg_read(dev->i2c_desc, LOCKED_TCTRL_CTRL3_ADDR, LOCKED_TCTRL_CTRL3_MASK, (uint8_t *)&link_lock_sts->link_locked);
    if(ret)
        goto error;

    if(link_lock_sts->link_locked != true)
    {
        *diag_err = true;
        GMSL_LOG_ERROR( dev->dev_name, dev->i2c_desc->slave_address, "Link A locked status: %d", link_lock_sts->link_locked);
    }
    else
    {
        GMSL_LOG_INFO( dev->dev_name, dev->i2c_desc->slave_address, "Link A locked status: %d", link_lock_sts->link_locked);
    }

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check the line fault status of the MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param line_fault_status Pointer to the structure where line fault status details will be stored.
 * @param diag_err Pointer to a boolean flag that will be set to true if any diagnostic errors are detected.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_link_diag_check_line_fault_status(struct gmsl_dev *dev, void *line_fault_status, bool *diag_err)
{
    int ret;
    struct max96793_link_diag_line_fault_status *line_fault_sts = (struct max96793_link_diag_line_fault_status *)line_fault_status;
    bool line_fault_enable_status[MAX96793_LMN_COUNT];

    /* Clear initially by default */
    *diag_err = false;

    /* Set LMN status to normal initially */
    for(uint8_t lmn = 0; lmn < MAX96793_LMN_COUNT; lmn++)
    {
        line_fault_sts->lmn_err_status[lmn] = MAX96793_LINK_DIAG_LMN_NORMAL;
    }

    ret = max96793_link_diag_get_line_fault_enabled_status(dev, line_fault_enable_status);
    if(ret)
        goto error;

    if(!(line_fault_enable_status[MAX96793_LINK_LMN_0] || line_fault_enable_status[MAX96793_LINK_LMN_1]))
    {
        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Detection is not enabled");
        return ret;
    }

    for(uint8_t lmn = 0; lmn < MAX96793_LMN_COUNT; lmn++)
    {
        GMSL_LOG_INFO("[%s-0x%x]LMN %d enabled status is %d", dev->dev_name, dev->i2c_desc->slave_address, lmn, line_fault_enable_status[lmn]);
    }

    ret = gmsl_reg_read(dev->i2c_desc, LFLT_INT_TCTRL_INTR3_ADDR, LFLT_INT_TCTRL_INTR3_MASK, (uint8_t *)&line_fault_sts->lf_int);
    if(ret)
        goto error;

    *diag_err = line_fault_sts->lf_int;

    if(!line_fault_sts->lf_int)
    {
        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault is not detected");
        return ret;
    }

    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault is detected");

    if((line_fault_enable_status[MAX96793_LINK_LMN_0] || line_fault_enable_status[MAX96793_LINK_LMN_1]))
    {
        ret = max96793_link_diag_get_line_fault_status(dev, line_fault_sts->lmn_err_status);
        if(ret)
            goto error;
    }

    for(uint8_t lmn = 0; lmn < MAX96793_LMN_COUNT; lmn++)
    {
        if(line_fault_enable_status[lmn]) {
            switch (line_fault_sts->lmn_err_status[lmn])
            {
                case MAX96793_LINK_DIAG_LMN_SHORT_TO_BAT:
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Monitor %d: Short to battery", lmn);
                    break;
                case MAX96793_LINK_DIAG_LMN_SHORT_TO_GND:
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Monitor %d: Short to GND", lmn);
                    break;
                case MAX96793_LINK_DIAG_LMN_NORMAL:
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Monitor %d: Normal operation", lmn);
                    break;
                case MAX96793_LINK_DIAG_LMN_LINE_OPEN:
                default:
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Monitor %d: Line open", lmn);
                    break;
            }
        }
    }

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d\r\n", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check the PCLK detect status of the MAX96793 serializer
 *
 * @param dev                  Device handle.
 * @param pclk_detect_status   Pointer to store the PCLK detect status.
 * @param diag_err             Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_csi_diag_check_pclk_detect_status(struct gmsl_dev *dev, void *pclk_detect_status, bool *diag_err)
{
    int ret;
    struct max96793_csi_diag_pclkdet_status *pclk_detect_sts = (struct max96793_csi_diag_pclkdet_status *)pclk_detect_status;
    *diag_err = false;

    ret = gmsl_reg_read(dev->i2c_desc, PCLKDET_VID_TX_Z_VIDEO_TX2_ADDR, PCLKDET_VID_TX_Z_VIDEO_TX2_MASK, (uint8_t *)&pclk_detect_sts->pclk_detected);
    if(ret)
        goto error;

    *diag_err = !pclk_detect_sts->pclk_detected;
    GMSL_LOG_DIAG(*diag_err, dev->dev_name, dev->i2c_desc->slave_address, "PCLK Detect Status: %s", ASSERT_DETECT_STR(pclk_detect_sts->pclk_detected));

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d\r\n", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check the video transmitter overflow status of the MAX96793 serializer
 *
 * @param dev  Device handle.
 * @param vid_tx_ovrflw_status Pointer to store the video transmitter overflow status.
 * @param diag_err  Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_pipe_diag_check_vid_tx_overflow_status(struct gmsl_dev *dev, void *vid_tx_ovrflw_status, bool *diag_err)
{
    int ret;
    *diag_err = false;
    struct max96793_pipe_diag_vid_tx_overflow_status *vid_tx_ovrflw_sts = (struct max96793_pipe_diag_vid_tx_overflow_status *)vid_tx_ovrflw_status;

    ret = gmsl_reg_read(dev->i2c_desc, OVERFLOW_VID_TX_Z_VIDEO_TX2_ADDR, OVERFLOW_VID_TX_Z_VIDEO_TX2_MASK, (uint8_t *)&vid_tx_ovrflw_sts->vid_tx_overflowed);
    if(ret)
        goto error;

    *diag_err = vid_tx_ovrflw_sts->vid_tx_overflowed;
    GMSL_LOG_DIAG(*diag_err, dev->dev_name, dev->i2c_desc->slave_address, "Video TX FIFO over flow error: %s", ASSERT_DETECT_STR(*diag_err));

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d\r\n", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check the video transmitter PCLK drift error status of the MAX96793 serializer
 *
 * @param dev  Device handle.
 * @param vid_tx_pclk_drift_status Pointer to store the PCLK drift status.
 * @param diag_err Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_csi_diag_check_vid_tx_pclk_drift_err_status(struct gmsl_dev *dev, void *vid_tx_pclk_drift_status, bool *diag_err)
{
    int ret;
    struct max96793_pipe_diag_vid_tx_pclk_drift_err_status *vid_tx_pclk_drift_sts = (struct max96793_pipe_diag_vid_tx_pclk_drift_err_status *)vid_tx_pclk_drift_status;
    *diag_err = false;

    ret = gmsl_reg_read(dev->i2c_desc, LIM_HEART_VID_TX_Z_VIDEO_TX2_ADDR, LIM_HEART_VID_TX_Z_VIDEO_TX2_MASK, (uint8_t *)&vid_tx_pclk_drift_sts->vid_tx_pclk_drift_err_detected);
    if(ret)
        goto error;

    *diag_err = vid_tx_pclk_drift_sts->vid_tx_pclk_drift_err_detected;
    GMSL_LOG_DIAG(*diag_err, dev->dev_name, dev->i2c_desc->slave_address, "Video TX PClk drift error: %s", ASSERT_DETECT_STR(*diag_err));

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d\r\n", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check the 2-bit uncorrectable memory ECC errors seen in any memories of the MAX96793 serializer
 *
 * @param dev  Device handle.
 * @param uncorrectable_2b_mem_ecc_err_status Pointer to 2-bit uncorrectable memory ECC error status.
 * @param diag_err Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_mem_diag_check_uncorrectable_2b_mem_ecc_err_status(struct gmsl_dev *dev, void *uncorrectable_2b_mem_ecc_err_status, bool *diag_err)
{
    int ret;
    *diag_err = false;
    struct max96793_mem_diag_uncorrectable_2b_mem_ecc_err_status *uncorrectable_2b_mem_ecc_err_sts = (struct max96793_mem_diag_uncorrectable_2b_mem_ecc_err_status *)uncorrectable_2b_mem_ecc_err_status;

    ret = gmsl_reg_read(dev->i2c_desc, MEM_ECC_ERR2_INT_FUNC_SAFE_FS_INTR1_ADDR, MEM_ECC_ERR2_INT_FUNC_SAFE_FS_INTR1_MASK, (uint8_t *)&uncorrectable_2b_mem_ecc_err_sts->uncorrectable_2b_mem_ecc_err_detected);
    if(ret)
        goto error;

    *diag_err = uncorrectable_2b_mem_ecc_err_sts->uncorrectable_2b_mem_ecc_err_detected;
    GMSL_LOG_DIAG(*diag_err, dev->dev_name, dev->i2c_desc->slave_address, "Uncorrectable 2bit memory ecc error: %s", ASSERT_DETECT_STR(*diag_err));

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d\r\n", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}
/**
 * @brief Checks the maximum retransmission error status for the MAX96793 device.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param max_retransmission_status Pointer to the structure where the maximum retransmission status will be stored.
 * @param diag_err Pointer to a boolean that will be set to true if any diagnostic error is detected.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_link_diag_check_max_retransmission_error_status(struct gmsl_dev *dev, void *max_retransmission_status, bool *diag_err)
{
    int ret = 0;
    uint8_t spi_chnl_err = 0u;
    uint8_t gpio_chnl_err = 0u;
    uint8_t passthrough_chnl1_err = 0u;
    uint8_t passthrough_chnl2_err = 0u;
    *diag_err = false;

    struct max96793_pipe_diag_max_retransmission_error_status *max_retransmission_sts = (struct max96793_pipe_diag_max_retransmission_error_status *)max_retransmission_status;
    /* Check for Combined Max Retransmission Error */
    ret = gmsl_reg_read(dev->i2c_desc, MAX_RT_FLAG_TCTRL_INTR5_ADDR, MAX_RT_FLAG_TCTRL_INTR5_MASK, (uint8_t *)&max_retransmission_sts->max_retrans_err_detected);
    if(ret)
        goto error;

    *diag_err = max_retransmission_sts->max_retrans_err_detected;

    if(max_retransmission_sts->max_retrans_err_detected)
    {
        /* Check for SPI Channel retransmission error */
        ret = gmsl_reg_read(dev->i2c_desc, CFGL_SPI_ARQ2_ADDR, CFGL_SPI_ARQ2_MASK, &spi_chnl_err);
        if(ret)
            goto error;

        max_retransmission_sts->max_retrans_data.spi_chnl_retrans_err_detected = ((spi_chnl_err & MAX_RT_ERR_CFGL_SPI_ARQ2_MASK)  != 0u);
        max_retransmission_sts->max_retrans_data.spi_chnl_retrans_count = spi_chnl_err & RT_CNT_CFGL_SPI_ARQ2_MASK;

        if(max_retransmission_sts->max_retrans_data.spi_chnl_retrans_err_detected)
        {
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Max Retransmission Error reported for SPI Channel with count - %d", max_retransmission_sts->max_retrans_data.spi_chnl_retrans_count);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported for SPI Channel");
        }

        /* Check for GPIO Channel error */
        ret = gmsl_reg_read(dev->i2c_desc, CFGL_GPIO_ARQ2_ADDR, CFGL_GPIO_ARQ2_MASK, &gpio_chnl_err);
        if(ret)
            goto error;

        max_retransmission_sts->max_retrans_data.gpio_chnl_retrans_err_detected = ((gpio_chnl_err & MAX_RT_ERR_CFGL_GPIO_ARQ2_MASK)  != 0u);
        max_retransmission_sts->max_retrans_data.gpio_chnl_retrans_count = gpio_chnl_err & RT_CNT_CFGL_GPIO_ARQ2_MASK;

        if(max_retransmission_sts->max_retrans_data.gpio_chnl_retrans_err_detected)
        {
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Max Retransmission Error reported for GPIO Channel with count - %d", max_retransmission_sts->max_retrans_data.gpio_chnl_retrans_count);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported for GPIO Channel");
        }

        /* Check for Pass through Channel 1 error */
        ret = gmsl_reg_read(dev->i2c_desc, CFGL_IIC_X_ARQ2_ADDR, CFGL_IIC_X_ARQ2_MASK, &passthrough_chnl1_err);
        if(ret)
            goto error;

        max_retransmission_sts->max_retrans_data.pass_thru_chnl1_retrans_err_detected = ((passthrough_chnl1_err & MAX_RT_ERR_CFGL_IIC_X_ARQ2_MASK)  != 0u);
        max_retransmission_sts->max_retrans_data.pass_thru_chnl1_retrans_count = passthrough_chnl1_err & RT_CNT_CFGL_IIC_X_ARQ2_MASK;

        if(max_retransmission_sts->max_retrans_data.pass_thru_chnl1_retrans_err_detected)
        {
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Max Retransmission Error reported for Pass Through Channel 1 with count - %d", max_retransmission_sts->max_retrans_data.pass_thru_chnl1_retrans_count);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported for Pass Through Channel 1");
        }

        /* Check for Pass through Channel 2 error */
        ret = gmsl_reg_read(dev->i2c_desc, CFGL_IIC_Y_ARQ2_ADDR, CFGL_IIC_Y_ARQ2_MASK, &passthrough_chnl2_err);
        if(ret)
            goto error;

        max_retransmission_sts->max_retrans_data.pass_thru_chnl2_retrans_err_detected = ((passthrough_chnl2_err & MAX_RT_ERR_CFGL_IIC_Y_ARQ2_MASK)  != 0u);
        max_retransmission_sts->max_retrans_data.pass_thru_chnl2_retrans_count = passthrough_chnl2_err & RT_CNT_CFGL_IIC_Y_ARQ2_MASK;

        if(max_retransmission_sts->max_retrans_data.pass_thru_chnl2_retrans_err_detected)
        {
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Max Retransmission Error reported for Pass Through Channel 2 with count - %d", max_retransmission_sts->max_retrans_data.pass_thru_chnl2_retrans_count);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported for Pass Through Channel 2");
        }
    }
    else
    {
        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported");
    }

    return ret;

    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check the MIPI RX error status of the MAX96793 serializer
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param mipi_rx_err_status Pointer to store the MIPI RX error status.
 * @param diag_err Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_csi_diag_check_mipi_rx_err_status(struct gmsl_dev *dev, void *mipi_rx_err_status, bool *diag_err)
{
    int ret;
    struct max96793_csi_diag_mipi_rx_err_status *mipi_rx_err_sts = (struct max96793_csi_diag_mipi_rx_err_status *)mipi_rx_err_status;
    *diag_err = false;

    ret = gmsl_reg_read(dev->i2c_desc, MIPI_ERR_FLAG_TCTRL_INTR7_ADDR, MIPI_ERR_FLAG_TCTRL_INTR7_MASK, (uint8_t *)&mipi_rx_err_sts->mipi_rx_err_detected);
    if(ret)
        goto error;

    if(mipi_rx_err_sts->mipi_rx_err_detected)
    {
        *diag_err = true;
        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "MIPI RX Error reported");

        for(uint8_t phy = 0; phy < MAX96793_MIPI_PHY_COUNT; phy++)
        {
            ret = max96793_csi_diag_get_mipi_phy_err_details(dev, (enum max96793_phy)phy, &mipi_rx_err_sts->mipi_phy_err_details[phy]);
            if(ret)
                goto error;

            GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hsync_lane0_1b_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "PHY %d HSYNC Lane 0 1bit Error: %s", phy, ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hsync_lane0_1b_err_detected));
            GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hsync_lane1_1b_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "PHY %d HSYNC Lane 1 1bit Error: %s", phy, ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hsync_lane1_1b_err_detected));
            GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hsync_lane0_2b_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "PHY %d HSYNC Lane 0 2bit Error: %s", phy, ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hsync_lane0_2b_err_detected));
            GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hsync_lane1_2b_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "PHY %d HSYNC Lane 1 2bit Error: %s", phy, ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hsync_lane1_2b_err_detected));
            GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hispeed_rx_skew_calib_lane0_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "PHY %d High Speed RX Skew Calibration Lane 0 Error: %s", phy, ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hispeed_rx_skew_calib_lane0_err_detected));
            GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hispeed_rx_skew_calib_lane1_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "PHY %d High Speed RX Skew Calibration Lane 1 Error: %s", phy, ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_phy_err_details[phy].phy_hispeed_rx_skew_calib_lane1_err_detected));
        }

        ret = max96793_csi_diag_get_mipi_ctrl_err_details(dev, &mipi_rx_err_sts->mipi_ctrl_err_details);
        if(ret)
            goto error;

        GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_1b_ecc_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "CSI Controller 1bit ECC Error: %s", ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_1b_ecc_err_detected));
        GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_2b_ecc_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "CSI Controller 2bit ECC Error: %s", ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_2b_ecc_err_detected));
        GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_crc_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "CSI Controller CRC Error: %s", ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_crc_err_detected));
        GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_pkt_terminated_early_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "CSI Controller Packet Terminated Early Error: %s", ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_pkt_terminated_early_err_detected));
        GMSL_LOG_DIAG(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_frame_cnt_err_detected, dev->dev_name, dev->i2c_desc->slave_address, "CSI Controller Frame Count Error: %s", ASSERT_DETECT_STR(mipi_rx_err_sts->mipi_ctrl_err_details.csi_ctrl_frame_cnt_err_detected));
    }
    else
    {
        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No MIPI RX Error reported");
    }

    return ret;
    error:
        GMSL_LOG_ERROR("%s() for dev_addr=0x%x failed with error=%d\r\n", __FUNCTION__, dev->i2c_desc->slave_address, ret);

    return ret;
}

/**
 * @brief Check the PHY error details of the MAX96793 serializer
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param lp_err_status Pointer to store the PHY error status.
 * @param diag_err Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_link_diag_check_phy_low_power_state_error_status(struct gmsl_dev *dev, void *lp_err_status, bool *diag_err)
{
    int ret;
    struct max96793_link_diag_phy_low_power_err_status *lp_err_sts = (struct max96793_link_diag_phy_low_power_err_status *)lp_err_status;

    *diag_err = false;

    for(uint8_t phy = 0; phy < MAX96793_MIPI_PHY_COUNT; phy++)
    {
        ret = max96793_link_diag_check_low_power_state_error_details(dev, (enum max96793_phy)phy, &lp_err_sts->lp_err_sts_details[phy]);
        BREAK_ON_ERR(ret);

        if(lp_err_sts->lp_err_sts_details[phy].unrecgnizd_esc_cmd_rcvd_on_d0)
        {
            *diag_err = true;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Unrecognized escape command received on PHY %d D0", phy);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No unrecognized escape command received on PHY %d D0", phy);
        }

        if(lp_err_sts->lp_err_sts_details[phy].unrecgnizd_esc_cmd_rcvd_on_clk)
        {
            *diag_err = true;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Unrecognized escape command received on PHY %d CLK", phy);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No unrecognized escape command received on PHY %d CLK", phy);
        }

        if(lp_err_sts->lp_err_sts_details[phy].invalid_line_seq_on_d0)
        {
            *diag_err = true;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Invalid line sequence detected on PHY %d D0", phy);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No invalid line sequence detected on PHY %d D0", phy);
        }

        if(lp_err_sts->lp_err_sts_details[phy].invalid_line_seq_on_d1)
        {
            *diag_err = true;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Invalid line sequence detected on PHY %d D1", phy);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No invalid line sequence detected on PHY %d D1", phy);
        }

        if(lp_err_sts->lp_err_sts_details[phy].invalid_line_seq_on_clk)
        {
            *diag_err = true;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Invalid line sequence detected on PHY %d CLK", phy);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No invalid line sequence detected on PHY %d CLK", phy);
        }
    }

    return ret;
}

/**
 * @brief Check the MIPI PHY packet count status of MAX96793 serializer
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param mipi_pkt_cnt_err_status Pointer to store the MIPI PHY packet count status.
 * @param diag_err Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_csi_diag_check_mipi_phy_pkt_cnt_status(struct gmsl_dev *dev, void *mipi_pkt_cnt_err_status, bool *diag_err)
{
    int ret;
    bool pipe_enabled = false;
    uint32_t phy_pkt_cnt_sum = 0u;
    uint32_t csi1_pkt_cnt_sum = 0u;
    uint32_t tun_pkt_cnt_sum = 0u;
    uint32_t phy_clk_cnt_sum = 0u;
    uint8_t index;

    struct max96793_csi_diag_mipi_pkt_cnt_status *mipi_pkt_cnt_err_sts = (struct max96793_csi_diag_mipi_pkt_cnt_status *)mipi_pkt_cnt_err_status;

    *diag_err = false;

    do {
        ret = max96793_pipe_diag_get_pipe_enabled_status(dev, &pipe_enabled);
        BREAK_ON_ERR(ret);

        if(!pipe_enabled)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video Pipe is not enabled");
            break;
        }
        else
        {
            /* Do nothing */
        }


        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video Pipe is enabled");

        ret = max96793_pipe_diag_get_tunnel_mode_enabled_status(dev, &mipi_pkt_cnt_err_sts->tunnel_mode_enabled);
        BREAK_ON_ERR(ret);

        for (index = 0u; index < MAX96793_CSI_DIAG_NUM_OF_MIPI_PKT_READ; index++)
        {
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY1_PKT_CNT_MIPI_RX_EXT_EXT21_ADDR, PHY1_PKT_CNT_MIPI_RX_EXT_EXT21_MASK, (uint8_t *)&mipi_pkt_cnt_err_sts->phy_pkt_cnt[index]);

            phy_pkt_cnt_sum += mipi_pkt_cnt_err_sts->phy_pkt_cnt[index];

            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, CSI1_PKT_CNT_MIPI_RX_EXT_EXT22_ADDR, CSI1_PKT_CNT_MIPI_RX_EXT_EXT22_MASK, (uint8_t *)&mipi_pkt_cnt_err_sts->csi1_pkt_cnt[index]);

            csi1_pkt_cnt_sum += mipi_pkt_cnt_err_sts->csi1_pkt_cnt[index];

            if(mipi_pkt_cnt_err_sts->tunnel_mode_enabled)
            {
                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, TUN_PKT_CNT_MIPI_RX_EXT_EXT23_ADDR, TUN_PKT_CNT_MIPI_RX_EXT_EXT23_MASK, (uint8_t *)&mipi_pkt_cnt_err_sts->tun_pkt_cnt[index]);

                tun_pkt_cnt_sum += mipi_pkt_cnt_err_sts->tun_pkt_cnt[index];
            }
            else
            {
                mipi_pkt_cnt_err_sts->tun_pkt_cnt[index] = 0u;
            }

            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_CLK_CNT_MIPI_RX_EXT_EXT24_ADDR, PHY_CLK_CNT_MIPI_RX_EXT_EXT24_MASK, (uint8_t *)&mipi_pkt_cnt_err_sts->phy_clk_cnt[index]);

            phy_clk_cnt_sum += mipi_pkt_cnt_err_sts->phy_clk_cnt[index];

            no_os_mdelay(MAX96793_CSI_DIAG_MIPI_PKT_READ_DELAY); /* Delay to read the next packet count */
        }

        BREAK_ON_ERR(ret);

        if(phy_pkt_cnt_sum > 0u)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video data is running on. PHY packet counts are %d, %d, %d and %d", mipi_pkt_cnt_err_sts->phy_pkt_cnt[0], mipi_pkt_cnt_err_sts->phy_pkt_cnt[1], mipi_pkt_cnt_err_sts->phy_pkt_cnt[2], mipi_pkt_cnt_err_sts->phy_pkt_cnt[3]);
        }
        else
        {
            *diag_err = true;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Video data is off. PHY packet count is zero");
        }

        if(csi1_pkt_cnt_sum > 0u)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "CSI1 data is running on. CSI1 controller packet counts are %d, %d, %d and %d", mipi_pkt_cnt_err_sts->csi1_pkt_cnt[0], mipi_pkt_cnt_err_sts->csi1_pkt_cnt[1], mipi_pkt_cnt_err_sts->csi1_pkt_cnt[2], mipi_pkt_cnt_err_sts->csi1_pkt_cnt[3]);
        }
        else
        {
            *diag_err = true;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Video data is off. CSI1 controller packet count is zero");
        }

        if(mipi_pkt_cnt_err_sts->tunnel_mode_enabled)
        {
            if(tun_pkt_cnt_sum > 0u)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video data is running on tunnel mode. MIPI tunnel packet counts are %d, %d, %d and %d", mipi_pkt_cnt_err_sts->tun_pkt_cnt[0], mipi_pkt_cnt_err_sts->tun_pkt_cnt[1], mipi_pkt_cnt_err_sts->tun_pkt_cnt[2], mipi_pkt_cnt_err_sts->tun_pkt_cnt[3]);
            }
            else
            {
                *diag_err = true;
                GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Video data is off in tunnel mode. MIPI tunnel packet count is zero");
            }
        }
        else
        {
            /* Do nothing */
        }

        if(phy_clk_cnt_sum > 0u)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MIPI clock lane is running. MIPI clock rx counts are %d, %d, %d and %d", mipi_pkt_cnt_err_sts->phy_clk_cnt[0], mipi_pkt_cnt_err_sts->phy_clk_cnt[1], mipi_pkt_cnt_err_sts->phy_clk_cnt[2], mipi_pkt_cnt_err_sts->phy_clk_cnt[3]);
        }
        else
        {
            *diag_err = true;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "MIPI clock lane is off. MIPI clock rx count is zero");
        }

    } while(false);

    return ret;
}

/**
 * @brief Check the stream ID status of the MAX96793 serializer
 *
 * @param dev Pointer to the GMSL device structure.
 * @param stream_id_status Pointer to store the stream ID status.
 * @param diag_err Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_pipe_diag_check_stream_id(struct gmsl_dev *dev, void *stream_id_status, bool *diag_err)
{
    int ret;
    struct max96793_pipe_diag_stream_id_status *stream_id_sts = (struct max96793_pipe_diag_stream_id_status *)stream_id_status;

    *diag_err = false;

    do
    {
        ret = max96793_pipe_diag_get_pipe_enabled_status(dev, &stream_id_sts->pipe_enabled);
        BREAK_ON_ERR(ret);

        if(stream_id_sts->pipe_enabled)
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video Pipe is enabled");

            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, TX_STR_SEL_CFGV_VIDEO_Z_TX3_ADDR, TX_STR_SEL_CFGV_VIDEO_Z_TX3_MASK, &stream_id_sts->stream_id);
            
            /** @todo: Error check needs to be done after comparison with app/deserializer's
               data and diag_err needs to be updated*/
            /** @todo: Log message needs to be printed in error/info verbosity based on error condition.
               Printing in info verbosity for now. */
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe Stream ID is %d", stream_id_sts->stream_id);
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video Pipe is not enabled. Stream ID status not checked");
        }
    } while(false);

    return ret;
}

/**
 * @brief Check the Eye Open Monitor (EOM) status of the MAX96793 device
 *
 * @param dev Pointer to the GMSL device structure.
 * @param eom_status Pointer to store the Eye Open Monitor status.
 * @param diag_err Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_link_diag_check_eye_open_monitor_status(struct gmsl_dev *dev, void* eom_status, bool *diag_err)
{
    int ret;
    struct max96793_link_diag_eom_status *eom_sts = (struct max96793_link_diag_eom_status *)eom_status;

    *diag_err = false;

    do
    {
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, EOM_ERR_FLAG_A_TCTRL_INTR5_ADDR, EOM_ERR_FLAG_A_TCTRL_INTR5_MASK, (uint8_t *)&eom_sts->eom_error);

        if(eom_sts->eom_error)
        {
            *diag_err = true;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "EOM Error reported");
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No EOM Error reported");
        }
    } while(false);

    return ret;
}

/**
 * @brief Check the remote error flag status of the MAX96793 device.
 *
 * @param dev            Pointer to the GMSL device structure.
 * @param rem_err_status Pointer to the structure to store the remote error status.
 * @param diag_err       Pointer to a boolean variable to store the diagnostic error status.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_dev_diag_check_remote_error_status(struct gmsl_dev *dev, void* rem_err_status, bool *diag_err)
{
    int ret;
    struct max96793_dev_diag_rem_err_flg_status *rem_err_sts = (struct max96793_dev_diag_rem_err_flg_status *)rem_err_status;
    
    *diag_err = false;

    do
    {
        /* Check if Remote Control Channel is Disabled */
        ret = max96793_dev_get_remote_cc_disabled_status(dev, &rem_err_sts->is_remote_cc_disabled);
        BREAK_ON_ERR(ret);

        if(!rem_err_sts->is_remote_cc_disabled)
        {
            /* Check Remote Error Flag */
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, REM_ERR_FLAG_TCTRL_INTR3_ADDR, REM_ERR_FLAG_TCTRL_INTR3_MASK, (uint8_t *)&rem_err_sts->remote_error);

            if(rem_err_sts->remote_error)
            {
                *diag_err = true;
                GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Remote Error detected");
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Remote Error detected");
            }
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Remote Control Channel is disabled. Remote Error Flag status Not Checked");
        }
    } while(false);

    return ret;
}

/**
 * @brief Check if FEC (Forward Error Correction) is enabled and running on the MAX96793 device
 *
 * @param dev Pointer to the GMSL device structure.
 * @param fec_active_status Pointer to store the FEC active status.
 * @param diag_err Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_csi_diag_check_fec_status(struct gmsl_dev *dev, void *fec_active_status, bool *diag_err)
{
    int ret;
    struct max96793_csi_diag_fec_active_status *fec_active_sts = (struct max96793_csi_diag_fec_active_status *)fec_active_status;

    *diag_err = false;

    do
    {
        /* Check if Remote Control Channel is Disabled */
        ret = max96793_dev_get_remote_cc_disabled_status(dev, &fec_active_sts->is_remote_cc_disabled);
        BREAK_ON_ERR(ret);

        if(!fec_active_sts->is_remote_cc_disabled)
        {
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, RX_FEC_EN_GMSL_TX0_ADDR, RX_FEC_EN_GMSL_TX0_MASK, (uint8_t *)&fec_active_sts->is_fec_enabled);

            if(fec_active_sts->is_fec_enabled)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "FEC is enabled");

                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, TX_FEC_ACTIVE_GMSL_TX3_ADDR, TX_FEC_ACTIVE_GMSL_TX3_MASK, (uint8_t *)&fec_active_sts->is_fec_active);

                /** @todo: Error condition to be updated in diag_err after comparison with input video compression status from app data */
                /** @todo: Log verbosity needs to be updated based on error condition. Printing at info verbosity for now */
                if(fec_active_sts->is_fec_active)
                {
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "FEC is active");
                }
                else
                {
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "FEC is not active");
                }
            }
            else
            {
                GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "FEC is not enabled. FEC status Not Checked");
            }
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Remote Control Channel is disabled. FEC status Not Checked");
        }        
    } while(false);

    return ret;
}

/**
 * @brief Check the part configuration status of the MAX96793 device
 * 
 * @param dev Pointer to the GMSL device structure.
 * @param part_config_status Pointer to store the part configuration status.
 * @param diag_err Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 * 
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_dev_diag_check_part_config_status(struct gmsl_dev *dev, void *part_config_status, bool *diag_err)
{
    int ret;
    *diag_err = false;

    struct max96793_dev_diag_part_config_status *part_config_sts = (struct max96793_dev_diag_part_config_status *)part_config_status;

    /** @todo: Error check needs to be done after comparison with app's configuration data and diag_err 
     * and debug log verbosity needs to be updated for all fields of part_config_sts */
    /* Note: Few parameters whose values are fixed/derived from domain logic are checked and diag_err and debug log verbosity are updated accordingly. */

    do
    {
        ret = max96793_pipe_diag_check_config_status(dev, &part_config_sts->pipe_config_status, diag_err);
        BREAK_ON_ERR(ret);

        ret = max96793_csi_diag_check_config_status(dev, &part_config_sts->csi_config_status, diag_err);

    } while(false);

    return ret;
}

/**
 * @brief Get the link lock status of the MAX96793 serializer.
 *
 * @param dev             - Device handle.
 * @param locked          - Link lock status.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_link_diag_get_link_lock_status(struct gmsl_dev *dev, bool *locked)
{
    *locked = false;
    /* Read Link Lock */
    return gmsl_reg_read(dev->i2c_desc, LOCKED_TCTRL_CTRL3_ADDR, LOCKED_TCTRL_CTRL3_MASK, (uint8_t *)locked);
}

/**
 * @brief Get the status of Error flag (ERRB) in the MAX96793 serializer.
 *
 * @param dev             - Device handle.
 * @param errb            - ERRB set/not set.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96793_dev_diag_get_errb_status(struct gmsl_dev *dev, bool *errb)
{
    *errb = false;
    /* Read Error flag */
    return gmsl_reg_read(dev->i2c_desc, ERROR_TCTRL_CTRL3_ADDR, ERROR_TCTRL_CTRL3_MASK, (uint8_t *)errb);
}
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */