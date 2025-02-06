/*****************************************************************************
*   @file   max96792_csi.c
*   @brief  CSI Driver Implementation of MAX96792 Deserializer.
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
#include "max96792_csi.h"
#include "max96792_regs.h"
/** \addtogroup ADI_GMSL_CAM_DESERIALIZER GMSL Camera Deserializers
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_DES3 MAX96792
 *  @{
 */

/** \addtogroup ADI_GMSL_CAM_DES3_CFG_API Configuration APIs
 *  @{
 */

#ifndef ADI_DISABLE_DOCUMENTATION

/*================================== STATIC ==================================*/
/**
 * @brief        This API maps the source Data Type and Virtual Channel for a given pipe.
 *
 * @param        dev    Device handle
 * @param        pipe   Pointer to PIPEs.
 * @param        remap  Pointer to Data-type to VC remaps.
 * @param        vc_id  Virtual Channel Number.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_map_pipe_src_vc_dt(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe, struct gmsl_pipe_des_dt_vc_remap *remap, unsigned int vc_id)
{
    int reg, ret = 0;
    uint8_t index = (pipe->index) - 1; /* Adjust index to match Pipe Y & Pipe Z enum values with register offset */

    /** @todo: implement extended Virtual Channel. */
    /* For each pipe, adding offset of 2 for VC-DT SRC-DST register pair */
    reg = MAP_SRC_0_MIPI_TX_1_MIPI_TX13_ADDR + REMAP_SRC_DST_REG_DISPLACEMENT * index + vc_id * 2;
    ret = gmsl_reg_write(dev->i2c_desc, reg, GMSL_CAM_DES_DT_VC(remap->from_dt, remap->from_vc));

    return ret;
}

/**
 * @brief        This API maps the Destination Data Type and Virtual Channel for a given pipe.
 *
 * @param        dev    Device handle
 * @param        pipe   Pointer to PIPEs.
 * @param        remap  Pointer to Data-type to VC remaps.
 * @param        vc_id  Virtual Channel Number.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_map_pipe_dst_vc_dt(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe, struct gmsl_pipe_des_dt_vc_remap *remap, unsigned int vc_id)
{
    int reg, ret = 0;
    uint8_t index = (pipe->index) - 1; /* Adjust index to match Pipe Y & Pipe Z enum values with register offset */

    /** @todo: implement extended Virtual Channel. */
    /* For each pipe, adding offset of 2 for VC-DT SRC-DST register pair*/
    reg = MAP_DST_0_MIPI_TX_1_MIPI_TX14_ADDR + (REMAP_SRC_DST_REG_DISPLACEMENT * index) + (vc_id * 2);
    ret = gmsl_reg_write(dev->i2c_desc, reg, GMSL_CAM_DES_DT_VC(remap->from_dt, remap->to_vc));

    return ret;
}

/**
 * @brief        This API maps the source to destination controller for a given pipe.
 *
 * @param        dev    Device handle
 * @param        pipe   Pointer to PIPEs.
 * @param        remap  Pointer to Data-type to VC remaps.
 * @param        vc_id  Virtual Channel Number.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_map_pipe_src_dst_controller(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe, struct gmsl_pipe_des_dt_vc_remap *remap, unsigned int vc_id)
{
    int ret = 0;
    unsigned int reg;
    uint8_t vc_bit_pos, mask, val, index = (pipe->index) - 1; /* Adjust index to match Pipe Y & Pipe Z enum values with register offset */

    /*Each Register Contains 4 VCs information*/
    reg = MIPI_TX_1_MIPI_TX45_ADDR + (REMAP_SRC_DST_REG_DISPLACEMENT * index) + (vc_id / MAP_DPHY_DEST_MIPI_TX_NO_OF_VCS_FOR_REG);
    vc_bit_pos = (vc_id % MAP_DPHY_DEST_MIPI_TX_NO_OF_VCS_FOR_REG) * 2;
    mask = MAP_DPHY_DEST_MIPI_TX_MIPI_MASK << vc_bit_pos;
    val = (remap->phy & MAP_DPHY_DEST_MIPI_TX_MIPI_MASK);

    ret = gmsl_reg_update(dev->i2c_desc, reg, val, mask);

    return ret;
}

/**
 * @brief        This API enables the remap from source to destination for a given pipe.
 *
 * @param        dev     Device handle
 * @param        pipe    Pointer to PIPEs.
 * @param        remap   Pointer to Data-type to VC remaps.
 * @param        vc_id   Virtual Channel Number.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_map_pipe_src_to_dst_en(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe, struct gmsl_pipe_des_dt_vc_remap *remap, unsigned int vc_id)
{
    int reg, ret = 0;
    uint8_t mask, index = (pipe->index) - 1; /* Adjust index to match Pipe Y & Pipe Z enum values with register offset */

    mask = (1 << (vc_id % MAP_EN_SRC_DST_NO_OF_VCS_FOR_REG));

    /*Each Register Contains 8 VCs information*/
    reg = MAP_EN_L_MIPI_TX_1_MIPI_TX11_ADDR + (REMAP_SRC_DST_REG_DISPLACEMENT * index) + (vc_id / MAP_EN_SRC_DST_NO_OF_VCS_FOR_REG);
    ret = gmsl_reg_update(dev->i2c_desc, reg, (uint8_t)0x01, mask);

    return ret;
}

/**
 * @brief        This API initializes the Deserializer PIPE remaps.
 *
 * @param        dev    Device handle
 * @param        pipe   Pointer to PIPEs.
 * @param        remap  Pointer to Data-type to VC remaps.
 * @param        vc_id  Virtual Channel Number.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_init_pipe_remap(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe, struct gmsl_pipe_des_dt_vc_remap *remap, unsigned int vc_id)
{
    int ret = 0;

    do
    {
        /* Set source Data Type and Virtual Channel. */
        ret = max96792_csi_map_pipe_src_vc_dt(dev, pipe, remap, vc_id);
        BREAK_ON_ERR(ret);

        /* Set destination Data Type and Virtual Channel. */
        ret = max96792_csi_map_pipe_dst_vc_dt(dev, pipe, remap, vc_id);
        BREAK_ON_ERR(ret);

        /* Set destination PHY. */
        ret = max96792_csi_map_pipe_src_dst_controller(dev, pipe, remap, vc_id);
        BREAK_ON_ERR(ret);

        /* Enable remap. */
        ret = max96792_csi_map_pipe_src_to_dst_en(dev, pipe, remap, vc_id);
        BREAK_ON_ERR(ret);

    } while (false);

    return ret;
}

/**
 * @brief        This API sets the lane configuration for the MAX96792 deserializer.
 *
 * @param        dev    Device handle
 * @param        phy    Pointer to the PHY configuration structure.
 *
 * @return       return 0 in case of success, negative error code in case of errors.
 */
static int max96792_csi_set_lane_config(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0;
    unsigned int reg, index = phy->index;

    reg = MIPI_TX_0_MIPI_TX10_ADDR + (index * MIPI_TX_REG_OFFSET);

    ret = gmsl_reg_update(dev->i2c_desc, reg, (phy->mipi.num_data_lanes - 1), CSI2_LANE_CNT_MIPI_TX_0_MIPI_TX10_MASK);

    return ret;
}

/**
 * @brief        This API sets the lane mapping for a given PHY.
 *
 * @param        dev    Device handle
 * @param        phy    Pointer to PHY configuration.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_set_lane_mapping(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0;
    uint8_t val = 0;
    unsigned int mask, shift, reg, index = phy->index;
    mask = PHY0_LANE_MAP_MIPI_PHY_MIPI_PHY3_MASK;
    shift = 4 * (index % 2); /*Each Register carries 2 PHYs lane map information in 4 bit fields*/
    /*
     index 0->d0
     index 1->d1
     */
    val = (phy->mipi.data_lanes[MAX96792_DATA_LANE_0]) | ((phy->mipi.data_lanes[MAX96792_DATA_LANE_1])<< VALUE_2);
    reg = PHY0_LANE_MAP_MIPI_PHY_MIPI_PHY3_ADDR + (index / VALUE_2);

    ret = gmsl_reg_update(dev->i2c_desc, reg, val, mask << shift);

    return ret;
}

/**
 * @brief        This API sets the lane polarity for the MAX96792 deserializer.
 *
 * @param        dev    Device handle
 * @param        phy    Pointer to PHY configuration.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_set_lane_polarity(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0, val = 0;
    unsigned int reg, shift, index = phy->index;
    unsigned int indx, num_used_lanes = 2;
    if (!phy->cphy_enabled)
    {
        /*
         +1 was for CLock lane
          0-> d0
          1-> d1
          2-> clock lane
        */
        num_used_lanes += 1; /*Data lanes + Clock lane*/
    }

    shift = NO_OF_LANES_POLARITY * (index % VALUE_2);
    reg = PHY0_POL_MAP_MIPI_PHY_MIPI_PHY5_ADDR + (index / VALUE_2);

    for (indx = 0; indx < num_used_lanes; indx++)
        if (phy->mipi.lane_polarities[indx])
            val |= (1 << indx);

    ret = gmsl_reg_update(dev->i2c_desc, reg, val , PHY0_POL_MAP_MIPI_PHY_MIPI_PHY5_MASK << shift);

    return ret;
}

/**
 * @brief        Configures the deskew settings for the MAX96792 deserializer.
 *
 * @param        dev    Device handle.
 * @param        phy    Pointer to the PHY configuration structure.
 *
 * @return       Returns 0 on success, or a negative error code on PAL failure.
 */
static int max96792_csi_deskew_config(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0;
    unsigned int index = phy->index;

    do
    {
        if (phy->mipi_clk > DEFAULT_MIPI_CLK)
        {
            /* Enable initial deskew with 2 x 32k UI. */
            REG_WRITE_BREAK_ON_ERR(ret, dev->i2c_desc, DESKEW_INIT_MIPI_TX_1_MIPI_TX3_ADDR + MIPI_TX_REG_OFFSET * index, MIPI_TX_0_MIPI_TX4_DESKEW_MASK);

            /* Disable periodic deskew with 2 x 1k UI.. */
            REG_WRITE_BREAK_ON_ERR(ret, dev->i2c_desc, DESKEW_PER_MIPI_TX_1_MIPI_TX4_ADDR + MIPI_TX_REG_OFFSET * index, MIPI_TX_0_MIPI_TX4_DESKEW_MASK);
        }
        else
        {
            /* Disable initial deskew. */
            REG_WRITE_BREAK_ON_ERR(ret, dev->i2c_desc, DESKEW_INIT_MIPI_TX_1_MIPI_TX3_ADDR + MIPI_TX_REG_OFFSET * index, DISABLE_INITIAL_DESKEW);

            /* Disable periodic deskew. */
            REG_WRITE_BREAK_ON_ERR(ret, dev->i2c_desc, DESKEW_PER_MIPI_TX_1_MIPI_TX4_ADDR + MIPI_TX_REG_OFFSET * index, DISABLE_PERIODIC_DESKEW);
        }
    } while (false);

    return ret;
}

/**
 * @brief        This API sets the PHY rate for the MAX96792 deserializer.
 *
 * @param        dev    Device handle
 * @param        phy    Pointer to PHY configuration structure.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_set_phy_rate(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0;
    unsigned int reg_freq, reg_res;
    unsigned int index = phy->index;

    do
    {
        if ((index == GMSL_LINK_MIPI_CSI_CTRLR_0) || (index == GMSL_LINK_MIPI_CSI_CTRLR_1))
        {
            reg_res = CONFIG_SOFT_RST_N_DPLL_CSI2_DPLL_0_ADDR;
        }
        else if ((index == GMSL_LINK_MIPI_CSI_CTRLR_2) || (index == GMSL_LINK_MIPI_CSI_CTRLR_3))
        {
            reg_res = CONFIG_SOFT_RST_N_DPLL_CSI3_DPLL_0_ADDR;
        }
        else
        {
            /* Do Nothing */
        }

        /* Release DPLL block reset */
        REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, reg_res, 0x00, CONFIG_SOFT_RST_N_DPLL_CSI1_DPLL_0_MASK);

        reg_freq = PHY0_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP22_ADDR + 0x3 * index;

        /* Set DPLL frequency. */
        REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, reg_freq, (phy->mipi_clk / VALUE_100), PHY0_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP22_MASK);

        /* Enable DPLL frequency. */
        REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, reg_freq, 0X01, PHY0_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP22_MASK);

        /* Pull DPLL block out of reset. */
        REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, reg_res, 0X01, CONFIG_SOFT_RST_N_DPLL_CSI1_DPLL_0_MASK);

    } while (false);

    return ret;
}

/**
 * @brief        This API configures the alternate memory map mode for the MAX96792 deserializer.
 *
 * @param        dev    Device handle
 * @param        phy    Pointer to PHY configuration.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_alt_mem_map_mode(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0;
    unsigned int reg, index = phy->index;
    uint8_t val = 0x00;

    /* Set alternate memory map modes. */
    if (phy->alt_mem_map12) 
    {
        val |= 1 << ALT_MEM_MAP12_MIPI_TX_1_MIPI_TX51_POS;
    }
    if (phy->alt_mem_map8) 
    {
        val |= 1 << ALT_MEM_MAP8_MIPI_TX_1_MIPI_TX51_POS;
    }
    if (phy->alt_mem_map10) 
    {
        val |= 1 << ALT_MEM_MAP10_MIPI_TX_1_MIPI_TX51_MASK;
    }
    if (phy->alt2_mem_map8) 
    {
        val |= 1 << ALT2_MEM_MAP8_MIPI_TX_1_MIPI_TX51_MASK;
    }
    reg = MIPI_TX_1_MIPI_TX51_ADDR + MIPI_TX_REG_OFFSET * index;

    ret = gmsl_reg_update(dev->i2c_desc, reg, val, MIPI_TX_ALT_MEM_MAP_MASK);

    return ret;
}

/**
 * @brief        This API Enables the PHY  for the MAX96792 deserializer.
 *
 * @param        dev    Device handle
 * @param        phy    Pointer to PHY configuration.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_csi_enable_phy(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0;
    uint8_t phy_stdby = 1 << (phy->index);
    uint8_t reg_val;

    do
    {
        /*  Read the current value from the register */
        REG_READ_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_STDBY_N_MIPI_PHY_MIPI_PHY2_ADDR,0xFF, &reg_val);

        /* Modify the specific bit field */
        reg_val |= (phy_stdby<<PHY_STDBY_N_MIPI_PHY_MIPI_PHY2_POS);

        /* Write the modified value back to the register */
        REG_WRITE_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_STDBY_N_MIPI_PHY_MIPI_PHY2_ADDR, reg_val);

    } while (false);

    return ret; 
}

/**
 * @brief        This API enables or disables video pipes for a given device.
 *
 * @param        dev           Device handle
 * @param        pipe          Pointer to PIPEs.
 * @param        disable_pipes Boolean flag to disable pipes.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_pipe_enable_vid_pipes(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe)
{
    unsigned int mask = (1 << ((pipe->index) - 1));
    int ret = 0;

    if (pipe->enabled)
    {
        ret = gmsl_reg_update(dev->i2c_desc, VIDEO_PIPE_SEL_VIDEO_PIPE_EN_ADDR, 0x01, mask);
    }
    else
    {
        ret = gmsl_reg_update(dev->i2c_desc, VIDEO_PIPE_SEL_VIDEO_PIPE_EN_ADDR, 0x00, mask);
    }

    return ret;
}

/**
 * @brief        This API sets the 10-bit double mode for a given pipe.
 *
 * @param        dev   Device handle
 * @param        pipe  Pointer to PIPEs.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_pipe_set_10bit_double_mode(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe)
{
    uint8_t dbl10_mask, dbl10mode_mask;
    bool dbl10_val = true, dbl10mode_val = true;
    enum gmsl_pipe pipe_id = pipe->index;
    unsigned int reg;
    int ret = 0;

    /* Set 10bit double mode. */
    do
    {
        if (pipe_id == GMSL_PIPE_Y)
        {
            reg = BPP10DBLY_BACKTOP_BACKTOP32_ADDR;
            dbl10_mask = BPP10DBLY_BACKTOP_BACKTOP32_MASK;
            dbl10mode_mask = BPP10DBLY_MODE_BACKTOP_BACKTOP32_MASK;
        }
        else if (pipe_id == GMSL_PIPE_Z)
        {
            reg = BPP10DBLZ_BACKTOP_BACKTOP32_ADDR;
            dbl10_mask = BPP10DBLZ_BACKTOP_BACKTOP32_MASK;
            dbl10mode_mask = BPP10DBLZ_MODE_BACKTOP_BACKTOP32_MASK;
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, " Invalid Pipe.");
            break;
        }

        if (!pipe->dbl10)
        {
            dbl10_val = false;
        }
        else
        {
            /* Do Nothing */
        }

        if (!pipe->dbl10mode)
        {
            dbl10mode_val = false;
        }
        else
        {
            /* Do Nothing */
        }

        REG_UPDATE_BREAK_ON_ERR(ret,dev->i2c_desc, reg, dbl10_val, dbl10_mask);
        REG_UPDATE_BREAK_ON_ERR(ret,dev->i2c_desc, reg, dbl10mode_val, dbl10mode_mask);

    } while (false);

    return ret;
}

/**
 * @brief        This API sets the 8-bit double mode for a given pipe.
 *
 * @param        dev   Device handle
 * @param        pipe  Pointer to PIPEs.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_pipe_set_8bit_double_mode(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe)
{
    uint8_t dbl8_mask, dbl8mode_mask;
    bool dbl8_val = true, dbl8mode_val = true;
    enum gmsl_pipe pipe_id = pipe->index;
    unsigned int reg;
    int ret = 0;

    /* Set 8bit double mode. */
    do
    {
        if (pipe_id == GMSL_PIPE_Y)
        {
            reg = BPP8DBLY_BACKTOP_BACKTOP21_ADDR;
            dbl8_mask = BPP8DBLY_BACKTOP_BACKTOP21_MASK;
            dbl8mode_mask = BPP8DBLY_MODE_BACKTOP_BACKTOP24_MASK;
        }
        else if (pipe_id == GMSL_PIPE_Z)
        {
            reg = BPP8DBLZ_BACKTOP_BACKTOP21_ADDR;
            dbl8_mask = BPP8DBLZ_BACKTOP_BACKTOP21_MASK;
            dbl8mode_mask = BPP8DBLZ_MODE_BACKTOP_BACKTOP24_MASK;
        }
        else
        {
            GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, " Invalid Pipe.");
            break;
        }

        if (!pipe->dbl8)
        {
            dbl8_val = false;
        }
        else
        {
            /* Do Nothing */
        }

        if (!pipe->dbl8mode)
        {
            dbl8mode_val = false;
        }
        else
        {
            /* Do Nothing */
        }

        REG_UPDATE_BREAK_ON_ERR(ret,dev->i2c_desc, reg, dbl8_val, dbl8_mask);
        REG_UPDATE_BREAK_ON_ERR(ret,dev->i2c_desc, reg, dbl8mode_val, dbl8mode_mask);

    } while (false);

    return ret;
}

/**
 * @brief        This API sets the 12-bit double lane mode for a given pipe.
 *
 * @param        dev    Device handle
 * @param        pipe   Pointer to PIPEs configuration.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_pipe_set_12bit_double_mode(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe)
{
    int ret = 0;
    unsigned int mask = 1 << pipe->index;
    uint8_t dbl12 = pipe->dbl12;

    ret = gmsl_reg_update(dev->i2c_desc, BPP12DBLY_BACKTOP_BACKTOP33_ADDR, dbl12 , mask);
    return ret;
}

/**
 * @brief        This API enables the stream select for a given pipe.
 *
 * @param        dev                     Device handle
 * @param        pipe                    Pointer to PIPEs.
 * @param        pipe_stream_autoselect  Boolean flag to enable/disable auto stream select.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
static int max96792_pipe_enable_stream_select(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe, bool pipe_stream_autoselect)
{
    unsigned int reg, stream_id;
    enum gmsl_pipe pipe_id = pipe->index;
    uint8_t  mask = (1 << (pipe_id - 1));
    int ret = 0;
    do
    {
        REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, VIDEO_PIPE_SEL_VIDEO_PIPE_EN_ADDR, 0x01, mask);

        if (!pipe_stream_autoselect)
        {

            if (pipe_id == GMSL_PIPE_Y)
            {
                reg = VIDEO_PIPE_SEL_Y_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_ADDR;
                mask = VIDEO_PIPE_SEL_Y_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_MASK;
            }
            else if (pipe_id == GMSL_PIPE_Z)
            {
                reg = VIDEO_PIPE_SEL_Z_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_ADDR;
                mask = VIDEO_PIPE_SEL_Z_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_MASK;
            }
            else
            {
                GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, " Invalid Pipe.");
                break;
            }

            stream_id = (pipe->stream_id) | (pipe->link_id << 2u);
            REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, reg, stream_id, mask);
        }

    } while (false);

    return ret;
}
#endif /*ADI_DISABLE_DOCUMENTATION */
/******************************************************************************/
/************************* Functions Definition *******************************/
/******************************************************************************/

/**
 * @brief        This API updates the Deserializer PIPE remaps.
 *
 * @param        dev   Device handle
 * @param        pipe  Pointer to PIPEs.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_csi_update_pipe_remaps(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe)
{
    uint8_t vc_id;
    struct gmsl_pipe_des_dt_vc_remap *remap;
    struct gmsl_dev_csi_des_init_param *init_param = (struct gmsl_dev_csi_des_init_param *)dev->dev_config;
    int ret = 0;

    if (!init_param->tunnel_mode)
    {
        for (vc_id = 0; vc_id < pipe->num_remaps; vc_id++)
        {
            remap = &pipe->remaps[vc_id];
            ret = max96792_csi_init_pipe_remap(dev, pipe, remap, vc_id);
            BREAK_ON_ERR(ret);
        }
    }
    return ret;
}

/**
 * @brief        This API selects/enables the Deserializer LINKs.
 *
 * @param        dev   Device handle
 * @param        mask  Bitmask representing the CSI links to be selected.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_csi_select_links(struct gmsl_dev *dev, unsigned int mask)
{
    int ret = 0;

    do
    {
        REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, GMSL1_COMMON_GMSL1_EN_ADDR, mask, LINK_EN_A_GMSL1_COMMON_GMSL1_EN_MASK | LINK_EN_B_GMSL1_COMMON_GMSL1_EN_MASK);
        no_os_mdelay(60);

    } while (false);

    return ret;
}

/**
 * @brief      This API enables Remote control channels of Deserializer device.
 *
 * @param        dev     Device handle
 * @param        link     Deserializer PHY/Link enum.
 * @param        enable   Enable/Disable Control Channel.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_cc_enable_remote_control_chnl(struct gmsl_dev *dev, enum gmsl_link link, bool enable)
{
    int ret = 0;
    unsigned int reg, mask;

    switch (link)
    {
    case GMSL_LINK_A:
        reg = DIS_REM_CC_DEV_REG1_ADDR;
        mask = DIS_REM_CC_DEV_REG1_MASK;
        break;
    case GMSL_LINK_B:
        reg = DIS_REM_CC_B_DEV_REG3_ADDR;
        mask = DIS_REM_CC_B_DEV_REG3_MASK;
        break;
    default:
        GMSL_LOG_DEV_INFO(dev->dev_name, dev->i2c_desc->slave_address, " Invalid link.");
        return -EINVAL;
    }
    ret = gmsl_reg_update(dev->i2c_desc, reg, (uint8_t)!enable, mask);

    return ret;
}

/**
 * @brief        This API enables the MIPI CSI OUT from the Deserializer.
 *
 * @param        dev     Device handle
 * @param        enable  enable/disable MIPI.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_csi_enable_mipi_out(struct gmsl_dev *dev, bool enable)
{
    int ret = 0;

    if (enable)
    {
        ret = gmsl_reg_update(dev->i2c_desc, CSI_OUT_EN_BACKTOP_BACKTOP12_ADDR, 0x01, CSI_OUT_EN_BACKTOP_BACKTOP12_MASK);
    }
    else
    {
        ret = gmsl_reg_update(dev->i2c_desc, CSI_OUT_EN_BACKTOP_BACKTOP12_ADDR, 0x00, CSI_OUT_EN_BACKTOP_BACKTOP12_MASK);
    }

    return ret;
}

/**
 * @brief        Initializes the CSI pipe for the MAX96792 deserializer.
 *
 * @param        dev                     Device handle.
 * @param        pipe_stream_autoselect  Boolean flag to enable or disable pipe stream auto-selection.
 * @param        pipe                    Pointer to the pipe configuration structure.
 *
 * @return       Returns 0 on success, or a negative error code on failure.
 */
int max96792_csi_init_pipe(struct gmsl_dev *dev, bool pipe_stream_autoselect, struct gmsl_pipe_des_pipe_cfg *pipe)
{
    int ret = 0;
    struct gmsl_dev_csi_des_init_param *init_param = (struct gmsl_dev_csi_des_init_param *)dev->dev_config;
    do
    {
        if(pipe->enabled)
        {
            /* Enable Video Pipes. */
            ret = max96792_pipe_enable_vid_pipes(dev, pipe);
            BREAK_ON_ERR(ret);

            if (!(init_param->tunnel_mode))
            {
                /* Set 10bit double mode. */
                ret = max96792_pipe_set_10bit_double_mode(dev, pipe);
                BREAK_ON_ERR(ret);

                /* Set 8bit double mode. */
                ret = max96792_pipe_set_8bit_double_mode(dev, pipe);
                BREAK_ON_ERR(ret);

                /* Set 12bit double mode. */
                ret = max96792_pipe_set_12bit_double_mode(dev, pipe);
                BREAK_ON_ERR(ret);
            }
            /* Enable stream select. */
            ret = max96792_pipe_enable_stream_select(dev, pipe, pipe_stream_autoselect);
            BREAK_ON_ERR(ret);
        }
    } while (false);

    return ret;
}

/**
 * @brief        This API initializes the PHY for the MAX96792 deserializer.
 *
 * @param        dev  Device handle
 * @param        phy  Pointer to PHY configuration structure.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_csi_init_phy(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0;
    if (phy->enabled)
    {
        if (phy->cphy_enabled)
        {
            ret = max96792_csi_set_cphy_config(dev, phy);
        }
        else
        {
            ret = max96792_csi_set_dphy_config(dev, phy);
        }
    }
    return ret;
}

/**
 * @brief        This API sets the C-PHY configuration for the MAX96792 deserializer.
 *
 * @param        dev  Device handle
 * @param        phy  Pointer to the PHY configuration structure.
 *
 * @return       Returns 0 in case of success, negative error code in case of PAL failure.
 */
int max96792_csi_set_cphy_config(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0;
    unsigned int index = phy->index;
    struct gmsl_dev_csi_des_init_param *init_param = (struct gmsl_dev_csi_des_init_param *)dev->dev_config;
    do
    {
        REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, CSI2_CPHY_EN_MIPI_TX_0_MIPI_TX10_ADDR + MIPI_TX_REG_OFFSET * index, 0x01, CSI2_CPHY_EN_MIPI_TX_0_MIPI_TX10_MASK);

        ret = max96792_csi_set_lane_config(dev, phy);
        BREAK_ON_ERR(ret);

        ret = max96792_csi_set_lane_mapping(dev, phy);
        BREAK_ON_ERR(ret);

        ret = max96792_csi_set_lane_polarity(dev, phy);
        BREAK_ON_ERR(ret);

        ret = max96792_csi_set_phy_rate(dev, phy);
        BREAK_ON_ERR(ret);

        if (!(init_param->tunnel_mode))
        {
            ret = max96792_csi_alt_mem_map_mode(dev, phy);
            BREAK_ON_ERR(ret);
        }

        ret = max96792_csi_enable_phy(dev, phy);
        BREAK_ON_ERR(ret);

    } while (false);
    return ret;
}

/**
 * @brief        This API sets the D-PHY configuration for the MAX96792 deserializer.
 *
 * @param        dev  Device handle
 * @param        phy  Pointer to the PHY configuration structure.
 *
 * @return       Returns 0 in case of success, negative error code in case of PAL failure.
 */
int max96792_csi_set_dphy_config(struct gmsl_dev *dev, struct gmsl_link_des_phy_cfg *phy)
{
    int ret = 0;
    struct gmsl_dev_csi_des_init_param *init_param = (struct gmsl_dev_csi_des_init_param *)dev->dev_config;
    do
    {
        ret = max96792_csi_set_lane_config(dev, phy);
        BREAK_ON_ERR(ret);

        ret = max96792_csi_set_lane_mapping(dev, phy);
        BREAK_ON_ERR(ret);

        ret = max96792_csi_set_lane_polarity(dev, phy);
        BREAK_ON_ERR(ret);

        ret = max96792_csi_deskew_config(dev, phy);
        BREAK_ON_ERR(ret);

        ret = max96792_csi_set_phy_rate(dev, phy);
        BREAK_ON_ERR(ret);

        if (!(init_param->tunnel_mode))
        {
            ret = max96792_csi_alt_mem_map_mode(dev, phy);
            BREAK_ON_ERR(ret);
        }

        ret = max96792_csi_enable_phy(dev, phy);
        BREAK_ON_ERR(ret);

    } while (false);
    return ret;
}

/**
 * @brief        This API sets the RX link rate for the MAX96792 deserializer.
 *
 * @param        dev         Device handle
 * @param        init_param  Pointer to initialization parameters
 * @param        mask        Mask to select GMSL links
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_csi_set_rx_link_rate(struct gmsl_dev *dev, struct gmsl_dev_csi_des_init_param *init_param, unsigned int mask)
{
    int ret = 0;

    do
    {
        if (mask & LINK_EN_A_GMSL1_COMMON_GMSL1_EN_MASK)
        {
            ret = gmsl_reg_update(dev->i2c_desc, RX_RATE_DEV_REG1_ADDR, init_param->link_rx_rate[GMSL_LINK_A], RX_RATE_DEV_REG1_MASK);
            BREAK_ON_ERR(ret);
        }

        if (mask & LINK_EN_B_GMSL1_COMMON_GMSL1_EN_MASK)
        {
            ret = gmsl_reg_update(dev->i2c_desc, RX_RATE_B_DEV_REG4_ADDR, init_param->link_rx_rate[GMSL_LINK_B], RX_RATE_B_DEV_REG4_MASK);
            BREAK_ON_ERR(ret);
        }

    } while (false);

    return ret;
}

/**
 * @brief        Initializes the MAX96792 CSI Deserializer.
 *
 * @param        dev          Device handle.
 *
 * @return       Returns 0 on success, or a negative error code on PAL failure.
 */
int max96792_csi_init(struct gmsl_dev *dev)
{
    int ret;

    do
    {
        /* Disable all PHYs. */
        ret = gmsl_reg_update(dev->i2c_desc, PHY_STDBY_N_MIPI_PHY_MIPI_PHY2_ADDR, 0x00, PHY_STDBY_N_MIPI_PHY_MIPI_PHY2_MASK);
        BREAK_ON_ERR(ret);

        /* Disable all pipes. */
        ret = gmsl_reg_update(dev->i2c_desc, VIDEO_PIPE_SEL_VIDEO_PIPE_EN_ADDR, 0x00, VIDEO_PIPE_EN_VIDEO_PIPE_SEL_VIDEO_PIPE_EN_MASK);
        BREAK_ON_ERR(ret);

        /* lane config Not needed By default its configured as 2(1x4)*/
    } while (false);

    return ret;
}

/**
 * @brief        This API controls the MIPI tunnel pipe for a given pipe.
 *
 * @param        dev            Device handle
 * @param        pipe           Pointer to PIPEs.
 * @param        dst_cntroller  Destination controller enum.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_mipi_tunnel_pipe_cntrl(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe, enum gmsl_link_mipi_csi_controller dst_cntroller)
{
    int ret = 0;
    unsigned int reg, index = (pipe->index) - 1;

    reg = TUN_EN_MIPI_TX_1_MIPI_TX52_ADDR + (index * TUN_EN_MIPI_TX_MIPI_REG_DISPLACEMENT);

    ret = gmsl_reg_update(dev->i2c_desc, reg, (uint8_t)dst_cntroller, TUN_DEST_MIPI_TX_1_MIPI_TX52_MASK);

    return ret;
}

/**
 * @brief        This API sets the PHY TX mode for a given pipe.
 *
 * @param        dev             Device handle
 * @param        pipe            Pointer to PIPEs.
 *
 * @return       return 0 in case of success, negative error code in case of PAL errors.
 */
int max96792_pipe_set_tx_mode(struct gmsl_dev *dev, struct gmsl_pipe_des_pipe_cfg *pipe)
{
    int ret = 0;
    unsigned int reg, index = (pipe->index) - 1;
    struct gmsl_dev_csi_des_init_param *init_param = (struct gmsl_dev_csi_des_init_param *)dev->dev_config;

    reg = TUN_EN_MIPI_TX_1_MIPI_TX52_ADDR + (index * TUN_EN_MIPI_TX_MIPI_REG_DISPLACEMENT);

    ret = gmsl_reg_update(dev->i2c_desc, reg, (uint8_t)init_param->tunnel_mode, TUN_EN_MIPI_TX_1_MIPI_TX52_MASK); /*tunnel_mode_en serializer tx mode will be updated here*/

    return ret;
}

/**
 * @brief        This API copies the PHY configuration from source controller to destination controller.
 *
 * @param        dev        Device handle.
 * @param        copy_en    Enable or disable the copy operation.
 * @param        src_cont   Source PHY controller.
 * @param        dest_cont  Destination PHY controller.
 *
 * @return       Returns 0 in case of success, negative error code in case of PAL failure.
 */
int max96792_csi_copy0_phy_cfg(struct gmsl_dev *dev, bool copy_en, enum gmsl_link_mipi_csi_controller *src_cont, enum gmsl_link_mipi_csi_controller *dest_cont)
{
    int ret = 0;

    do
    {
        /*Enable Copy */
        REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_CP0_MIPI_PHY_MIPI_PHY6_ADDR, copy_en, PHY_CP0_MIPI_PHY_MIPI_PHY6_MASK);

        if (copy_en)
        {
            /*Set Source Controller*/
            REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_CP0_SRC_MIPI_PHY_MIPI_PHY10_ADDR, *src_cont, PHY_CP0_SRC_MIPI_PHY_MIPI_PHY10_MASK);

            /*Set Destination Controller*/
            REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_CP0_DST_MIPI_PHY_MIPI_PHY9_ADDR, *dest_cont, PHY_CP0_DST_MIPI_PHY_MIPI_PHY9_MASK);
        }

    } while (false);

    return ret;
}

/**
 * @brief        This API copies the PHY configuration from source controller to destination controller.
 *
 * @param        dev        Device handle.
 * @param        copy_en    Enable or disable the copy operation.
 * @param        src_cont   Source PHY controller.
 * @param        dest_cont  Destination PHY controller.
 *
 * @return       Returns 0 in case of success, negative error code in case of PAL failure.
 */
int max96792_csi_copy1_phy_cfg(struct gmsl_dev *dev, bool copy_en, enum gmsl_link_mipi_csi_controller *src_cont, enum gmsl_link_mipi_csi_controller *dest_cont)
{
    int ret = 0;

    do
    {
        /*Enable Copy */
        REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_CP1_MIPI_PHY_MIPI_PHY6_ADDR, copy_en, PHY_CP1_MIPI_PHY_MIPI_PHY6_MASK);

        if (copy_en)
        {
            /*Set Source Controller*/
            REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_CP1_SRC_MIPI_PHY_MIPI_PHY12_ADDR, *src_cont, PHY_CP1_SRC_MIPI_PHY_MIPI_PHY12_MASK);

            /*Set Destination Controller*/
            REG_UPDATE_BREAK_ON_ERR(ret, dev->i2c_desc, PHY_CP1_DST_MIPI_PHY_MIPI_PHY11_ADDR, *dest_cont, PHY_CP1_DST_MIPI_PHY_MIPI_PHY11_MASK);
        }
    } while (false);

    return ret;
}

int max96792_csi_update_link_config(struct gmsl_dev *dev, unsigned int mask)
{
    int ret = 0;
    ret = gmsl_reg_update(dev->i2c_desc, LINK_CFG_TCTRL_CTRL0_ADDR, mask, LINK_CFG_TCTRL_CTRL0_MASK);
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