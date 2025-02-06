/***************************************************************************//**
 *   @file   max96793.c
 *   @brief  Implementation of MAX96793 Driver.
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
#include "max96793.h"
#include "max96793_diag.h"
#include "max96793_csi.h"
#include "gmsl_dbg.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "gmsl_reg_access.h"

#define MAX96793_MAX_LINKS          (1U)
#define MAX96793_MAX_LMN            (2U)
#define MAX96793_MAX_PHY_CTRLR      (1U)

#define DEV_NAME "MAX96793"

/** \addtogroup ADI_GMSL_CAM_SERIALIZER GMSL Camera Serializers
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_SER2 MAX96793
 *  @{
 *  The MAX96793 GMSL serializer receives video on a MIPI CSI-2 interface and outputs it to a GMSL2 or GMSL3 serial link transceiver. Simultaneously, it
 *  sends and receives bidirectional control channel data across the same GMSL link. GMSL2 or GMSL3 data can be transported over coaxial or shielded 
 *  twisted pair (STP) cables. GMSL2 links operate at a fixed rate of 3Gbps or 6Gbps in the forward direction and 187.5Mbps in the reverse direction. 
 *  GMSL3 links operate at a fixed rate of 12Gbps in the forward direction and 187.5Mbps in the reverse direction. The device is programmed through a 
 *  local I2C interface or across the link from a matching deserializer. MAX96793 serializer can be used with GMSL 2/3 Camera Deserializers like MAX96792.
 */

/** \addtogroup ADI_GMSL_CAM_SER2_DEV_API Device APIs
 *  @{
 */

#ifndef ADI_DISABLE_DOCUMENTATION

/*=================================== STATIC ==================================*/

static const struct gmsl_dev_diag_ops max96793_diag_ops = {
    .diag_check_device_id = max96793_dev_diag_get_device_id,
    .diag_check_device_rev = max96793_dev_diag_check_device_rev,
    .diag_check_part_config = max96793_dev_diag_check_part_config_status,
    .diag_check_decode_error_status = max96793_link_diag_check_decode_error_status,
    .diag_check_idle_err_status = max96793_link_diag_check_idle_error_status,
    .diag_check_link_lock_status = max96793_link_diag_check_link_lock_status,
    .diag_check_max_retrans_status = max96793_link_diag_check_max_retransmission_error_status,
    .diag_check_mipi_rx_err_status = max96793_csi_diag_check_mipi_rx_err_status,
    .diag_check_dp_fec_support = NULL,
    .diag_check_drift_status = max96793_csi_diag_check_vid_tx_pclk_drift_err_status,
    .diag_check_eom_status = max96793_link_diag_check_eye_open_monitor_status,
    .diag_check_fec_status = max96793_csi_diag_check_fec_status,
    .diag_check_get_is_video_line_crc_enabled = NULL,
    .diag_check_hdcp_status = NULL,
    .diag_check_line_fault_status = max96793_link_diag_check_line_fault_status,
    .diag_check_link_training_status = NULL,
    .diag_check_video_stream_dsc_compression = NULL,
    .diag_check_stream_id_status = max96793_pipe_diag_check_stream_id,
    .diag_check_pps_status = NULL,
    .diag_check_pclk_lost_status = max96793_csi_diag_check_pclk_detect_status,
    .diag_check_parameter_status = NULL,
    .diag_check_overflow_status = max96793_pipe_diag_check_vid_tx_overflow_status,
    .diag_check_mem_ecc_2b_err_status = max96793_mem_diag_check_uncorrectable_2b_mem_ecc_err_status,
    .diag_check_phy_low_power_err_status = max96793_link_diag_check_phy_low_power_state_error_status,
    .diag_check_mipi_status = max96793_csi_diag_check_mipi_phy_pkt_cnt_status,
    .diag_check_remote_err = max96793_dev_diag_check_remote_error_status,
};

#endif /*ADI_DISABLE_DOCUMENTATION */

/******************************************************************************/
/************************* Functions Definition *******************************/
/******************************************************************************/

/**
 * @brief Initialize the MAX96793 device.
 *
 * @param device     - Pointer to the GMSL device structure.
 * @param i2c_desc   - I2C descriptor.
 * @param init_param - Initialization parameters.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t max96793_init(struct gmsl_dev **device, struct no_os_i2c_desc *i2c_desc, void *init_param)
{
    int32_t ret = 0;

    struct gmsl_dev *dev;
    struct max96793_dev_capabilities *dev_cap;
    struct gmsl_dev_csi_ser_init_param *init_cfg_param = (struct gmsl_dev_csi_ser_init_param *)init_param;

    if (!device) {
        ret = -EINVAL;
        goto init_error;
    }

    dev = no_os_calloc(1, sizeof(*dev));
    if (!dev) {
        ret = -ENOMEM;
        goto init_error;
    }

    dev_cap = no_os_calloc(1, sizeof(*dev_cap));
    if (!dev_cap) {
        ret = -ENOMEM;
        goto error_remove_dev;
    }

    dev_cap->max_links_count = MAX96793_MAX_LINKS;
    dev_cap->max_lmn_count = MAX96793_MAX_LMN;
    dev_cap->max_phy_ctrlr = MAX96793_MAX_PHY_CTRLR;

    dev->index = init_cfg_param->index;
    dev->part_id = 0;     /** @todo: Implement reading the device ID from the hardware */
    dev->rev_id = 0;     /** @todo: Implement reading the revision ID from the hardware */
    dev->dev_cap = dev_cap;
    dev->i2c_desc = i2c_desc;
    dev->dev_config = init_param;
    dev->dev_diag = (void *)&max96793_diag_ops;

    if(init_cfg_param->dev_name != NULL)
        strcpy(dev->dev_name, init_cfg_param->dev_name);
    else
        strcpy(dev->dev_name, DEV_NAME);         /*!< Default device name */

    dev->dev_state = no_os_calloc(1, MAX96793_DEV_STATE_MEM_SIZE );
    if (!dev->dev_state) {
        ret = -ENOMEM;
        goto error_remove_cap;
    }

    *device = dev;
    
    GMSL_LOG_INFO("%s-0x%x initialization passed", dev->dev_name, dev->i2c_desc->slave_address);

    goto exit;

    error_remove_cap:
        no_os_free(dev_cap);

    error_remove_dev:
        no_os_free(dev);

    init_error:
        GMSL_LOG_ERROR("%s-0x%x initialization failed", dev->dev_name, dev->i2c_desc->slave_address);

    exit:
        return ret;
}

/**
 * @brief Free memory allocated by max96793_init().
 *
 * @param dev - Device handle.
 *
 * @return 0 in case of success, error code otherwise.
 */
int32_t max96793_remove(struct gmsl_dev *dev)
{
    int32_t ret = 0;
#ifdef _DEBUG
    uint8_t slave_addr;
#endif
    if (!dev)
    {
        GMSL_LOG_ERROR("%s remove failed", (char *)DEV_NAME);
        return -EINVAL;
    }
#ifdef _DEBUG
    slave_addr = dev->i2c_desc->slave_address;
#endif
    if(dev->dev_state != NULL)
    {
        no_os_free(dev->dev_state);
    }
    else
    {
        GMSL_LOG_ERROR("%s-0x%x Freeing of device state memory failed", (char *)DEV_NAME, slave_addr);
        ret = -EINVAL;
    }

    if(dev->dev_cap != NULL)
    {
        no_os_free(dev->dev_cap);
    }
    else
    {
        GMSL_LOG_ERROR("%s-0x%x Freeing of device capabilities memory failed", (char *)DEV_NAME, slave_addr);
        ret = -EINVAL;
    }

    no_os_free(dev);

    GMSL_LOG_INFO("%s-0x%x remove passed", (char *)DEV_NAME, slave_addr);

    return ret;
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