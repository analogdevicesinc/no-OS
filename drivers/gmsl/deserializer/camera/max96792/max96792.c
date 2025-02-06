/*****************************************************************************
 *   @file   max96792.c
 *   @brief  Implementation of max96792 Driver.
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
#include "max96792_regs.h"
#include "max96792_csi.h"
#include "gmsl_dbg.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "gmsl_cam_des.h"
#include "max96792_diag.h"

#define MAX96792_MAX_LINKS          (2U)
#define MAX96792_MAX_LMN            (4U)
#define MAX96792_MAX_PHY_CTRLR      (4U)
#define DEV_NAME                    "MAX96792"
/** \addtogroup ADI_GMSL_CAM_DESERIALIZER GMSL Camera Deserializers
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_DES3 MAX96792
 *  @{
 *  The MAX96792A converts dual GMSL3/GMSL2 serial inputs to MIPI CSI-2. It transmits and receives control channel data over each serial link, enabling
 *  transmission of forward path video and bidirectional control data over coaxial or shielded twisted pair (STP) cables. The GMSL3 serial link operates
 *  at 12Gbps in the forward direction and 187.5Mbps in reverse direction. The MAX96792A is backward compatible with GMSL2 and can be paired with 6Gbps
 *  or 3Gbps GMSL2 serializers. The GMSL inputs operate independently, allowing video with different timing and resolution to be received on each input.
 *  The two GMSL links can also have different data rates.Video data from both inputs can be aggregated for output on a single CSI-2 port and can be
 *  replicated on a second output port for redundant processing. The device is configured using an I2C interface. MAX96792 deserializer can be used
 *  with GMSL 2/3 Camera Serializers like MAX96717 and MAX96793.
*/ 
/** \addtogroup ADI_GMSL_CAM_DES3_DEV_API Device APIs
 *  @{
 */

#ifndef ADI_DISABLE_DOCUMENTATION

/**
 * @struct gmsl_dev_diag_ops
 * @brief Diagnostic operations for the MAX96792 deserializer.
 *
 * This structure defines a set of function pointers for performing various
 * diagnostic checks on the MAX96792 deserializer. Each function pointer
 * corresponds to a specific diagnostic check.
 */
static const struct gmsl_dev_diag_ops max96792_diag_ops = {
    .diag_check_device_id = max96792_dev_diag_get_device_id,                                   /*!< API to check the device ID of the MAX96792 deserializer. */
    .diag_check_device_rev = max96792_dev_diag_get_device_revision,                            /*!< API to check the device revision of the MAX96792 deserializer. */
    .diag_check_part_config = max96792_dev_diag_check_part_config,                             /*!< API to check the part configuration of the MAX96792 deserializer. */
    .diag_check_link_lock_status = max96792_link_diag_check_link_lock_status,                  /*!< API to check the link lock status of the MAX96792 deserializer. */
    .diag_check_decode_error_status = max96792_link_diag_check_decode_error_status,            /*!< API to check the decode error status of the MAX96792 deserializer. */
    .diag_check_idle_err_status = max96792_link_diag_check_idle_error_status,                  /*!< API to check the idle error status of the MAX96792 deserializer. */
    .diag_check_max_retrans_status = max96792_link_diag_check_max_retransmission_error_status,        /*!< API to check the maximum retransmission error status of the MAX96792 deserializer. */
    .diag_check_line_fault_status = max96792_link_diag_check_line_fault_status,                /*!< API to check the line fault status of the MAX96792 deserializer. */
    .diag_check_mipi_status = max96792_csi_diag_check_mipi_status,                             /*!< API to check the MIPI status of the MAX96792 deserializer. */
    .diag_check_fec_status = NOT_APPLICABLE_FOR_MAX96792,
    .diag_check_eom_status = max96792_link_diag_check_eye_open_monitor_error, /*!< API to check the eye open monitor status of the MAX96792 deserializer. */
    .diag_check_pclk_lost_status = NOT_APPLICABLE_FOR_MAX96792,
    .diag_check_lmo_status = max96792_pipe_diag_check_line_memory_overflow, /*!< API to check the line memory overflow status of the MAX96792 deserializer. */
    .diag_check_drift_status = NOT_APPLICABLE_FOR_MAX96792,
    .diag_check_get_is_video_line_crc_error_status = max96792_pipe_diag_check_crc_error, /*!< API to check the video line CRC error status of the MAX96792 deserializer. */
    .diag_check_stream_id_status = max96792_pipe_diag_check_stream_id,                   /*!< API to check the stream ID status of the MAX96792 deserializer. */
    .diag_check_parameter_status = NOT_APPLICABLE_FOR_MAX96792,
    .diag_check_pps_status = NOT_APPLICABLE_FOR_MAX96792,
    .diag_check_video_stream_dsc_compression = NOT_APPLICABLE_FOR_MAX96792,
    .diag_check_dp_fec_support = NOT_APPLICABLE_FOR_MAX96792,
    .diag_check_hdcp_status = NOT_APPLICABLE_FOR_MAX96792,
    .diag_check_link_training_status = NOT_APPLICABLE_FOR_MAX96792,
    .diag_check_video_lock_status = max96792_pipe_diag_check_video_lock_status,             /*!< API to check the video lock status of the MAX96792 deserializer. */
    .diag_check_video_block_length_err = max96792_pipe_diag_check_video_block_length_error, /*!< API to check the video block length error status of the MAX96792 deserializer. */
    .diag_check_remote_err = max96792_dev_diag_check_remote_err_flg,                        /*!< API to check the remote error flag status of the MAX96792 deserializer. */
};
#endif /*ADI_DISABLE_DOCUMENTATION */

/**************************************************************************** */
/**************************Static Funtion Declaration *************************/
/**************************************************************************** */

/******************************************************************************/
/************************* Functions Definition *******************************/
/******************************************************************************/

/**
 * @brief Initialize the device driver before use.
 * @param  device     - Device handle pointer.
 * @param  i2c_desc   - Pointer to the i2c descriptor.
 * @param  init_param - Pointer to the initialization structure.
 * @return 0 in case of success, error code otherwise.
 */
int32_t max96792_init(struct gmsl_dev** device, struct no_os_i2c_desc* i2c_desc, void* init_param)
{
    int32_t ret = 0;
    bool error_remove_cap = false, error_remove_dev = false, init_error = false;
    struct gmsl_dev* dev;
    struct max96792_dev_capabilities* dev_cap;
    struct gmsl_dev_csi_des_init_param* init_cfg_param = (struct gmsl_dev_csi_des_init_param*)init_param;
    do
    {
        if (device == NULL)
        {
            ret = -EINVAL;
            init_error = true;
            break;
        }

        dev = no_os_calloc(1, sizeof(*dev));
        if (!dev) 
        {
            ret = -ENOMEM;
            init_error = true;
            break;
        }

        dev_cap = no_os_calloc(1, sizeof(*dev_cap));
        if (dev_cap == NULL) 
        {
            ret = -ENOMEM;
            error_remove_dev = true;
            break;
        }

        dev_cap->max_links_count = MAX96792_MAX_LINKS;
        dev_cap->max_lmn_count   = MAX96792_MAX_LMN;
        dev_cap->max_phy_ctrlr   = MAX96792_MAX_PHY_CTRLR;
        dev->index = init_cfg_param->index;

        ret = gmsl_reg_read(i2c_desc, DEV_ID_DEV_REG13_ADDR, DEV_ID_DEV_REG13_MASK, (uint8_t *)&dev->part_id);
        if(ret != 0) 
        {
            GMSL_LOG_ERROR("%s-0x%x I2c Part ID read failed", dev->dev_name, i2c_desc->slave_address);
            error_remove_cap = true;
            break;
        }

        ret = gmsl_reg_read(i2c_desc, DEV_REV_DEV_REG14_ADDR, DEV_REV_DEV_REG14_MASK, &dev->rev_id);
        if (ret != 0)
        {
            GMSL_LOG_ERROR("%s-0x%x I2c Revision ID read failed", dev->dev_name, i2c_desc->slave_address);
            error_remove_cap = true;
            break;
        }

        dev->dev_cap = dev_cap;
        dev->i2c_desc = i2c_desc;
        dev->dev_config = (void*)init_cfg_param;
        dev->dev_diag = (void*)&max96792_diag_ops;

        if (init_cfg_param->dev_name != NULL)
            strcpy(dev->dev_name, init_cfg_param->dev_name);
        else
            strcpy(dev->dev_name, DEV_NAME);

        dev->dev_state = no_os_calloc(1, MAX96792_DEV_STATE_MEM_SIZE);
        if (!dev->dev_state) 
        {
            ret = -ENOMEM;
            error_remove_cap = true;
            break;
        }

        *device = dev;
        GMSL_LOG_INFO("%s-0x%x initialization passed", dev->dev_name, dev->i2c_desc->slave_address);

    } while (false);

    if (error_remove_cap)
    {
        no_os_free(dev_cap);
    }

    if (error_remove_cap || error_remove_dev)
    {
        no_os_free(dev);
    }
    if (error_remove_cap || error_remove_dev || init_error)
    {
        GMSL_LOG_ERROR("%s-0x%x initialization failed", dev->dev_name, dev->i2c_desc->slave_address);
    }

    return ret;
}

/**
 * @brief Free memory allocated by max96792_init().
 * @param      dev - Device handle.
 * @return 0 in case of success, error code otherwise.
 */
int32_t max96792_remove(struct gmsl_dev* dev)
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
    if (dev->dev_state != NULL)
    {
        no_os_free(dev->dev_state);
    }
    else
    {
        GMSL_LOG_ERROR("%s-0x%x Freeing of device state memory failed", (char *)DEV_NAME, slave_addr);
        ret = -EINVAL;
    }

    if (dev->dev_cap != NULL)
    {
        no_os_free(dev->dev_cap);
    }
    else
    {
        GMSL_LOG_ERROR("%s-0x%x Freeing of device capability memory failed", (char *)DEV_NAME, slave_addr);
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