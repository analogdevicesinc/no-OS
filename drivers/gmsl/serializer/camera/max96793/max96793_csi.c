/***************************************************************************//**
 *   @file   max96793_csi.c
 *   @brief  CSI Driver Implementation of MAX96793 GMSL3 Serializer.
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
#include "max96793_csi.h"
#include "gmsl_dbg.h"
#include "gmsl_reg_access.h"
#include "no_os_error.h"
#include "max96793_regs.h"
/** \addtogroup ADI_GMSL_CAM_SERIALIZER GMSL Camera Serializers
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_SER2 MAX96793
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_SER2_CFG_API Configuration APIs
 *  @{
 */
/******************************************************************************/
/************************* Functions Definition *******************************/
/******************************************************************************/

/**
 * @brief Change the serializer address of the MAX96793 device.
 *
 * @param dev      - Pointer to the GMSL device structure.
 * @param new_addr - New address to be set.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t max96793_csi_change_ser_address(struct gmsl_dev *dev, unsigned int new_addr)
{
    int ret;

    ret = gmsl_reg_write(dev->i2c_desc, DEV_ADDR_DEV_REG0_ADDR, new_addr);

    return ret;
}

/**
 * @brief Enable or disable the pipe of the MAX96793 device.
 *
 * @param dev    - Pointer to the GMSL device structure.
 * @param enable - Boolean to enable or disable the pipe.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t max96793_csi_set_pipe_enable(struct gmsl_dev *dev, bool enable)
{
    int32_t ret = 0;
    
    ret = gmsl_reg_update(dev->i2c_desc, VID_TX_EN_Z_DEV_REG2_ADDR, (uint8_t)enable, VID_TX_EN_Z_DEV_REG2_MASK);

    return ret;
}

/**
 * @brief Configure the sensor to serializer lane mapping for the MAX96793 device.
 *
 * @param dev - Pointer to the GMSL device structure.
 * @param phy - Pointer to the GMSL camera serializer PHY structure.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t max96793_csi_config_sensor_to_ser_lane_map(struct gmsl_dev *dev, struct gmsl_link_ser_phy_cfg *phy)
{
    int ret = 0;
    unsigned int val;

    GMSL_LOG_INFO("MAX96793 Sensor to Serializer Lane Mapping configuration.");

    val = 0x0FU & ((unsigned int)phy->mipi.lane[0] | ((unsigned int)phy->mipi.lane[1] << 2U));
 
    switch(phy->id)
    {
        case 1:
            ret = gmsl_reg_update(dev->i2c_desc, PHY1_LANE_MAP_MIPI_RX_MIPI_RX2_ADDR, val, PHY1_LANE_MAP_MIPI_RX_MIPI_RX2_MASK);
            if(ret != 0)
                return ret;
            break;
        case 2:
            ret = gmsl_reg_update(dev->i2c_desc, PHY2_LANE_MAP_MIPI_RX_MIPI_RX3_ADDR, val, PHY2_LANE_MAP_MIPI_RX_MIPI_RX3_MASK);
            if(ret != 0)
                return ret;
            break;
        default:
            GMSL_LOG_ERROR("Invalid PHY ID. ");
            ret = -ENOTSUP;
            break;
    }

    return ret;
}

/**
 * @brief Configure the lane polarities for the MAX96793 device.
 *
 * @param dev - Pointer to the GMSL device structure.
 * @param phy - Pointer to the GMSL camera serializer PHY structure.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t max96793_csi_config_sensor_lane_pol(struct gmsl_dev *dev, struct gmsl_link_ser_phy_cfg *phy)
{
    int ret = 0;
    unsigned int val = 0;
    
    GMSL_LOG_INFO("MAX96793 Lane Polarities configuration.");

    val = 0x07U & ((unsigned int)phy->mipi.lane_polarities[0] | ((unsigned int)phy->mipi.lane_polarities[1] << 1U) | ((unsigned int)phy->mipi.lane_polarities[2] << 2U));

    switch(phy->id)
    {
        case 1:
            ret = gmsl_reg_update(dev->i2c_desc, PHY1_POL_MAP_MIPI_RX_MIPI_RX4_ADDR, val, PHY1_POL_MAP_MIPI_RX_MIPI_RX4_MASK);
            if(ret != 0)
                return ret;
            break;
        case 2:
            ret = gmsl_reg_update(dev->i2c_desc, PHY2_POL_MAP_MIPI_RX_MIPI_RX5_ADDR, val, PHY2_POL_MAP_MIPI_RX_MIPI_RX5_MASK);
            if(ret != 0)
                return ret;
            break;
        default:
            GMSL_LOG_ERROR("Invalid PHY ID. ");
            ret = -ENOTSUP;
            break;
    }

    return ret;
}

/**
 * @brief Initialize the CSI port for the MAX96793 device.
 *
 * @param dev  - Pointer to the GMSL device structure.
 * @param port - Pointer to the GMSL camera serializer port structure.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t max96793_csi_init_port(struct gmsl_dev *dev, struct gmsl_csi_ser_port_cfg *port)
{
    unsigned int val;
    unsigned int i;
    int ret = 0;

    switch(port->lane_count)
    {
    case 4:
        val = 0x3;
        break;
    case 2:
        val = 0x1;
        break;
    case 1:
        val = 0x0;
        break;
    default:
        GMSL_LOG_ERROR("Invalid number of data lanes.");
        ret = -ENOTSUP;
        break;
    }
    if (ret != 0)
        return ret;

    /* Configure a lane count. */
    ret = gmsl_reg_update(dev->i2c_desc, CTRL1_NUM_LANES_MIPI_RX_MIPI_RX1_ADDR, val , CTRL1_NUM_LANES_MIPI_RX_MIPI_RX1_MASK);
    if(ret != 0)
        return ret;
    if((port->en_cphy == false) && (port->en_deskew == true))
    {
        /* Configure Deskew. */
        ret = gmsl_reg_update(dev->i2c_desc, CTRL1_DESKEWEN_MIPI_RX_MIPI_RX1_ADDR, 1, CTRL1_DESKEWEN_MIPI_RX_MIPI_RX1_MASK);
    }
    
    for (i = 0; i < GMSL_CAM_SER_CSI2_MAX_MIPI_PHY_PER_PORT; i++)
    {
        struct gmsl_link_ser_phy_cfg* phy = &port->phy[i];
        /* Configure lane mapping. */
        ret = max96793_csi_config_sensor_to_ser_lane_map(dev, phy);
        if(ret != 0)
            return ret;

        /* Configure lane polarities. */
        ret = max96793_csi_config_sensor_lane_pol(dev, phy);
        if(ret != 0)
            return ret;
    }

    /* Enable PHY. */
    ret = gmsl_reg_write(dev->i2c_desc, START_PORTB_FRONTTOP_FRONTTOP_0_ADDR, FRONTTOP_FRONTTOP_0_DEFAULT);

    return ret;
}

/**
 * @brief Initialize the MAX96793 device.
 *
 * @param dev         - Pointer to the GMSL device structure.
 * @param tunnel_mode - Boolean to enable or disable tunnel mode.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t max96793_csi_init(struct gmsl_dev *dev, bool tunnel_mode)
{
    int ret;

    GMSL_LOG_INFO("MAX96793 Initialization.");

    ret = gmsl_reg_update(dev->i2c_desc, TUN_MODE_MIPI_RX_EXT_EXT11_ADDR, tunnel_mode, TUN_MODE_MIPI_RX_EXT_EXT11_MASK);
    if(ret != 0)
        return ret;

    /* Disable CSI PortB. */
    ret = gmsl_reg_update(dev->i2c_desc, START_PORTB_FRONTTOP_FRONTTOP_0_ADDR, 0x00, START_PORTB_FRONTTOP_FRONTTOP_0_MASK);
    if(ret != 0)
        return ret;

    /* Reset ClockZ. */
    ret = gmsl_reg_update(dev->i2c_desc, FRONTTOP_FRONTTOP_0_ADDR, 0x00, 0x02);
    if(ret != 0)
        return ret;

    /* Disable Pipe Z. */
    ret = gmsl_reg_update(dev->i2c_desc, START_PORTBZ_FRONTTOP_FRONTTOP_9_ADDR, 0x00, START_PORTBZ_FRONTTOP_FRONTTOP_9_MASK);
    if(ret != 0)
        return ret;

    /* Setting MIPI RX PHY phy-config to 1x4, one port with 4 data lanes*/
    ret = gmsl_reg_update(dev->i2c_desc, MIPI_RX_MIPI_RX0_ADDR, 0x00, 0x07);

    return ret;
}

/**
 * @brief Initialize the pipe for the MAX96793 device.
 *
 * @param dev  - Pointer to the GMSL device structure.
 * @param pipe - Pointer to the GMSL camera serializer pipe structure.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t max96793_csi_init_pipe(struct gmsl_dev *dev, struct gmsl_pipe_ser_pipe_cfg *pipe)
{
    int ret;

    GMSL_LOG_INFO("MAX96793 PIPE Initialization.");
    /* Enable pipe output to PHY -> set START_PORTBZ */
    ret = gmsl_reg_update(dev->i2c_desc, START_PORTBZ_FRONTTOP_FRONTTOP_9_ADDR, 0x01, START_PORTBZ_FRONTTOP_FRONTTOP_9_MASK);
    if(ret != 0)
        return ret;

    /* Set 8bit double mode. */
    ret = gmsl_reg_update(dev->i2c_desc, BPP8DBLZ_FRONTTOP_FRONTTOP_10_ADDR, pipe->dbl8 , BPP8DBLZ_FRONTTOP_FRONTTOP_10_MASK);
    if(ret != 0)
        return ret;

    /* Set 10bit double mode. */
    ret = gmsl_reg_update(dev->i2c_desc, BPP10DBLZ_FRONTTOP_FRONTTOP_11_ADDR, pipe->dbl10 , BPP10DBLZ_FRONTTOP_FRONTTOP_11_MASK);
    if(ret != 0)
        return ret;

    /* Set 12bit double mode. */
    ret = gmsl_reg_update(dev->i2c_desc, BPP12DBLZ_FRONTTOP_FRONTTOP_11_ADDR, pipe->dbl12 , BPP12DBLZ_FRONTTOP_FRONTTOP_11_MASK);
    if(ret != 0)
        return ret;

    /* Set soft BPP value. */
    ret = gmsl_reg_update(dev->i2c_desc, SOFT_BPPZ_FRONTTOP_FRONTTOP_22_ADDR, pipe->soft_bpp, SOFT_BPPZ_FRONTTOP_FRONTTOP_22_MASK);
    if(ret != 0)
        return ret;

    /* Enable override soft BPP. */
    ret = gmsl_reg_update(dev->i2c_desc, SOFT_BPPZ_EN_FRONTTOP_FRONTTOP_22_ADDR, (pipe->soft_bpp != 0U), SOFT_BPPZ_EN_FRONTTOP_FRONTTOP_22_MASK);
    if(ret != 0)
        return ret;

    ret = gmsl_reg_write(dev->i2c_desc, TX_STR_SEL_CFGV_VIDEO_Z_TX3_ADDR, (pipe->stream_id & TX_STR_SEL_CFGV_VIDEO_Z_TX3_MASK));
    if(ret != 0)
        return ret;

    ret = max96793_csi_set_pipe_enable(dev, true);

    return 0;
}
/**
 * @brief Configure the GPIO settings for the camera on the MAX96792 serializer.
 *
 * This function sets up the GPIO configuration for the camera connected to the
 * MAX96793 serializer. It ensures that the GPIO pins are correctly configured
 * for communication with the camera module.
 *
 * @param dev Pointer to the GMSL device structure.
 *
 * @return 0 on success, negative error code on PAL failure.
 */
int max96793_gpio_cam_cfg(struct gmsl_dev *dev)
{
    int ret = 0;
    do
    {
     REG_WRITE_BREAK_ON_ERR(ret, dev->i2c_desc, GPIO0_0_GPIO_A_ADDR, 0x90);
     no_os_mdelay(20);
     REG_WRITE_BREAK_ON_ERR(ret, dev->i2c_desc, GPIO0_0_GPIO_B_ADDR, 0X60);
     no_os_mdelay(20);
     REG_WRITE_BREAK_ON_ERR(ret, dev->i2c_desc, REF_VTG_REF_VTG1_ADDR, 0X89);
     no_os_mdelay(20);
     REG_WRITE_BREAK_ON_ERR(ret, dev->i2c_desc, MISC_PIO_SLEW_1_ADDR, 0X0C);
     no_os_mdelay(20);
    } while (false);
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