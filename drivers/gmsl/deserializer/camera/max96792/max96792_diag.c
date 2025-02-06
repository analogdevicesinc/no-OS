/***************************************************************************/ /**
    *   @file   max96792_diag.c
    *   @brief  Diagnostics Driver Implementation of max96792 Deserializer.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "max96792.h"
#include "max96792_diag.h"
#include "max96792_regs.h"
#include "gmsl_cam_des.h"
#include "gmsl_dbg.h"
#include "gmsl_reg_access.h"
#include "no_os_error.h"
#include "no_os_delay.h"
/** \addtogroup ADI_GMSL_CAM_DESERIALIZER GMSL Camera Deserializers
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_DES3 MAX96792
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_DES3_DIAG_API Diagnostic APIs
 *  @{
 */
/******************************************************************************/
/************************* Global Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/************************* Functions Definition *******************************/
/******************************************************************************/

/**
 * @brief Check if the video pipes are enabled for the MAX96792 device.
 *
 * This function reads the video pipe enable register of the MAX96792 device to determine
 * if the video pipes are enabled. It uses the I2C descriptor of the device to perform
 * the register read operation.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param pipe_en Pointer to a structure where the video pipe enable status will be stored.
 * @return int Returns 0 on success, or a negative error code on failure.
 */

int max96792_check_video_pipe_enable(struct gmsl_dev *dev, struct max96792_pipe_enabled_status *pipe_en)
{
    int ret = 0;
    const uint8_t pipe_en_mask[MAX96792_NUM_OF_VIDEO_PIPES] = {VID_EN_Y_DEV_REG2_MASK, VID_EN_Z_DEV_REG2_MASK};
    uint8_t pipe_en_reg = 0;

    ret = gmsl_reg_read(dev->i2c_desc, VID_EN_Y_DEV_REG2_ADDR, MAX96792_MASK_TO_RW_ALL_MASK, &pipe_en_reg);
    if (ret == 0u)
    {
        for (uint8_t pipe_idx = 0; pipe_idx < MAX96792_NUM_OF_VIDEO_PIPES; pipe_idx++)
        {
            pipe_en->pipe_enabled[pipe_idx] = ((pipe_en_reg & pipe_en_mask[pipe_idx]) != 0u);
        }
    }
    else
    {
        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Failed to read Video Pipe Enable Register");
    }

    return ret;
}

/**
 * @brief Check if the GMSL links are enabled for the MAX96792 device.
 *
 * This function reads the GMSL link enable register of the MAX96792 device to determine
 * if the GMSL links are enabled. It uses the I2C descriptor of the device to perform
 * the register read operation.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param link_en Pointer to a structure where the GMSL link enable status will be stored.
 * @return int Returns 0 on success, or a negative error code on failure.
 */

int max96792_check_gmsl_link_enable(struct gmsl_dev *dev, struct max96792_link_enabled_status *link_en)
{
    int ret = 0;
    const uint8_t link_en_mask[MAX96792_NUM_OF_GMSL_LINKS] = {LINK_EN_A_GMSL1_COMMON_GMSL1_EN_MASK, LINK_EN_B_GMSL1_COMMON_GMSL1_EN_MASK};
    uint8_t link_status = 0;

    ret = gmsl_reg_read(dev->i2c_desc, GMSL1_COMMON_GMSL1_EN_ADDR, MAX96792_MASK_TO_RW_ALL_MASK, &link_status);
    if (ret == 0u)
    {
        for (uint8_t link_idx = 0; link_idx < MAX96792_NUM_OF_GMSL_LINKS; link_idx++)
        {
            link_en->link_enabled[link_idx] = (link_status & link_en_mask[link_idx]) != 0u;
        }
    }
    else
    {
        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Failed to read GMSL Link Enable Register");
    }

    return ret;
}

/**
 * @brief Check the video lock status of the max96792 deserializer.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param video_lock_status Pointer to the structure where the video lock status for each pipe will be stored.
 * @param diag_err Pointer to a boolean that will be set to true if a diagnostic error is detected, otherwise false.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_pipe_diag_check_video_lock_status(struct gmsl_dev *dev, void *video_lock_status, bool *diag_err)
{
    int ret = 0;
    struct max96792_pipe_diag_video_lock_status *video_lock_stat = (struct max96792_pipe_diag_video_lock_status *)video_lock_status;
    uint8_t pipe_index = 0;
#ifdef _DEBUG
    const char pipe_name[MAX96792_NUM_OF_VIDEO_PIPES] = {'Y', 'Z'};
#endif
    uint16_t base_reg = VID_LOCK_VID_RX_Y_VIDEO_RX8_ADDR;
    uint16_t offset = 0;
    uint8_t mask = VID_LOCK_VID_RX_Z_VIDEO_RX8_MASK;
    uint8_t video_lock = 0;
    struct max96792_pipe_enabled_status pipe_en;
    *diag_err = false;

    do
    {

        ret = max96792_check_video_pipe_enable(dev, &pipe_en);
        BREAK_ON_ERR(ret);

        for (pipe_index = 0; pipe_index < MAX96792_NUM_OF_VIDEO_PIPES; pipe_index++)
        {
            /*Check if pipe is enabled*/

            if (pipe_en.pipe_enabled[pipe_index])
            {
                offset = VIDEO_PIPE_REG_OFFSET_MULT * pipe_index;
                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_reg + offset), mask, &video_lock);
                if (video_lock == 0u)
                {
                    video_lock_stat->video_lock[pipe_index] = false;
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c Video Not Locked", pipe_name[pipe_index]);
                    *diag_err = true;
                }
                else
                {
                    video_lock_stat->video_lock[pipe_index] = true;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c Video Locked", pipe_name[pipe_index]);
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c is not enabled, Video Lock is not checked", pipe_name[pipe_index]);
            }
        }
    } while (false);

    return ret;
}

/**
 * @brief Check the link lock status of the max96792 deserializer.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param link_lock_status Pointer to the structure where the link lock status will be stored.
 * @param diag_err Pointer to a boolean that will be set to true if a diagnostic error is detected.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_link_diag_check_link_lock_status(struct gmsl_dev *dev, void *link_lock_status, bool *diag_err)
{
    int ret = 0;
    struct max96792_link_diag_link_lock_status *link_lock_stat = (struct max96792_link_diag_link_lock_status *)link_lock_status;
    uint8_t link_lock_a = 0;
    uint8_t link_lock_b = 0;
    struct max96792_link_enabled_status link_en;
    *diag_err = false;

    do
    {
        /* Check if link A is enabled */
        ret = max96792_check_gmsl_link_enable(dev, &link_en);
        BREAK_ON_ERR(ret);

        if (link_en.link_enabled[MAX96792_LINK_A])
        {

            /* Read Link Lock for link A */
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, DEV_CTRL3_LINK_A_LOCK_ADDR, DEV_CTRL3_LINK_A_LOCK_MASK, &link_lock_a);
            if (link_lock_a != 0)
            {
                link_lock_stat->link_lock_a = true;
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Link A is Locked");
            }
            else
            {
                link_lock_stat->link_lock_a = false;
                *diag_err = true;
                GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Link A is not Locked");
            }
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Link A is not enabled, Link Lock is not checked");
        }

        /* Check if link B is enabled */
        if (link_en.link_enabled[MAX96792_LINK_B])
        {

            /* Read Link Lock for link B */
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, DEV_CTRL9_LINK_B_LOCK_ADDR, DEV_CTRL9_LINK_B_LOCK_MASK, &link_lock_b);
            if (link_lock_b != 0)
            {
                link_lock_stat->link_lock_b = true;
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Link B is Locked");
            }
            else
            {
                link_lock_stat->link_lock_b = false;
                *diag_err = true;
                GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Link B is not Locked");
            }
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Link B is not enabled, Link Lock is not checked");
        }

    } while (false);

    return ret;
}

/**
 * @brief Retrieve the device ID of the max96792 deserializer and check for diagnostic errors.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param dev_id_status Pointer to the structure where the device ID status will be stored.
 * @param diag_err Pointer to a boolean that will be set to true if a diagnostic error occurs.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_dev_diag_get_device_id(struct gmsl_dev *dev, void *dev_id_status, bool *diag_err)
{
    int ret = 0;
    struct max96792_dev_diag_device_id_status *dev_id_sts = (struct max96792_dev_diag_device_id_status *)dev_id_status;
    *diag_err = false;
    dev_id_sts->dev_id = 0xFFU;
    do
    {
        /* Read Device ID */
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, DEV_ID_DEV_REG13_ADDR, DEV_ID_DEV_REG13_MASK, &dev_id_sts->dev_id);
        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Device ID: %u", dev_id_sts->dev_id);

    } while (false);
    return ret;
}

/**
 * @brief Retrieve the device revision status of the max96792 deserializer.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param dev_revision_status Pointer to the structure where the device revision status will be stored.
 * @param diag_err Pointer to a boolean that will be set to true if a diagnostic error occurs.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_dev_diag_get_device_revision(struct gmsl_dev *dev, void *dev_revision_status, bool *diag_err)
{
    int ret = 0;
    struct max96792_dev_diag_dev_rev_status *dev_rev_sts = (struct max96792_dev_diag_dev_rev_status *)dev_revision_status;
    *diag_err = false;
    dev_rev_sts->dev_rev = 0xFFU;
    do
    {
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, DEV_REV_DEV_REG14_ADDR, DEV_REV_DEV_REG14_MASK, &dev_rev_sts->dev_rev);
        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Device Revision: %d", dev_rev_sts->dev_rev);
    } while (false);

    return ret;
}

/**
 * @brief Check decode error status for max96792 link diagnostics.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param decode_err_status Pointer to the structure to store the decode error status.
 * @param diag_err Pointer to a boolean to store the diagnostic error status.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_link_diag_check_decode_error_status(struct gmsl_dev *dev, void *decode_err_status, bool *diag_err)
{
    int ret = 0;
    uint8_t link_index = 0;
    uint16_t base_addr = DEC_ERR_FLAG_A_TCTRL_INTR3_ADDR;
    const uint8_t mask[MAX96792_NUM_OF_GMSL_LINKS] = {DEC_ERR_FLAG_A_TCTRL_INTR3_MASK, DEC_ERR_FLAG_B_TCTRL_INTR3_MASK};
#ifdef _DEBUG
    const char link_name[MAX96792_NUM_OF_GMSL_LINKS] = {'A', 'B'};
#endif
    uint8_t link_err_flag = 0;
    struct max96792_link_enabled_status link_en;
    struct max96792_pipe_diag_decode_err_status *decode_err_stat = (struct max96792_pipe_diag_decode_err_status *)decode_err_status;

    *diag_err = false;

    do
    {

        ret = max96792_check_gmsl_link_enable(dev, &link_en);
        BREAK_ON_ERR(ret);

        for (link_index = 0; link_index < MAX96792_NUM_OF_GMSL_LINKS; link_index++)
        {
            /* Check if link is enabled */
            if (link_en.link_enabled[link_index])
            {

                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr), mask[link_index], (uint8_t *)&link_err_flag);
                if (link_err_flag != 0u)
                {
                    decode_err_stat->decode_err_link_flag[link_index] = true;
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + link_index + 7u), MAX96792_MASK_TO_RW_ALL_MASK, (uint8_t *)&decode_err_stat->decode_err_cnt[link_index]);
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Link %c decode error detected with error count %u", link_name[link_index], decode_err_stat->decode_err_cnt[link_index]);
                    *diag_err = true;
                }
                else
                {
                    decode_err_stat->decode_err_link_flag[link_index] = false;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Link %c decode error not detected", link_name[link_index]);
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Link %c is not enabled, Decode Error is not checked", link_name[link_index]);
            }
        }
    } while (false);

    return ret;
}

/**
 * @brief Check the idle error status of the max96792 link.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param idle_err_status Pointer to the structure to store idle error status.
 * @param diag_err Pointer to a boolean to store the diagnostic error status.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_link_diag_check_idle_error_status(struct gmsl_dev *dev, void *idle_err_status, bool *diag_err)
{
    int ret = 0;
    uint16_t base_addr = IDLE_ERR_FLAG_TCTRL_INTR3_ADDR;
    uint16_t offset = 0;
    const uint16_t count_offset[MAX96792_NUM_OF_GMSL_LINKS] = {MAX96792_IDLE_ERROR_COUNT_REG_OFFSET_A, MAX96792_IDLE_ERROR_COUNT_REG_OFFSET_B};
    const uint8_t mask[MAX96792_NUM_OF_GMSL_LINKS] = {IDLE_ERR_FLAG_TCTRL_INTR3_MASK, IDLE_ERR_FLAG_B_TCTRL_EXT_INTR11_MASK};
#ifdef _DEBUG
    const char link_name[MAX96792_NUM_OF_GMSL_LINKS] = {'A', 'B'};
#endif
    struct max96792_pipe_diag_idle_error_status *idle_err_stat = (struct max96792_pipe_diag_idle_error_status *)idle_err_status;
    uint8_t idle_status = 0;
    struct max96792_link_enabled_status link_en;
    *diag_err = false;

    do
    {
        ret = max96792_check_gmsl_link_enable(dev, &link_en);
        BREAK_ON_ERR(ret);

        for (uint8_t link_index = 0; link_index < MAX96792_NUM_OF_GMSL_LINKS; link_index++)
        {
            /* Check if link is enabled */
            if (link_en.link_enabled[link_index])
            {
                offset = link_index * MAX96792_LINK_IDLE_REG_OFFSET_MULTIPLIER;
                idle_err_stat->idle_error_count[link_index] = 0u;

                /* Read the Idle Error Flag*/
                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, base_addr + offset, mask[link_index], &idle_status);

                if (idle_status != 0)
                {
                    idle_err_stat->idle_error_detected[link_index] = true;
                    /* Check Idle Error Count for Link A*/
                    *diag_err = true;
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, base_addr + offset + count_offset[link_index], MAX96792_MASK_TO_RW_ALL_MASK, (uint8_t *)&idle_err_stat->idle_error_count[link_index]);
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Idle Error Reported for Link %c with Error Count: %u", link_name[link_index], idle_err_stat->idle_error_count[link_index]);
                }
                else
                {
                    idle_err_stat->idle_error_detected[link_index] = false;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Idle Error reported for Link %c", link_name[link_index]);
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Link %c is not enabled, Idle Error is not checked", link_name[link_index]);
            }
        }

    } while (false);
    return ret;
}

/**
 * @brief Check the MIPI status of the max96792 deserializer.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param mipi_status Pointer to the structure where MIPI status will be stored.
 * @param diag_err Pointer to a boolean that will be set to true if a diagnostic error is detected.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_csi_diag_check_mipi_status(struct gmsl_dev *dev, void *mipi_status, bool *diag_err)
{
    int ret = 0;
    uint16_t csi2_pkt_cnt_sum[MAX96792_NUM_OF_CSI_CONTROLLERS] = {0u};
    uint16_t phy_pkt_cnt_sum[MAX96792_NUM_OF_MIPI_PHY] = {0u};
    uint16_t csi_base_address = CSI2_TX1_PKT_CNT_MIPI_PHY_MIPI_PHY18_ADDR;
    uint16_t mipi_phy_base_address = PHY1_PKT_CNT_MIPI_PHY_MIPI_PHY20_ADDR;
    uint16_t mipi_phy_offset = 0;
    const uint8_t csi_mask[MAX96792_NUM_OF_CSI_CONTROLLERS] = {CSI2_TX1_PKT_CNT_MIPI_PHY_MIPI_PHY18_MASK, CSI2_TX2_PKT_CNT_MIPI_PHY_MIPI_PHY18_MASK};
#ifdef _DEBUG
    const char csi_controller_index_char[MAX96792_NUM_OF_CSI_CONTROLLERS] = {'1', '2'};
    const char mipi_phy_index_char[MAX96792_NUM_OF_MIPI_PHY] = {'0', '1', '2', '3'};
#endif
    uint8_t mipi_phy_index = 0;
    struct max96792_csi_mipi_status *mipi_stat = (struct max96792_csi_mipi_status *)mipi_status;
    uint8_t mipi_out_enabled = 0;
    *diag_err = false;

    do
    {
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, CSI_OUT_EN_BACKTOP_BACKTOP12_ADDR, CSI_OUT_EN_BACKTOP_BACKTOP12_MASK, &mipi_out_enabled);

        if (mipi_out_enabled == 0)
        {
            mipi_stat->mipi_out_enabled = false;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "MIPI CSI output is not enabled");
            *diag_err = true;
        }
        else
        {
            mipi_stat->mipi_out_enabled = true;
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MIPI CSI output is enabled");
        }

        for (uint8_t csi_index = 0u; csi_index < MAX96792_NUM_OF_CSI_CONTROLLERS; csi_index++)
        {
            mipi_phy_offset = csi_index;

            for (uint8_t pkt_index = 0u; pkt_index < MAX96792_NUM_OF_MIPI_PKT_READ; pkt_index++)
            {
                mipi_phy_index = 2 * csi_index;

                /* Check for Packet Count sent out of CSI2 Controllers */
                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, csi_base_address, csi_mask[csi_index], &mipi_stat->csi2_pkt_cnt[csi_index][pkt_index]);
                csi2_pkt_cnt_sum[csi_index] += mipi_stat->csi2_pkt_cnt[csi_index][pkt_index];

                /* Check for Packet Count sent out of corrosponding PHY-1 */

                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (mipi_phy_base_address + mipi_phy_offset), PHY0_PKT_CNT_MIPI_PHY_MIPI_PHY20_MASK, &mipi_stat->phy_pkt_cnt[mipi_phy_index][pkt_index]);
                phy_pkt_cnt_sum[mipi_phy_index] += mipi_stat->phy_pkt_cnt[mipi_phy_index][pkt_index];

                mipi_phy_index = (2 * csi_index) + 1;
                /* Check for Packet Count sent out of corrosponding PHY-2 */

                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (mipi_phy_base_address + mipi_phy_offset), PHY1_PKT_CNT_MIPI_PHY_MIPI_PHY20_MASK, &mipi_stat->phy_pkt_cnt[mipi_phy_index][pkt_index]);
                phy_pkt_cnt_sum[mipi_phy_index] += mipi_stat->phy_pkt_cnt[mipi_phy_index][pkt_index];

                no_os_mdelay(10); /* Delay to read the next packet count */
            }
            BREAK_ON_ERR(ret);
            if (csi2_pkt_cnt_sum[csi_index] == 0u)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Packet not sent out of CSI2 Controller-%c. Packet Counts: %d, %d, %d, %d", csi_controller_index_char[csi_index], mipi_stat->csi2_pkt_cnt[csi_index][0], mipi_stat->csi2_pkt_cnt[csi_index][1], mipi_stat->csi2_pkt_cnt[csi_index][2], mipi_stat->csi2_pkt_cnt[csi_index][3]);
                *diag_err = true;
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Packet sent out of CSI2 Controller-%c. Packet Counts: %d, %d, %d, %d", csi_controller_index_char[csi_index], mipi_stat->csi2_pkt_cnt[csi_index][0], mipi_stat->csi2_pkt_cnt[csi_index][1], mipi_stat->csi2_pkt_cnt[csi_index][2], mipi_stat->csi2_pkt_cnt[csi_index][3]);
            }
        }

        BREAK_ON_ERR(ret);

        for (uint8_t pkt_count = 0; pkt_count < MAX96792_NUM_OF_MIPI_PHY; pkt_count++)
        {
            if (phy_pkt_cnt_sum[pkt_count] == 0u)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Packet not sent out of MIPI PHY-%c. Packet Counts: %d, %d, %d, %d", mipi_phy_index_char[pkt_count], mipi_stat->phy_pkt_cnt[pkt_count][0], mipi_stat->phy_pkt_cnt[pkt_count][1], mipi_stat->phy_pkt_cnt[pkt_count][2], mipi_stat->phy_pkt_cnt[pkt_count][3]);
                *diag_err = true;
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Packet sent out of MIPI PHY-%c. Packet Counts: %d, %d, %d, %d", mipi_phy_index_char[pkt_count], mipi_stat->phy_pkt_cnt[pkt_count][0], mipi_stat->phy_pkt_cnt[pkt_count][1], mipi_stat->phy_pkt_cnt[pkt_count][2], mipi_stat->phy_pkt_cnt[pkt_count][3]);
            }
        }

    } while (false);

    return ret;
}

/**
 * @brief Get the line fault interrupt status for the max96792 deserializer.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param line_fault_int Pointer to a variable where the line fault interrupt status will be stored.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_link_diag_get_line_fault_int_status(struct gmsl_dev *dev, uint8_t *line_fault_int)
{
    return gmsl_reg_read(dev->i2c_desc, LFLT_INT_TCTRL_INTR3_ADDR, LFLT_INT_TCTRL_INTR3_MASK, line_fault_int);
}

/**
 * @brief Get the enabled status of line fault detection for the max96792 deserializer.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param line_fault_enable_status Pointer to a variable where the line fault enable status will be stored.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_link_diag_get_line_fault_enabled_status(struct gmsl_dev *dev, uint8_t *line_fault_enable_status)
{
    return gmsl_reg_read(dev->i2c_desc, PU_LF0_DEV_REG5_ADDR, (PU_LF0_DEV_REG5_MASK | PU_LF1_DEV_REG5_MASK | PU_LF2_DEV_REG5_MASK | PU_LF3_DEV_REG5_MASK), line_fault_enable_status);
}

/**
 * @brief Get the link diagnostic status for a specified LMN.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param lmn The LMN link to get the diagnostic status for.
 * @param line_fault_status Pointer to store the retrieved diagnostic status.
 *
 * @return 0 on success, negative error code otherwise.
 */
int max96792_link_diag_get_lmn_status(struct gmsl_dev *dev, enum max96792_link_lmn lmn, enum max96792_link_diag_lmn_status *line_fault_status)
{
    int ret;

    *line_fault_status = MAX96792_LINK_DIAG_LMN_NORMAL;

    switch (lmn)
    {

    case MAX96792_LINK_LMN_0:
        ret = gmsl_reg_read(dev->i2c_desc, LF_0_DEV_REG26_ADDR, LF_0_DEV_REG26_MASK, (uint8_t *)line_fault_status);
        break;

    case MAX96792_LINK_LMN_1:
        ret = gmsl_reg_read(dev->i2c_desc, LF_1_DEV_REG26_ADDR, LF_1_DEV_REG26_MASK, (uint8_t *)line_fault_status);
        break;

    case MAX96792_LINK_LMN_2:
        ret = gmsl_reg_read(dev->i2c_desc, LF_2_DEV_REG27_ADDR, LF_2_DEV_REG27_MASK, (uint8_t *)line_fault_status);
        break;

    case MAX96792_LINK_LMN_3:
        ret = gmsl_reg_read(dev->i2c_desc, LF_3_DEV_REG27_ADDR, LF_3_DEV_REG27_MASK, (uint8_t *)line_fault_status);
        break;

    default:
        GMSL_LOG_ERROR("MAX96792: LMN %d not supported in serializer (0x%x)", lmn, dev->i2c_desc->slave_address);
        ret = -ENOTSUP;
        break;
    }
    return ret;
}

/**
 * @brief Check the line fault status of the max96792 deserializer.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param line_fault_status Pointer to the structure where line fault status details will be stored.
 * @param diag_err Pointer to a boolean flag that will be set to true if any diagnostic errors are detected.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_link_diag_check_line_fault_status(struct gmsl_dev *dev, void *line_fault_status, bool *diag_err)
{
    int ret = 0;
    uint8_t nIdx = 0u;
    uint8_t line_fault_enabled = 0u;
    struct max96792_link_diag_line_fault_details *line_fault_stat = (struct max96792_link_diag_line_fault_details *)line_fault_status;
    *diag_err = false;

    do
    {
        ret = max96792_link_diag_get_line_fault_int_status(dev, (uint8_t *)&line_fault_stat->lf_int);
        BREAK_ON_ERR(ret);

        if (line_fault_stat->lf_int)
        {
            *diag_err = true;
            ret = max96792_link_diag_get_line_fault_enabled_status(dev, &line_fault_enabled);
            BREAK_ON_ERR(ret);

            for (nIdx = 0u; nIdx < MAX96792_NUM_OF_LINE_FAULT_MONITORS; nIdx++)
            {
                if ((line_fault_enabled & (1U << nIdx)) != 0u)
                {
                    ret = max96792_link_diag_get_lmn_status(dev, (enum max96792_link_lmn)nIdx, (enum max96792_link_diag_lmn_status *)&line_fault_stat->lmn_err_status[nIdx]);
                    BREAK_ON_ERR(ret);
#ifdef _DEBUG
                    switch (line_fault_stat->lmn_err_status[nIdx])
                    {
                    case MAX96792_LINK_DIAG_LMN_SHORT_TO_BAT:
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Monitor %d indicate Short-To-Battery fault", nIdx);
                        break;
                    case MAX96792_LINK_DIAG_LMN_SHORT_TO_GND:
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Monitor %d indicate Short-To-Ground fault", nIdx);
                        break;
                    case MAX96792_LINK_DIAG_LMN_NORMAL:
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Monitor %d status is Normal", nIdx);
                        break;
                    case MAX96792_LINK_DIAG_LMN_LINE_OPEN:
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Monitor %d indicate Line-Open fault", nIdx);
                        break;
                    case MAX96792_LINK_DIAG_LMN_LINE_TO_LINE_SHORT:
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Line Fault Monitor %d indicate Line-to-Line Short fault", nIdx);
                        break;

                    default:
                        break;
                    }
#endif
                }
            }
            BREAK_ON_ERR(ret);
        }

    } while (false);

    return ret;
}

/**
 * @brief Check the part configuration of the max96792 device.
 *
 * This function reads various configuration registers of the max96792 device
 * and logs the diagnostic information. It checks the video mode, stream ID,
 * source-destination virtual channel and data type map information, lane count,
 * lane mapping, lane polarity mapping, MIPI output enable status, MIPI lane
 * frequency, and PLL functional reset status.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param part_cfg_status Pointer to the part configuration status structure.
 * @param diag_err Pointer to a boolean variable to store the diagnostic error status.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_dev_diag_check_part_config(struct gmsl_dev *dev, void *part_cfg_status, bool *diag_err)
{
    int ret = 0;
    uint8_t map_en_lb = 0u;
    uint8_t map_en_hb = 0u;
    uint8_t src_dst_count = 0u;
    uint16_t src_dst_map_enabled = 0U;
    uint8_t src_map = 0u;
    uint8_t dst_map = 0u;
    uint8_t dphy_dst = 0u;
    uint16_t dphy_dst_reg = MAP_DPHY_DEST_0_MIPI_TX_1_MIPI_TX45_ADDR;
    uint8_t alt_mem_map_en = 0u;
    uint8_t phy_lane_polarity_map[MAX96792_NUM_OF_MIPI_PHY];
#ifdef _DEBUG
    const char video_pipe_char[MAX96792_NUM_OF_VIDEO_PIPES] = {'Y', 'Z'};
#endif
    struct max96792_dev_diag_part_cfg_status *part_cfg_stat = (struct max96792_dev_diag_part_cfg_status *)part_cfg_status;
    uint16_t base_addr = TUN_EN_MIPI_TX_1_MIPI_TX52_ADDR;
    uint16_t offset;
    uint8_t stream_id_mask[MAX96792_NUM_OF_VIDEO_PIPES] = {VIDEO_PIPE_SEL_Y_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_MASK, VIDEO_PIPE_SEL_Z_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_MASK};
    uint8_t polarity_mask[MAX96792_NUM_OF_CSI_CONTROLLERS] = {PHY0_POL_MAP_MIPI_PHY_MIPI_PHY5_MASK, PHY1_POL_MAP_MIPI_PHY_MIPI_PHY5_MASK};
    uint8_t polarity_mask_index = 0;
    uint8_t pll_func_reset = 0u;
    uint8_t mipi_out_enabled = 0u;
    uint8_t lane_count = 0;
    uint8_t video_mode = 0;
    uint8_t stream_id = 0;
    struct max96792_pipe_enabled_status pipe_en;
    *diag_err = false;

    do
    {
        ret = max96792_check_video_pipe_enable(dev, &pipe_en);
        BREAK_ON_ERR(ret);

        for (uint8_t pipe_index = 0; pipe_index < MAX96792_NUM_OF_VIDEO_PIPES; pipe_index++)
        {
            /*First check if pipe is enabled*/

            if (pipe_en.pipe_enabled[pipe_index])
            {
                offset = MIPI_REG_OFFSET_MULT * pipe_index;

                /* Read Video Mode*/
                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), TUN_EN_MIPI_TX_1_MIPI_TX52_MASK, (uint8_t *)&video_mode);
                part_cfg_stat->mode[pipe_index] = video_mode;

                if (part_cfg_stat->mode[pipe_index] == MAX96792_DEV_CSI_PIXEL_MODE)
                {
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "CSI Video Mode for pipe %c: Pixel Mode", video_pipe_char[pipe_index]);
                }
                else if (part_cfg_stat->mode[pipe_index] == MAX96792_DEV_CSI_TUNNEL_MODE)
                {
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "CSI Video Mode for pipe %c: Tunnel Mode", video_pipe_char[pipe_index]);
                }
                else
                {
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Invalid Video Mode for pipe %c", video_pipe_char[pipe_index]);
                    *diag_err = true;
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c is not enabled, Video Mode is not checked", video_pipe_char[pipe_index]);
            }
        }
        BREAK_ON_ERR(ret);
        /* Read Stream ID */
        base_addr = VIDEO_PIPE_SEL_Y_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_ADDR;
        offset = 0;

        for (uint8_t pipe_index = 0; pipe_index < MAX96792_NUM_OF_VIDEO_PIPES; pipe_index++)
        {
            if (pipe_en.pipe_enabled[pipe_index])
            {
                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), stream_id_mask[pipe_index], (uint8_t *)&part_cfg_stat->stream_id[pipe_index]);
                stream_id = (part_cfg_stat->stream_id[pipe_index]) & MAX96792_STREAM_ID_EXTRACTION_MASK;
                part_cfg_stat->stream_id[pipe_index] = stream_id;
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Stream ID for Pipe %c: %d", video_pipe_char[pipe_index], stream_id);
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c is not enabled, Stream ID is not checked", video_pipe_char[pipe_index]);
            }
        }
        BREAK_ON_ERR(ret);

        for (uint8_t pipe_index = 0; pipe_index < MAX96792_NUM_OF_VIDEO_PIPES; pipe_index++)
        {
            if (pipe_en.pipe_enabled[pipe_index])
            {
                if (part_cfg_stat->mode[pipe_index] == MAX96792_DEV_CSI_PIXEL_MODE) /* For Pixel Mode - Read Source Destination Virtual Channel and Data Type Map information */
                {
                    base_addr = MAP_EN_L_MIPI_TX_1_MIPI_TX11_ADDR;
                    offset = MIPI_REG_OFFSET_MULT * pipe_index;

                    /* Read SRC DST REMAP Lower Byte */
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), MAP_EN_L_MIPI_TX_1_MIPI_TX11_MASK, &map_en_lb);

                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MAP_EN_L Pipe %c: 0x%x", video_pipe_char[pipe_index], map_en_lb);

                    offset++;

                    /* Read SRC DST REMAP Higher Byte */
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), MAP_EN_L_MIPI_TX_1_MIPI_TX11_MASK, &map_en_hb);

                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MAP_EN_H Pipe %c: 0x%x", video_pipe_char[pipe_index], map_en_hb);

                    src_dst_map_enabled = ((uint16_t)map_en_hb << 8U) | map_en_lb;

                    base_addr = MAP_SRC_0_MIPI_TX_1_MIPI_TX13_ADDR;
                    offset = MIPI_REG_OFFSET_MULT * pipe_index;
                    dphy_dst_reg = MAP_DPHY_DEST_0_MIPI_TX_1_MIPI_TX45_ADDR + pipe_index * 0x40;

                    for (uint8_t map_idx = 0u; map_idx < MAX96792_MAX_NUM_OF_SRC_DST_DT_VC_MAP; map_idx++)
                    {

                        if ((map_idx > 0) && ((map_idx % 4) == 0))
                        {
                            dphy_dst_reg += 1; /* Increment the DPHY Destination Register */
                        }
                        else
                        {
                            /* No increment needed */
                        }

                        if (((src_dst_map_enabled >> map_idx) & 0x1u) == 1u)
                        {
                            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset) + 2 * map_idx, MAX96792_MASK_TO_RW_ALL_MASK, &src_map);

                            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + 1 + offset) + 2 * map_idx, MAX96792_MASK_TO_RW_ALL_MASK, &dst_map);

                            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, dphy_dst_reg, MAP_DPHY_DEST_0_MIPI_TX_1_MIPI_TX45_MASK << (2 * (map_idx % 4)), &dphy_dst);

                            part_cfg_stat->src_dst_dt_vc_map[pipe_index][map_idx].src_pipe_dt = (src_map & 0x3FU);
                            part_cfg_stat->src_dst_dt_vc_map[pipe_index][map_idx].src_pipe_vc = (src_map >> 6u) & 0x3U;
                            part_cfg_stat->src_dst_dt_vc_map[pipe_index][map_idx].dst_pipe_dt = (dst_map & 0x3FU);
                            part_cfg_stat->src_dst_dt_vc_map[pipe_index][map_idx].dst_pipe_vc = (dst_map >> 6u) & 0x3U;
                            part_cfg_stat->dphy_dst[pipe_index][map_idx] = dphy_dst;
                            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MAP_DPHY_DST_%d Pipe %c: %d", map_idx, video_pipe_char[pipe_index], part_cfg_stat->dphy_dst[pipe_index][map_idx]);
                            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MAP_SRC_%d Pipe %c DT: %d, VC: %d", map_idx, video_pipe_char[pipe_index], part_cfg_stat->src_dst_dt_vc_map[pipe_index][map_idx].src_pipe_dt, part_cfg_stat->src_dst_dt_vc_map[pipe_index][map_idx].src_pipe_vc);
                            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MAP_DST_%d Pipe %c DT: %d, VC: %d", map_idx, video_pipe_char[pipe_index], part_cfg_stat->src_dst_dt_vc_map[pipe_index][map_idx].dst_pipe_dt, part_cfg_stat->src_dst_dt_vc_map[pipe_index][map_idx].dst_pipe_vc);
                        }
                        src_dst_count++;
                    }

                    base_addr = MIPI_TX_1_MIPI_TX51_ADDR;
                    offset = MIPI_REG_OFFSET_MULT * pipe_index;
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), MAX96792_READ_ALT_MEM_MAP_MASK, &alt_mem_map_en);
#ifdef _DEBUG
                    if ((alt_mem_map_en & 1u) == 1u)
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Alternative memory map enabled for 12-bit DT on pipe %c", video_pipe_char[pipe_index]);
                    }
                    else if (((alt_mem_map_en >> 1) & 1u) == 1u)
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Alternative memory map enabled for 8-bit DT on pipe %c", video_pipe_char[pipe_index]);
                    }
                    else if (((alt_mem_map_en >> 2) & 1u) == 1u)
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Alternative memory map enabled for 10-bit DT on pipe %c", video_pipe_char[pipe_index]);
                    }
                    /* Report ALT2_MEM_MAP8: */
                    if (((alt_mem_map_en >> 4) & 1u) == 1u)
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Alternative memory read mapping enabled for 8-bit DT when sharing the same video pipe with RAW16 on pipe %c", video_pipe_char[pipe_index]);
                    }
#endif
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c is not enabled, Source-Destination Virtual Channel and Data Type Map information is not checked", video_pipe_char[pipe_index]);
            }
        }

        for (uint8_t csi_index = 0; csi_index < MAX96792_NUM_OF_CSI_CONTROLLERS; csi_index++)
        {
            /* Read Lane Count */
            base_addr = CSI2_LANE_CNT_MIPI_TX_1_MIPI_TX10_ADDR;
            offset = MIPI_REG_OFFSET_MULT * csi_index;
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), CSI2_LANE_CNT_MIPI_TX_1_MIPI_TX10_MASK, (uint8_t *)&lane_count);
            part_cfg_stat->lane_count[csi_index] = lane_count;
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Number of MIPI data lanes configured for CSI2 controller %d: %u", (csi_index + 1), lane_count);

            /* Read Lane Mapping */
            base_addr = PHY0_LANE_MAP_MIPI_PHY_MIPI_PHY3_ADDR;
            offset = csi_index;
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), MAX96792_MASK_TO_RW_ALL_MASK, &part_cfg_stat->lane_map[csi_index]);
#ifdef _DEBUG
            if (part_cfg_stat->lane_map[csi_index] == MIPI_PHY_MIPI_PHY3_DEFAULT)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Lane Mapping for phy %d and phy %d: Default", 2 * csi_index, 2 * csi_index + 1);
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Lane mapping for phy %d: %x", 2 * csi_index, (part_cfg_stat->lane_map[csi_index] & PHY2_LANE_MAP_MIPI_PHY_MIPI_PHY4_MASK));
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Lane mapping for phy %d: %x", 2 * csi_index + 1, (part_cfg_stat->lane_map[csi_index] & PHY3_LANE_MAP_MIPI_PHY_MIPI_PHY4_MASK) >> 4);
            }
#endif
        }

        BREAK_ON_ERR(ret);

        /* Read PHY Mapping */
        base_addr = PHY0_POL_MAP_MIPI_PHY_MIPI_PHY5_ADDR;
        offset = 0;
        for (uint8_t phy_index = 0; phy_index < MAX96792_NUM_OF_MIPI_PHY; phy_index++)
        {
            offset = phy_index / 2;
            if (phy_index == 2)
            {
                base_addr = PHY2_POL_MAP_MIPI_PHY_MIPI_PHY6_ADDR;
                offset = 0;
            }

            if (phy_index % 2 == 0)
            {
                polarity_mask_index = 0;
            }
            else
            {
                polarity_mask_index = 1;
            }

            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), polarity_mask[polarity_mask_index], &phy_lane_polarity_map[phy_index]);
            if (phy_lane_polarity_map[phy_index] == 0u)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MIPI PHY%d Lane Polarity Mapping: Default (Normal)", phy_index);
                part_cfg_stat->lane_pol[phy_index].lane_0 = false;
                part_cfg_stat->lane_pol[phy_index].lane_1 = false;
                part_cfg_stat->lane_pol[phy_index].clk_lane = false;
            }
            else
            {
                if (phy_lane_polarity_map[phy_index] & 1u)
                {
                    part_cfg_stat->lane_pol[phy_index].lane_0 = true;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MIPI PHY%d Polarity for Lane 0 is Swapped", phy_index);
                }
                if ((phy_lane_polarity_map[phy_index] >> 1) & 1u)
                {
                    part_cfg_stat->lane_pol[phy_index].lane_1 = true;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MIPI PHY%d Polarity for Lane 1 is Swapped", phy_index);
                }
                if ((phy_index == 1 || phy_index == 2) && ((phy_lane_polarity_map[phy_index] >> 2) & 1u))
                {
                    part_cfg_stat->lane_pol[phy_index].clk_lane = true;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MIPI PHY%d Lane Polarity Mapping for Clock Lane is Swapped", phy_index);
                }
            }
        }

        BREAK_ON_ERR(ret);

        /* Check if MIPI Output is Enabled */

        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, CSI_OUT_EN_BACKTOP_BACKTOP12_ADDR, CSI_OUT_EN_BACKTOP_BACKTOP12_MASK, &mipi_out_enabled);

        if (mipi_out_enabled == 0u)
        {
            part_cfg_stat->mipi_out_enabled = false;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "MIPI CSI output is not enabled");
            *diag_err = true;
        }
        else
        {
            part_cfg_stat->mipi_out_enabled = true;
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MIPI CSI output is enabled");
        }

        /* Read MIPI Lane Frequency */
        base_addr = PHY0_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP22_ADDR;
        offset = 0;

        for (uint8_t phy_index = 0; phy_index < MAX96792_NUM_OF_MIPI_PHY; phy_index++)
        {
            part_cfg_stat->csi_lane_freq[phy_index] = 0u;
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), PHY1_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP25_MASK, (uint8_t *)&part_cfg_stat->csi_lane_freq[phy_index]);

#ifdef _DEBUG
            if (part_cfg_stat->csi_lane_freq[phy_index] == 0u)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MIPI CSI per Lane Frequency for PHY%d: 80 Mbps", phy_index);
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "MIPI CSI per Lane Frequency for PHY%d: %u Mbps", phy_index, 100 * part_cfg_stat->csi_lane_freq[phy_index]);
            }
#endif

            offset += 3;
        }

        BREAK_ON_ERR(ret);

        /* Read CSI PHYs software-override disable for frequency fine-tuning  */
        base_addr = PHY1_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP25_ADDR;
        offset = 0;

        for (uint8_t mipi_phy_index = 0; mipi_phy_index < MAX96792_NUM_OF_MIPI_PHY; mipi_phy_index++)
        {
            part_cfg_stat->csi_lane_freq_sw_override[mipi_phy_index] = 0u;
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), PHY1_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP25_MASK, (uint8_t *)&part_cfg_stat->csi_lane_freq_sw_override[mipi_phy_index]);
#ifdef _DEBUG
            if (part_cfg_stat->csi_lane_freq_sw_override[mipi_phy_index] == 0u)
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Software override for frequency fine tuning not enabled for PHY%d", mipi_phy_index);
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Software override for frequency fine tuning enabled for PHY%d", mipi_phy_index);
            }
#endif
            offset += 2;
        }

        BREAK_ON_ERR(ret);

        /* Read CSI PHY1 software-override disable for frequency fine-tuning  */
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY1_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP25_ADDR, PHY1_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP25_MASK, &pll_func_reset);

        if (pll_func_reset == 0u)
        {
            part_cfg_stat->pll_func_reset = false;
            GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "PLL functional reset not done");
        }
        else
        {
            part_cfg_stat->pll_func_reset = true;
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "PLL functional reset done");
        }

    } while (false);

    /* To-do: Error conditions to be updated after comparison with App Data */
    return ret;
}

/**
 * @brief Get the link lock status of the MAX96792 deserializer.
 *
 * @param dev                        - Device handle.
 * @param max_retransmission_status  - Pointer to maximum retransmission status structure.
 * @param diag_err                   - Pointer to a boolean variable to store the diagnostic error status.

 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_link_diag_check_max_retransmission_error_status(struct gmsl_dev *dev, void *max_retransmission_status, bool *diag_err)
{
    int ret = 0;
    uint8_t main_ctrl_chnl_err_stat_link = 0u;
    uint8_t gpio_chnl_err_stat_link = 0u;
    uint8_t passthrough_chnl1_err_stat_link = 0u;
    uint8_t passthrough_chnl2_err_stat_link = 0u;
    uint16_t combined_err_base_addr[MAX96792_NUM_OF_GMSL_LINKS] = {MAX_RT_FLAG_TCTRL_INTR5_ADDR, RT_CNT_FLAG_B_TCTRL_EXT_INTR11_ADDR};
    uint16_t main_ctrl_base_addr[MAX96792_NUM_OF_GMSL_LINKS] = {CFGC_CC_ARQ2_ADDR, CFGC_B_CC_ARQ2_ADDR};
    uint16_t gpio_base_addr[MAX96792_NUM_OF_GMSL_LINKS] = {CFGL_GPIO_ARQ2_ADDR, CFGL_B_GPIO_ARQ2_ADDR};
    uint16_t passthrough_chnl1_base_addr[MAX96792_NUM_OF_GMSL_LINKS] = {CFGC_IIC_X_ARQ2_ADDR, CFGC_B_IIC_X_ARQ2_ADDR};
#ifdef _DEBUG
    const char link_name[MAX96792_NUM_OF_GMSL_LINKS] = {'A', 'B'};
#endif

    uint8_t mask = MAX_RT_FLAG_TCTRL_INTR5_MASK;
    struct max96792_link_enabled_status link_en;
    uint8_t max_retrans_err_detected = 0;
    *diag_err = false;

    struct max96792_link_diag_max_retransmission_error_status *max_retransmission_stat = (struct max96792_link_diag_max_retransmission_error_status *)max_retransmission_status;

    do
    {
        ret = max96792_check_gmsl_link_enable(dev, &link_en);
        BREAK_ON_ERR(ret);

        /* Check for Combined Max Retransmission Error */
        for (uint8_t link_index = 0; link_index < MAX96792_NUM_OF_GMSL_LINKS; link_index++)
        {
            if (link_en.link_enabled[link_index])
            {

                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, combined_err_base_addr[link_index], mask, &max_retrans_err_detected);
                max_retransmission_stat->max_retrans_err_detected[link_index] = max_retrans_err_detected;

                if (max_retrans_err_detected != 0u)
                {
                    max_retransmission_stat->max_retrans_err_detected[link_index] = true;
                    *diag_err = true;

                    /* Check for Main Control Channel Error */
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, main_ctrl_base_addr[link_index], MAX96792_MASK_TO_RW_ALL_MASK, &main_ctrl_chnl_err_stat_link);

                    if ((main_ctrl_chnl_err_stat_link & 0x80u) != 0u)
                    {
                        max_retransmission_stat->max_retrans_data[link_index].main_control_channel_err_detected = true;
                        max_retransmission_stat->max_retrans_data[link_index].main_ctrl_chnl_retrans_count = main_ctrl_chnl_err_stat_link & 0x7Fu;
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Max Retransmission Error reported for Main Control Channel on Link %c with Error count - %d", link_name[link_index], max_retransmission_stat->max_retrans_data[link_index].main_ctrl_chnl_retrans_count);
                    }
                    else
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported for Main Control Channel on Link %c", link_name[link_index]);
                    }

                    /* Check for GPIO Channel error */
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, gpio_base_addr[link_index], MAX96792_MASK_TO_RW_ALL_MASK, &gpio_chnl_err_stat_link);

                    if ((gpio_chnl_err_stat_link & 0x80u) != 0u)
                    {
                        max_retransmission_stat->max_retrans_data[link_index].gpio_chnl_err_detected = true;
                        max_retransmission_stat->max_retrans_data[link_index].gpio_chnl_retrans_count = gpio_chnl_err_stat_link & 0x7Fu;
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Max Retransmission Error reported for GPIO Channel on Link %c with Error count - %d", link_name[link_index], max_retransmission_stat->max_retrans_data[link_index].gpio_chnl_retrans_count);
                    }
                    else
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported for GPIO Channel on Link %c", link_name[link_index]);
                    }

                    /* Check for error on Pass Through Channel 1 */
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, passthrough_chnl1_base_addr[link_index], MAX96792_MASK_TO_RW_ALL_MASK, &passthrough_chnl1_err_stat_link);

                    if ((passthrough_chnl1_err_stat_link & 0x80u) != 0u)
                    {
                        max_retransmission_stat->max_retrans_data[link_index].pass_thru_chnl1_err_detected = true;
                        max_retransmission_stat->max_retrans_data[link_index].pass_thru_chnl1_retrans_count = passthrough_chnl1_err_stat_link & 0x7Fu;
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Max Retransmission Error reported for Pass Through Channel 1 on Link %c with Error count - %d", link_name[link_index], max_retransmission_stat->max_retrans_data[link_index].pass_thru_chnl1_retrans_count);
                    }
                    else
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported for Pass Through Channel 1 on Link %c", link_name[link_index]);
                    }

                    /* Check for error on Pass Through Channel 2 */
                    REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, passthrough_chnl1_base_addr[link_index] + 1, MAX96792_MASK_TO_RW_ALL_MASK, &passthrough_chnl2_err_stat_link);

                    if ((passthrough_chnl2_err_stat_link & 0x80u) != 0u)
                    {
                        max_retransmission_stat->max_retrans_data[link_index].pass_thru_chnl2_err_detected = true;
                        max_retransmission_stat->max_retrans_data[link_index].pass_thru_chnl2_retrans_count = passthrough_chnl2_err_stat_link & 0x7Fu;
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Max Retransmission Error reported for Pass Through Channel 2 on Link %c with Error count - %d", link_name[link_index], max_retransmission_stat->max_retrans_data[link_index].pass_thru_chnl2_retrans_count);
                    }
                    else
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported for Pass Through Channel 2 on Link %c", link_name[link_index]);
                    }
                }
                else
                {
                    max_retransmission_stat->max_retrans_err_detected[link_index] = false;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Max Retransmission Error reported for Link %c", link_name[link_index]);
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Link %c is not enabled, Max Retransmission Error is not checked", link_name[link_index]);
            }
        }
    } while (false);

    return 0;
}

/**
 * @brief Check for video block length errors in the MAX96792 deserializer.
 *
 * This function checks for video block length errors in the specified GMSL device.
 * It reads the error status for each video pipe and logs the result.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param video_block_length_status Pointer to the structure where the video block length error status will be stored.
 * @param diag_err Pointer to a boolean variable that will be set to true if any video block length error is detected, false otherwise.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int max96792_pipe_diag_check_video_block_length_error(struct gmsl_dev *dev, void *video_block_length_status, bool *diag_err)
{
    int ret = 0;
    uint16_t base_addr = VID_BLK_LEN_ERR_VID_RX_Y_VIDEO_RX8_ADDR;
    uint16_t offset = 0;
    uint8_t mask = VID_BLK_LEN_ERR_VID_RX_Y_VIDEO_RX8_MASK;
#ifdef _DEBUG
    const char video_pipe_char[MAX96792_NUM_OF_VIDEO_PIPES] = {'Y', 'Z'};
#endif
    uint8_t vid_blk_length_err = 0;
    struct max96792_pipe_diag_video_block_length_status *video_block_length_stat = (struct max96792_pipe_diag_video_block_length_status *)video_block_length_status;
    struct max96792_pipe_enabled_status pipe_en;
    *diag_err = false;

    do
    {
        ret = max96792_check_video_pipe_enable(dev, &pipe_en);
        BREAK_ON_ERR(ret);

        for (uint8_t pipe_index = 0; pipe_index < MAX96792_NUM_OF_VIDEO_PIPES; pipe_index++)
        {

            if (pipe_en.pipe_enabled[pipe_index])
            {
                offset = MAX96792_BLK_LEN_ERR_OFFSET_MULT * pipe_index;

                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr + offset), mask, &vid_blk_length_err);

                if (vid_blk_length_err != 0u)
                {
                    video_block_length_stat->vid_blk_length_err[pipe_index] = true;
                    *diag_err = true;
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Video Block Length Error detected in Pipe %c", video_pipe_char[pipe_index]);
                }
                else
                {
                    video_block_length_stat->vid_blk_length_err[pipe_index] = false;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video Block Length Error Not detected in Pipe %c", video_pipe_char[pipe_index]);
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c is not enabled, Block Length Error is not checked", video_pipe_char[pipe_index]);
            }
        }
    } while (false);
    return 0;
}

/**
 * @brief Check for Remote Error Flag status in the MAX96792 deserializer.
 *
 * This function checks for Remote Error Flag status in the specified GMSL device.
 * It reads the error status for the Remote Control Channel and logs the result.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param rem_err_flg_status Pointer to the structure where the Remote Error Flag status will be stored.
 * @param diag_err Pointer to a boolean variable that will be set to true if any Remote Error Flag is detected, false otherwise.
 *
 * @return 0 on success, or a negative error code on failure.
 */

int max96792_dev_diag_check_remote_err_flg(struct gmsl_dev *dev, void *rem_err_flg_status, bool *diag_err)
{
    int ret = 0;
    struct max96792_dev_diag_rem_err_flg_status *rem_err_flg_stat = (struct max96792_dev_diag_rem_err_flg_status *)rem_err_flg_status;
    uint8_t rem_error = 0;
    uint8_t rem_error_ena = 0;

    *diag_err = false;
    do
    {
        /* Check if Remote Control Channel is Enabled */
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, DIS_REM_CC_DEV_REG1_ADDR, DIS_REM_CC_DEV_REG1_MASK, &rem_error_ena);
        if (rem_error_ena == 0u)
        {
            rem_err_flg_stat->rem_cc_en = true;
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Remote Control Channel is enabled");
            /* Check Remote Error Flag */
            REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, REM_ERR_FLAG_TCTRL_INTR3_ADDR, REM_ERR_FLAG_TCTRL_INTR3_MASK, &rem_error);
            if (rem_error != 0u)
            {
                rem_err_flg_stat->rem_err_flg = true;
                *diag_err = true;
                GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Remote Error detected");
            }
            else
            {
                rem_err_flg_stat->rem_err_flg = false;
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Remote Error detected");
            }
        }
        else
        {
            rem_err_flg_stat->rem_cc_en = false;
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Remote Control Channel is disabled. Remote Error Flag status Not Checked");
        }

    } while (false);

    return 0;
}

/**
 * @brief Check for Eye Open Monitor errors in the MAX96792 deserializer.
 *
 * This function checks for Eye Open Monitor errors in the specified GMSL device.
 * It reads the error status for each GMSL link and logs the result.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param eom_status Pointer to the structure where the Eye Open Monitor error status will be stored.
 * @param diag_err Pointer to a boolean variable that will be set to true if any Eye Open Monitor error is detected, false otherwise.
 *
 * @return 0 on success, or a negative error code on failure.
 */

int max96792_link_diag_check_eye_open_monitor_error(struct gmsl_dev *dev, void *eom_status, bool *diag_err)
{
    int ret = 0;
    uint16_t base_addr = EOM_ERR_FLAG_A_TCTRL_INTR5_ADDR;
    const uint8_t mask[MAX96792_NUM_OF_GMSL_LINKS] = {EOM_ERR_FLAG_A_TCTRL_INTR5_MASK, EOM_ERR_FLAG_B_TCTRL_INTR5_MASK};
#ifdef _DEBUG
    const char link_name[MAX96792_NUM_OF_GMSL_LINKS] = {'A', 'B'};
#endif
    struct max96792_link_enabled_status link_en;
    struct max96792_link_diag_eom_status *eom_stat = (struct max96792_link_diag_eom_status *)eom_status;
    uint8_t eom_err = 0;
    *diag_err = false;

    do
    {

        ret = max96792_check_gmsl_link_enable(dev, &link_en);
        BREAK_ON_ERR(ret);

        for (uint8_t link_index = 0; link_index < MAX96792_NUM_OF_GMSL_LINKS; link_index++)
        {
            if (link_en.link_enabled[link_index])
            {

                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (base_addr), mask[link_index], &eom_err);

                if (eom_err != 0u)
                {
                    eom_stat->eom_err[link_index] = true;
                    *diag_err = true;
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "EOM Error detected in Link %c", link_name[link_index]);
                }
                else
                {
                    eom_stat->eom_err[link_index] = false;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No EOM Error detected in Link %c", link_name[link_index]);
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Link %c is not enabled, EOM Error is not checked", link_name[link_index]);
            }
        }
    } while (false);
    return 0;
}

/**
 * @brief Check for Line Memory Overflow errors in the video pipes of the MAX96792 deserializer.
 *
 * This function checks for Line Memory Overflow errors in the video pipes of the specified GMSL device.
 * It reads the Line Memory Overflow status for each video pipe and logs the result.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param line_mem_ovrflw_status Pointer to the structure where the Line Memory Overflow error status will be stored.
 * @param diag_err Pointer to a boolean variable that will be set to true if any Line Memory Overflow error is detected, false otherwise.
 *
 */

int max96792_pipe_diag_check_line_memory_overflow(struct gmsl_dev *dev, void *line_mem_ovrflw_status, bool *diag_err)
{
    int ret = 0;
    const uint8_t mask[MAX96792_NUM_OF_VIDEO_PIPES] = {LMO_Y_BACKTOP_BACKTOP11_MASK, LMO_Z_BACKTOP_BACKTOP11_MASK};
    const uint8_t pos[MAX96792_NUM_OF_VIDEO_PIPES] = {LMO_Y_BACKTOP_BACKTOP11_POS, LMO_Z_BACKTOP_BACKTOP11_POS};
#ifdef _DEBUG
    const char video_pipe_char[MAX96792_NUM_OF_VIDEO_PIPES] = {'Y', 'Z'};
#endif
    uint8_t line_mem_ovrflw = 0;
    uint8_t line_mem_ovrflw_reg = 0;
    struct max96792_pipe_enabled_status pipe_en;

    struct max96792_pipe_diag_line_mem_ovrflw_status *line_mem_ovrflw_stat = (struct max96792_pipe_diag_line_mem_ovrflw_status *)line_mem_ovrflw_status;
    *diag_err = false;

    do
    {
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, LMO_Y_BACKTOP_BACKTOP11_ADDR, MAX96792_MASK_TO_RW_ALL_MASK, &line_mem_ovrflw_reg);
        ret = max96792_check_video_pipe_enable(dev, &pipe_en);
        BREAK_ON_ERR(ret);

        for (uint8_t pipe_index = 0; pipe_index < MAX96792_NUM_OF_VIDEO_PIPES; pipe_index++)
        {
            if (pipe_en.pipe_enabled[pipe_index])
            {

                line_mem_ovrflw = (line_mem_ovrflw_reg & mask[pipe_index]) >> pos[pipe_index];

                if (line_mem_ovrflw != 0u)
                {
                    line_mem_ovrflw_stat->line_mem_ovrflw_detected[pipe_index] = true;
                    *diag_err = true;
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Line Memory Overflow Error detected in Pipe %c", video_pipe_char[pipe_index]);
                }
                else
                {
                    line_mem_ovrflw_stat->line_mem_ovrflw_detected[pipe_index] = false;
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Line Memory Overflow Error detected in Pipe %c", video_pipe_char[pipe_index]);
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c is not enabled, Line memory overflow error is not checked", video_pipe_char[pipe_index]);
            }
        }
    } while (false);
    return 0;
}

/**
 * @brief Check for CRC errors in the video pipes of the MAX96792 deserializer.
 *
 * This function checks for CRC errors in the video pipes of the specified GMSL device.
 * It reads the CRC error status for each video pipe and logs the result.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param vid_line_crc_status Pointer to the structure where the video line CRC error status will be stored.
 * @param diag_err Pointer to a boolean variable that will be set to true if any video line CRC error is detected, false otherwise.
 *
 */
int max96792_pipe_diag_check_crc_error(struct gmsl_dev *dev, void *vid_line_crc_status, bool *diag_err)
{
    int ret = 0;
    uint16_t crc_en_base_addr = LINE_CRC_EN_VID_RX_Y_VIDEO_RX0_ADDR;
    uint8_t mask = LCRC_ERR_VID_RX_Y_VIDEO_RX0_MASK;
    uint16_t offset = 0;
    uint8_t crc_en_mask = LINE_CRC_EN_VID_RX_Y_VIDEO_RX0_MASK;
#ifdef _DEBUG
    const char video_pipe_char[MAX96792_NUM_OF_VIDEO_PIPES] = {'Y', 'Z'};
#endif
    uint8_t crc_reg = 0;
    struct max96792_pipe_enabled_status pipe_en;
    struct max96792_pipe_diag_crc_err_status *vid_line_crc_stat = (struct max96792_pipe_diag_crc_err_status *)vid_line_crc_status;
    *diag_err = false;

    do
    {

        ret = max96792_check_video_pipe_enable(dev, &pipe_en);
        BREAK_ON_ERR(ret);

        for (uint8_t pipe_index = 0; pipe_index < MAX96792_NUM_OF_VIDEO_PIPES; pipe_index++)
        {

            if (pipe_en.pipe_enabled[pipe_index])
            {
                offset = MAX96792_PIPE_DIAG_CHECK_CRC_ERROR_OFFSET_MULT * pipe_index;

                /* Check if Line CRC is Enabled */
                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, (crc_en_base_addr + offset), MAX96792_MASK_TO_RW_ALL_MASK, &crc_reg);

                vid_line_crc_stat->line_crc_en[pipe_index] = (crc_reg & crc_en_mask) >> LINE_CRC_EN_VID_RX_Y_VIDEO_RX0_POS;

                if (vid_line_crc_stat->line_crc_en[pipe_index])
                {
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video Line CRC is Enabled for Pipe %c", video_pipe_char[pipe_index]);

                    /* Check Line CRC Error */
                    vid_line_crc_stat->line_crc_err[pipe_index] = (crc_reg & mask) >> LCRC_ERR_VID_RX_Y_VIDEO_RX0_POS;
                    if (vid_line_crc_stat->line_crc_err[pipe_index])
                    {
                        *diag_err = vid_line_crc_stat->line_crc_err[pipe_index];
                        GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Video Line CRC Error detected in Pipe %c", video_pipe_char[pipe_index]);
                    }
                    else
                    {
                        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "No Video Line CRC Error detected in Pipe %c", video_pipe_char[pipe_index]);
                    }
                }
                else
                {
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Video Line CRC is Not Enabled for Pipe %c", video_pipe_char[pipe_index]);
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c is not enabled, CRC error is not checked", video_pipe_char[pipe_index]);
            }
        }

    } while (false);
    return 0;
}

/**
 * @brief Check the stream ID for each video pipe and log diagnostic information.
 *
 * This function reads the stream ID for each video pipe from the device and checks if it is valid.
 * If the stream ID is invalid, an error is logged and the diagnostic error flag is set.
 * Otherwise, the stream ID is logged for diagnostic purposes.
 *
 * @param dev Pointer to the GMSL device structure.
 * @param stream_id_status Pointer to the structure where stream ID status will be stored.
 * @param diag_err Pointer to a boolean flag that will be set to true if a diagnostic error is detected.
 * @return 0 on success, or a negative error code on failure.
 */
int max96792_pipe_diag_check_stream_id(struct gmsl_dev *dev, void *stream_id_status, bool *diag_err)
{
    int ret = 0;
    const uint8_t mask[MAX96792_NUM_OF_VIDEO_PIPES] = {VIDEO_PIPE_SEL_Y_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_MASK, VIDEO_PIPE_SEL_Z_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_MASK};
#ifdef _DEBUG
    const char video_pipe_char[MAX96792_NUM_OF_VIDEO_PIPES] = {'Y', 'Z'};
#endif
    struct max96792_csi_diag_stream_id_status *stream_id_stat = (struct max96792_csi_diag_stream_id_status *)stream_id_status;
    struct max96792_pipe_enabled_status pipe_en;
    *diag_err = false;

    do
    {
        ret = max96792_check_video_pipe_enable(dev, &pipe_en);
        BREAK_ON_ERR(ret);

        for (uint8_t pipe_index = 0; pipe_index < MAX96792_NUM_OF_VIDEO_PIPES; pipe_index++)
        {

            if (pipe_en.pipe_enabled[pipe_index])
            {
                /* Read Stream ID */
                REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, VIDEO_PIPE_SEL_Y_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_ADDR, mask[pipe_index], (uint8_t *)&stream_id_stat->stream_id[pipe_index]);
                stream_id_stat->stream_id[pipe_index] = stream_id_stat->stream_id[pipe_index] & 0x03;
                if (((stream_id_stat->stream_id[pipe_index]) & (0x03)) > MAX96792_MAX_STREAM_ID)
                {
                    GMSL_LOG_DEV_ERROR(dev->dev_name, dev->i2c_desc->slave_address, "Invalid Stream ID for Pipe %c", video_pipe_char[pipe_index]);
                    *diag_err = true;
                }
                else
                {
                    GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Stream ID for Pipe %c: %d", video_pipe_char[pipe_index], stream_id_stat->stream_id[pipe_index]);
                }
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, "Pipe %c is not enabled, Stream ID is not checked", video_pipe_char[pipe_index]);
            }
        }
    } while (false);
    return 0;
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