/*!
 * \brief     APIs for JESD Rx (JRx)
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_JRX
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_utils.h"
#include "adi_apollo_jrx.h"

#include "adi_apollo_bf_jrx_jesd_l0.h"
#include "adi_apollo_bf_jrx_phy_ifx.h"
#include "adi_apollo_bf_jrx_core.h"
#include "adi_apollo_bf_jrx_wrapper.h"
#include "adi_apollo_bf_jrx_dac_sample_prbs.h"
#include "adi_apollo_bf_jrx_dl_204c.h"
#include "adi_apollo_bf_jrx_dl_204b.h"
#include "adi_apollo_bf_custom.h"
#include "adi_apollo_bf_jrx_jesd_l0.h"
#include "adi_apollo_bf_custom.h"

#include "adi_apollo_serdes.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

static uint32_t calc_jrx_core_base(int32_t link);
static uint32_t calc_jrx_wrapper_base(int32_t link);
static uint32_t calc_jrx_dl_204c_base(int32_t link);
static uint32_t calc_jrx_dl_204b_base(int32_t link);
static uint32_t calc_jrx_jesd_l0_base(int32_t link);
static void calc_j204c_irq_en(uint32_t base, int32_t link, uint32_t idx, uint32_t *address, uint32_t *info);
static void calc_j204c_irq(uint32_t base, int32_t link, uint32_t idx, uint32_t *address, uint32_t *info);
static uint32_t calc_jrx_phy_ifx_base(int32_t link);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_jrx_link_enable_set(adi_apollo_device_t *device,
    const uint16_t links,
    uint8_t link_en)
{
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint8_t i, link_enable = 0;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i++) {
        if ((1 << i) & links) {

            regmap_base_addr = calc_jrx_wrapper_base(i);
            err = adi_apollo_hal_bf_get(device, BF_LINK_EN_INFO(regmap_base_addr), &link_enable, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            if(i%ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE == 0) {
                link_enable &= ~(0x01);
                link_enable |= link_en;
            } else {
                link_enable &= ~(0x02);
                link_enable |= (link_en << 1);
            }
            err = adi_apollo_hal_bf_set(device, BF_LINK_EN_INFO(regmap_base_addr), link_enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_lane_fifo_status(adi_apollo_device_t *device,
    const uint16_t link,
    uint32_t *status)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint32_t res = 0x00;
    uint16_t lane_fifo_empty;
    uint16_t lane_fifo_full;
    uint8_t link_index = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(link) != 1); // Check if only one link is selected
    
    link_index  = adi_api_utils_select_lsb_get(link);

    regmap_base_addr = calc_jrx_wrapper_base(link_index);

    err = adi_apollo_hal_bf_get(device, BF_LANE_FIFO_EMPTY_INFO(regmap_base_addr), (uint8_t *)&lane_fifo_empty, 2);
    ADI_APOLLO_ERROR_RETURN(err);
    res |= lane_fifo_empty & 0x0FFF;

    err = adi_apollo_hal_bf_get(device, BF_LANE_FIFO_FULL_INFO(regmap_base_addr), (uint8_t *)&lane_fifo_full, 2);
    ADI_APOLLO_ERROR_RETURN(err);
    res |= (lane_fifo_full << 16) & 0x0FFF0000;

    *status = res;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_rm_fifo_status(adi_apollo_device_t *device,
    const uint16_t link,
    uint16_t *status)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint16_t res = 0x00;
    uint8_t  reg8;
    uint8_t link_index = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);
    //ADI_APOLLO_INVALID_PARAM_RETURN(link_side > ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(link) != 1); // Check if only one link is selected
    
    link_index  = adi_api_utils_select_lsb_get(link);
    

    regmap_base_addr = calc_jrx_wrapper_base(link_index);

    err = adi_apollo_hal_reg_get(device, REG_RM_FIFO_STATUS_ADDR(regmap_base_addr), &reg8); /*  RM FIFO status */
    ADI_APOLLO_ERROR_RETURN(err);
    if(link_index % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE == 0) {
        res |= reg8 & 0x01;
        res |= (reg8 >> 1) & 0x02;
        res |= (reg8 >> 2) & 0x04;
    } else {
        res |= (reg8 >> 1) & 0x01;
        res |= (reg8 >> 2) & 0x02;
        res |= (reg8 >> 2) & 0x04;
    }

    *status = res;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_rm_fifo_reset(adi_apollo_device_t *device, 
    const uint16_t link_sides)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i += ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            regmap_base_addr = calc_jrx_wrapper_base(i);
            err = adi_apollo_hal_bf_set(device, BF_RM_FIFO_RESET_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_RM_FIFO_RESET_INFO(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_RM_FIFO_RESET_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_j204c_lane_status_get(adi_apollo_device_t *device,
    const uint16_t link,
    const uint16_t lane,
    uint16_t *status)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint16_t res = 0x00;
    uint8_t  reg8;
    uint8_t link_index = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);
    ADI_APOLLO_INVALID_PARAM_RETURN(lane >= ADI_APOLLO_JESD_DESER_COUNT); // Check if lane is in range
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(link) != 1); // Check if only one link is selected
    
    link_index  = adi_api_utils_select_lsb_get(link);

    regmap_base_addr = calc_jrx_dl_204c_base(link_index);

    err = adi_apollo_hal_bf_get(device, BF_JRX_DL_204C_STATE_INFO(regmap_base_addr, lane), &reg8, 1); /*  204C Lane status register */
    ADI_APOLLO_ERROR_RETURN(err);
    res |= (reg8 & 0x07);

    regmap_base_addr = calc_jrx_core_base(link_index);

    if (link_index % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE == 0) {
        err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_PCLK_SLOW_ERROR_LINK0_INFO(regmap_base_addr, lane), &reg8, 1);
        res |= (reg8 << 4) & 0x10;

        err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_PCLK_FAST_ERROR_LINK0_INFO(regmap_base_addr, lane), &reg8, 1);
        res |= (reg8 << 5) & 0x20;
    } else {

        err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_PCLK_SLOW_ERROR_LINK1_INFO(regmap_base_addr, lane), &reg8, 1);
        res |= (reg8 << 4) & 0x10;

        err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_PCLK_FAST_ERROR_LINK1_INFO(regmap_base_addr, lane), &reg8, 1);
        res |= (reg8 << 5) & 0x20;
    }

    *status = res;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_link_status_get(adi_apollo_device_t *device,
    const uint16_t link,
    uint16_t *status)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t  reg8;
    uint16_t res = 0x00;
    uint8_t link_index = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(link) != 1); // Check if only one link is selected
    
    link_index  = adi_api_utils_select_lsb_get(link);

    regmap_base_addr = calc_jrx_wrapper_base(link_index);

    err = adi_apollo_hal_bf_get(device, BF_JESD_MODES_NOT_IN_TABLE_INFO(regmap_base_addr, link_index % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), &reg8, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    res |= (reg8 & 0x01);       // & ADI_APOLLO_JRX_LINK_STAT_NOT_IN_TABLE

    err = adi_apollo_hal_bf_get(device, BF_DOWN_SCALE_OVERFLOW_INFO(regmap_base_addr, link_index % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), &reg8, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    res |= (reg8 << 1) & ADI_APOLLO_JRX_LINK_STAT_SCALE_OVERFLOW;

    regmap_base_addr = calc_jrx_core_base(link);

    err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_USR_DATA_RDY_INFO(regmap_base_addr, link_index % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), &reg8, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    res |= (reg8 << 5) & ADI_APOLLO_JRX_LINK_STAT_USR_DAT_RDY;

    err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_SYSREF_RCVD_INFO(regmap_base_addr, link_index % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), &reg8, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    res |= (reg8 << 6) & ADI_APOLLO_JRX_LINK_STAT_SYSREF_RECV;

    err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_CFG_INVALID_INFO(regmap_base_addr, link_index % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), &reg8, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    res |= (reg8 << 7) & ADI_APOLLO_JRX_LINK_STAT_CFG_INVALID;

    *status = res;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_j204b_lane_status_get(adi_apollo_device_t *device,
    const uint16_t link,
    const uint16_t lane,
    uint16_t *status)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint16_t res = 0x00;
    uint8_t  reg8;
    uint8_t link_index = 0; 

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);
    ADI_APOLLO_INVALID_PARAM_RETURN(lane >= ADI_APOLLO_JESD_DESER_COUNT); // Check if lane is in range
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(link) != 1); // Check if only one link is selected
    
    link_index  = adi_api_utils_select_lsb_get(link);
    
    regmap_base_addr = calc_jrx_dl_204b_base(link_index);

    err = adi_apollo_hal_reg_get(device, REG_JRX_DL_204B_LANE_STATUS_ADDR(regmap_base_addr, lane), &reg8);

    ADI_APOLLO_ERROR_RETURN(err);
    res |= reg8 & 0x3F;

    *status = res;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_j204b_lane_error_get(adi_apollo_device_t *device,
    const uint16_t link,
    const uint16_t lane,
    uint16_t *status)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t  reg8;
    uint16_t res = 0x00;
    uint8_t link_index = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);
    ADI_APOLLO_INVALID_PARAM_RETURN(lane >= ADI_APOLLO_JESD_DESER_COUNT); // Check if lane is in range
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(link) != 1); // Check if only one link is selected
    
    link_index  = adi_api_utils_select_lsb_get(link);

    regmap_base_addr = calc_jrx_dl_204b_base(link_index);

    err = adi_apollo_hal_reg_get(device, REG_JRX_DL_204B_LANE_ERR_STATUS_ADDR(regmap_base_addr, lane), &reg8);

    ADI_APOLLO_ERROR_RETURN(err);
    res |= reg8;

    *status = res;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_lr_adapt_get(adi_apollo_device_t *device,
                                        uint8_t link_sides,
                                        uint8_t physical_lane,
                                        uint8_t *lr_adapt)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(lr_adapt);
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JRX_LANES_MAX - 1);


    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i += ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            regmap_base_addr = calc_jrx_phy_ifx_base(i);
            err = adi_apollo_hal_bf_get(device, BF_JRX_IFX_LOG2_SPLIT_INFO(regmap_base_addr, physical_lane), lr_adapt, 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_clear_pclk_errors(adi_apollo_device_t *device,
    const uint16_t links)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t link;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (link = 0; link < ADI_APOLLO_NUM_JRX_LINKS; link++) {
        if ((1 << link) & links) {
            regmap_base_addr = calc_jrx_core_base(link);
            err = adi_apollo_hal_bf_set(device, BF_JRX_CORE_PCLK_ERROR_CLEAR_INFO(regmap_base_addr, link / ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_clear_fec_errors(adi_apollo_device_t *device,
    const uint16_t links)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t link;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (link = 0; link < ADI_APOLLO_NUM_JRX_LINKS; link++) {
        if ((1 << link) & links) {
            regmap_base_addr = calc_jrx_dl_204c_base(link);
            err = adi_apollo_hal_bf_set(device, BF_JRX_DL_204C_CLR_ERR_CNT_INFO(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_fec_errors(adi_apollo_device_t *device,
    const uint16_t link,
    const uint16_t lane,
    adi_apollo_fec_errors_t *fec_errors)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t  reg8;
    uint8_t link_index = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(fec_errors);
    ADI_APOLLO_INVALID_PARAM_RETURN(lane >= ADI_APOLLO_JESD_DESER_COUNT); // Check if lane is in range
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(link) != 1); // Check if only one link is selected
    
    link_index  = adi_api_utils_select_lsb_get(link);

    regmap_base_addr = calc_jrx_dl_204c_base(link_index);

    err = adi_apollo_hal_bf_get(device, BF_JRX_DL_204C_SH_ERR_CNT_INFO(regmap_base_addr, lane), &reg8, 1); /*  204C Lane status */
    ADI_APOLLO_ERROR_RETURN(err);
    fec_errors->sh_err_cnt = reg8;

    err = adi_apollo_hal_bf_get(device, BF_JRX_DL_204C_FEC_ERR_CNT_INFO(regmap_base_addr, lane), &reg8, 1); /*  204C Lane status */
    ADI_APOLLO_ERROR_RETURN(err);
    fec_errors->parity_err_cnt = reg8;

    err = adi_apollo_hal_bf_get(device, BF_JRX_DL_204C_FEC_UNCORRECTABLE_ERR_CNT_INFO(regmap_base_addr, lane), &reg8, 1); /*  204C Lane status */
    ADI_APOLLO_ERROR_RETURN(err);
    fec_errors->uncorrectable_err_cnt = reg8;

    err = adi_apollo_hal_bf_get(device, BF_JRX_DL_204C_CRC_ERR_CNT_INFO(regmap_base_addr, lane), &reg8, 1); /*  204C Lane status */
    ADI_APOLLO_ERROR_RETURN(err);
    fec_errors->crc_err_cnt = reg8;

    err = adi_apollo_hal_bf_get(device, BF_JRX_DL_204C_EMB_ERR_CNT_INFO(regmap_base_addr, lane), &reg8, 1); /*  204C Lane status */
    ADI_APOLLO_ERROR_RETURN(err);
    fec_errors->emb_err_cnt = reg8;

    err = adi_apollo_hal_bf_get(device, BF_JRX_DL_204C_MB_ERR_CNT_INFO(regmap_base_addr, lane), &reg8, 1); /*  204C Lane status */
    ADI_APOLLO_ERROR_RETURN(err);
    fec_errors->mb_err_cnt = reg8;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_link_inspect(adi_apollo_device_t *device,
    const uint16_t links,
    adi_apollo_jesd_rx_inspect_t *jrx_inspect)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t link = 0;
    uint8_t link_enable;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(jrx_inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(links) != 1);

    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i++) {
        if ((1 << i) & links) {
            link = i%ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE;
            regmap_base_addr = calc_jrx_jesd_l0_base(i);

            /* inspect from device registers */
            err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_L_CFG_INFO(regmap_base_addr, link), (uint8_t*) &(jrx_inspect->l_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_F_CFG_INFO(regmap_base_addr, link), (uint8_t*) &(jrx_inspect->f_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_M_CFG_INFO(regmap_base_addr, link), (uint8_t*) &(jrx_inspect->m_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_S_CFG_INFO(regmap_base_addr, link), (uint8_t*) &(jrx_inspect->s_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_N_CFG_INFO(regmap_base_addr, link), (uint8_t*) &(jrx_inspect->n_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_NP_CFG_INFO(regmap_base_addr, link), (uint8_t*) &(jrx_inspect->np_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_K_CFG_INFO(regmap_base_addr, link), &(jrx_inspect->k_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_CS_CFG_INFO(regmap_base_addr, link), (uint8_t*) &(jrx_inspect->cs), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JRX_CORE_SUBCLASSV_CFG_INFO(regmap_base_addr, link), &(jrx_inspect->subclass), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            
            /* get link enable state */
            regmap_base_addr = calc_jrx_wrapper_base(i);
            err = adi_apollo_hal_bf_get(device, BF_LINK_EN_INFO(regmap_base_addr), &link_enable, 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_JESD_MODE_INFO(regmap_base_addr, link), (uint8_t *) &(jrx_inspect->jesd_mode), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, BF_READBACK_JRX_NS_PARAM_INFO(regmap_base_addr, link), (uint8_t *)&(jrx_inspect->ns_minus1), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            if (i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE == 0) {
                jrx_inspect->link_en = (link_enable & 0x01) != 0 ? 1 : 0;
            } else {
                jrx_inspect->link_en = (link_enable & 0x02) != 0 ? 1 : 0;
            }

            regmap_base_addr = calc_jrx_core_base(i);
            err = adi_apollo_hal_bf_get(device, BF_JRX_LINK_TYPE_INFO(regmap_base_addr), (uint8_t *) &(jrx_inspect->ver), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Only one inspect per call */
            break;
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_phase_adjust_set(adi_apollo_device_t* device,
    const uint16_t links,
    const uint16_t phase_adjust)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jrx_core_base(i);

            err = adi_apollo_hal_bf_set(device, BF_JRX_CORE_PHASE_ADJUST_INFO_LSB(regmap_base_addr, (i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)), (uint8_t)phase_adjust);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_JRX_CORE_PHASE_ADJUST_INFO_MSB(regmap_base_addr, (i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)), (uint8_t)(phase_adjust >> 8));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_phase_adjust_get(adi_apollo_device_t *device,
                                        const uint16_t links,
                                        uint16_t *phase_adjust)
{
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(phase_adjust);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(links) != 1);

    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jrx_core_base(i);

            err = adi_apollo_hal_reg_get(device, REG_JRX_CORE_PHASE_ADJUST0_ADDR(regmap_base_addr, i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), ((uint8_t *)phase_adjust + 0));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_reg_get(device, REG_JRX_CORE_PHASE_ADJUST1_ADDR(regmap_base_addr, i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), ((uint8_t *)phase_adjust + 1));
            ADI_APOLLO_ERROR_RETURN(err);

            break; // single value per call
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_phase_adjust_calc(adi_apollo_device_t *device, const uint16_t links, uint16_t margin, uint16_t *phase_adjust)
{
    int32_t err = API_CMS_ERROR_OK;
    uint16_t jrx_phase_diff0, jrx_phase_diff1;
    uint16_t max_jrx_core_phase_diff;
    uint16_t lmfc_period;
    adi_apollo_jesd_rx_inspect_t jrx_status;
    int16_t phase_diff;
    uint16_t abs_phase_diff;
    
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(phase_adjust);

    /* Function currently only supporting single link per side */
    if (links != (ADI_APOLLO_LINK_A0 | ADI_APOLLO_LINK_B0)) {
        ADI_APOLLO_LOG_ERR("Phase adjust calc support for LINKs A0 & B0 only");
        return API_CMS_ERROR_NOT_SUPPORTED;
    }

    err = adi_apollo_jrx_link_inspect(device, ADI_APOLLO_LINK_A0, &jrx_status);
    ADI_APOLLO_ERROR_RETURN(err);

    if ((jrx_status.ns_minus1 + 1) == 0) {
        ADI_APOLLO_ERROR_RETURN(API_CMS_ERROR_DIV_BY_ZERO);
    }
    
    lmfc_period = ((jrx_status.k_minus1 + 1) * (jrx_status.s_minus1 + 1)) / (jrx_status.ns_minus1 + 1);

    err = adi_apollo_jrx_phase_diff_get(device, ADI_APOLLO_LINK_A0, &jrx_phase_diff0);
    ADI_APOLLO_ERROR_RETURN(err);

    err =adi_apollo_jrx_phase_diff_get(device, ADI_APOLLO_LINK_B0, &jrx_phase_diff1);
    ADI_APOLLO_ERROR_RETURN(err);
    
    phase_diff = (jrx_phase_diff0 - jrx_phase_diff1);
    abs_phase_diff = (phase_diff < 0) ? -phase_diff : phase_diff;
    if (abs_phase_diff < (lmfc_period / 2)) {
        max_jrx_core_phase_diff = ADI_UTILS_MAX(jrx_phase_diff0, jrx_phase_diff1);
    } else {
        max_jrx_core_phase_diff = ADI_UTILS_MIN(jrx_phase_diff0, jrx_phase_diff1);
    }

    *phase_adjust = (max_jrx_core_phase_diff + margin) % lmfc_period;

    return err;
}

int32_t adi_apollo_jrx_phase_diff_get(adi_apollo_device_t* device,
    const uint16_t links,
    uint16_t *phase_diff)
{
    int32_t  err;
    uint32_t regmap_base_addr = 0;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(phase_diff);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(links) != 1);

    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jrx_core_base(i);

            err = adi_apollo_hal_reg_get(device, REG_JRX_CORE_LANE_PHASE_DIFF_0_ADDR(regmap_base_addr, i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), ((uint8_t *)phase_diff+0));
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_reg_get(device, REG_JRX_CORE_LANE_PHASE_DIFF_1_ADDR(regmap_base_addr, i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE), ((uint8_t*)phase_diff+1));
            ADI_APOLLO_ERROR_RETURN(err);
            
            break;      // single value per call
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_lane_xbar_set(adi_apollo_device_t *device,
    const uint16_t links,
    uint8_t physical_lane,
    uint8_t logical_lane)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JRX_LANES_MAX-1);

    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i++) {
        if ((1 << i) & links) {

            regmap_base_addr = calc_jrx_core_base(i);

            if ((i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) == 0) {
                err = adi_apollo_hal_bf_set(device, BF_JRX_CORE_LANE_SEL_LINK0_INFO(regmap_base_addr, physical_lane), logical_lane);   /* Link 0 */
            } else {
                err = adi_apollo_hal_bf_set(device, BF_JRX_CORE_LANE_SEL_LINK1_INFO(regmap_base_addr, physical_lane), logical_lane);   /* Link 1*/
            }
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_lanes_xbar_set(adi_apollo_device_t *device,
    const uint16_t links,
    uint8_t logical_lanes[],
    uint32_t length)
{
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(length > ADI_APOLLO_JRX_LANES_MAX);

    for (i = 0; i < length; i++) {
        err = adi_apollo_jrx_lane_xbar_set(device, links, i, logical_lanes[i]);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_subclass_set(adi_apollo_device_t *device,
    const uint16_t links,
    const uint16_t subclass)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jrx_jesd_l0_base(i);
            err = adi_apollo_hal_bf_set(device, BF_JRX_CORE_SUBCLASSV_CFG_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)), subclass);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_j204c_irq_enable_set(adi_apollo_device_t *device,
    const uint16_t links,
    const uint32_t irqs, uint8_t enable)
{
    int32_t err, link_idx, irq_idx;
    uint32_t regmap_base_addr = 0, address, info;
    uint16_t link;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(enable > 1);

    for (link_idx = 0; link_idx < ADI_APOLLO_NUM_JRX_LINKS; link_idx++) {
        link = (1 << link_idx) & links;
        if (link > 0) {
            regmap_base_addr = calc_jrx_wrapper_base(link_idx);
            for (irq_idx = 0; irq_idx < ADI_APOLLO_JRX_J204C_IRQ_NUM; irq_idx++) {
                if (irqs & (1 << irq_idx)) {
                    calc_j204c_irq_en(regmap_base_addr, link_idx, irq_idx, &address, &info);
                    err = adi_apollo_hal_bf_set(device, address, info, enable);
                    ADI_APOLLO_ERROR_RETURN(err);
                    
                    calc_j204c_irq(regmap_base_addr, link_idx, irq_idx, &address, &info);
                    err = adi_apollo_hal_bf_set(device, address, info, 0);
                    ADI_APOLLO_ERROR_RETURN(err);
                }
            }  
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_j204c_irq_enable_get(adi_apollo_device_t *device,
                                            const uint16_t links,
                                            uint32_t *irqs_enabled)
{
    int32_t err, link_idx, irq_idx;
    uint32_t regmap_base_addr = 0, address, info;
    uint16_t link;
    uint8_t single_status;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(irqs_enabled);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(links) != 1);

    *irqs_enabled = 0;
    
    for (link_idx = 0; link_idx < ADI_APOLLO_NUM_JRX_LINKS; link_idx++) {
        link = (1 << link_idx) & links;
        if (link > 0) {
            regmap_base_addr = calc_jrx_wrapper_base(link_idx);
            for (irq_idx = 0; irq_idx < ADI_APOLLO_JRX_J204C_IRQ_NUM; irq_idx++) {
                calc_j204c_irq_en(regmap_base_addr, link_idx, irq_idx, &address, &info);
                err = adi_apollo_hal_bf_get(device, address, info, &single_status, 1);
                ADI_APOLLO_ERROR_RETURN(err);
                if (single_status) {
                    *irqs_enabled = *irqs_enabled | (1 << irq_idx);
                }
            }
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_jrx_j204c_irq_get(adi_apollo_device_t *device,
    const uint16_t links,
    const uint32_t irqs, uint8_t clear, uint32_t *status)
{
    int32_t err, link_idx, irq_idx;
    uint32_t regmap_base_addr = 0, address, info;
    uint16_t link;
    uint8_t single_status;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);

    *status = 0;

    for (link_idx = 0; link_idx < ADI_APOLLO_NUM_JRX_LINKS; link_idx++) {
        link = (1 << link_idx) & links;
        if (link > 0) {
            regmap_base_addr = calc_jrx_wrapper_base(link_idx);
            for (irq_idx = 0; irq_idx < ADI_APOLLO_JRX_J204C_IRQ_NUM; irq_idx++) {
                if (irqs & (1 << irq_idx)) {
                    calc_j204c_irq(regmap_base_addr, link_idx, irq_idx, &address, &info);
                    err = adi_apollo_hal_bf_get(device, address, info, &single_status, 1);
                    ADI_APOLLO_ERROR_RETURN(err);
                    if (single_status) {
                        *status = *status | (1 << irq_idx);
                    }
                }
            }  
        }
    }

    if (clear) {
        err = adi_apollo_jrx_j204c_irq_clear(device, links, irqs);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    
    return err;
}

int32_t adi_apollo_jrx_j204c_irq_clear(adi_apollo_device_t *device,
    const uint16_t links,
    const uint32_t irqs)
{
    int32_t err, link_idx, irq_idx;
    uint32_t regmap_base_addr = 0, address, info;
    uint16_t link;
    uint32_t jrx_dl_204c_instance;
    uint8_t single_status;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (link_idx = 0; link_idx < ADI_APOLLO_NUM_JRX_LINKS; link_idx++) {
        link = (1 << link_idx) & links;
        if (link > 0) {
            regmap_base_addr = calc_jrx_wrapper_base(link_idx);
            for (irq_idx = 0; irq_idx < ADI_APOLLO_JRX_J204C_IRQ_NUM; irq_idx++) {
                if (irqs & (1 << irq_idx)) {
                    calc_j204c_irq(regmap_base_addr, link_idx, irq_idx, &address, &info);
                    err = adi_apollo_hal_bf_get(device, address, info, &single_status, 1);
                    ADI_APOLLO_ERROR_RETURN(err);
                    if (single_status) {
                        if (irq_idx == 0) { // For CRC, must clear the error count BEFORE clearing IRQ status
                            jrx_dl_204c_instance = calc_jrx_dl_204c_base(link_idx);
                            err = adi_apollo_hal_bf_set(device, BF_JRX_DL_204C_CLR_ERR_CNT_INFO(jrx_dl_204c_instance), 1);
                            ADI_APOLLO_ERROR_RETURN(err);
                            err = adi_apollo_hal_bf_set(device, BF_JRX_DL_204C_CLR_ERR_CNT_INFO(jrx_dl_204c_instance), 0);
                            ADI_APOLLO_ERROR_RETURN(err);
                        }
                        err = adi_apollo_hal_bf_set(device, address, info, 1);
                        ADI_APOLLO_ERROR_RETURN(err);
                        err = adi_apollo_hal_bf_set(device, address, info, 0);
                        ADI_APOLLO_ERROR_RETURN(err);
                    }
                }
            }  
        }
    }

    return API_CMS_ERROR_OK;
}
int32_t adi_apollo_jrx_lr_adapt_set(adi_apollo_device_t *device,
                                        uint8_t link_sides,
                                        uint8_t physical_lane,
                                        uint8_t lr_adapt)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(link_sides > ADI_APOLLO_LINK_SIDE_ALL);
    ADI_APOLLO_INVALID_PARAM_RETURN(physical_lane > ADI_APOLLO_JRX_LANES_MAX - 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(lr_adapt > ADI_APOLLO_SERDES_LR_ADAPT_RATIO_4);

    for (i = 0; i < ADI_APOLLO_NUM_JRX_LINKS; i += ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) {
        if ((1 << i) & link_sides) {
            regmap_base_addr = calc_jrx_phy_ifx_base(i);
            err = adi_apollo_hal_bf_set(device, BF_JRX_IFX_LOG2_SPLIT_INFO(regmap_base_addr, physical_lane), lr_adapt);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }    

    return API_CMS_ERROR_OK;
}

/*==================== L O C A L   A P I   C O D E ====================*/

int32_t adi_apollo_jesd_rx_sample_repeat_en(adi_apollo_device_t *device,
	const uint16_t link_sides,
	uint8_t enable)
{
	int32_t err, i;
	uint32_t regmap_base_addr = 0;

	ADI_APOLLO_NULL_POINTER_RETURN(device);
	ADI_APOLLO_LOG_FUNC();
	ADI_APOLLO_INVALID_PARAM_RETURN(enable > 0x1);

	for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
		if ((1 << i) & link_sides) {
			regmap_base_addr = calc_jrx_wrapper_base(i);
			err = adi_apollo_hal_bf_set(device, BF_SAMPLE_REPEAT_EN_INFO(regmap_base_addr, (i % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)), enable);
			ADI_APOLLO_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

static uint32_t calc_jrx_core_base(int32_t link)
{
    return ((link / ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) == 0) ? JRX_CORE_JRX_TX_DIGITAL0 : JRX_CORE_JRX_TX_DIGITAL1;
}

static uint32_t calc_jrx_wrapper_base(int32_t link)
{
    return ((link / ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) == 0) ? JRX_WRAPPER_JRX_TX_DIGITAL0 : JRX_WRAPPER_JRX_TX_DIGITAL1;
}

static uint32_t calc_jrx_dl_204c_base(int32_t link)
{
    return ((link/ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) == 0) ? JRX_DL_204C_JRX_TX_DIGITAL0 : JRX_DL_204C_JRX_TX_DIGITAL1;
}

static uint32_t calc_jrx_dl_204b_base(int32_t link)
{
    return ((link/ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) == 0) ? JRX_DL_204B_JRX_TX_DIGITAL0 : JRX_DL_204B_JRX_TX_DIGITAL1;
}

static uint32_t calc_jrx_jesd_l0_base(int32_t link)
{
    return ((link/ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) == 0) ? JRX_JESD_L0_JRX_TX_DIGITAL0 : JRX_JESD_L0_JRX_TX_DIGITAL1;
}

static void calc_j204c_irq_en(uint32_t base, int32_t link, uint32_t idx, uint32_t *address, uint32_t *info)
{
    uint32_t j204c_irq_map[ADI_APOLLO_JRX_J204C_IRQ_NUM * 2] = {
        BF_JRX_204C_CRC_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_204C_SH_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_204C_MB_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_204C_EMB_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_DATA_RDY_LOST_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_RM_FIFO_EMPTY_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_RM_FIFO_FULL_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_LANE_FIFO_EMPTY_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_LANE_FIFO_FULL_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_INVALID_SAMPLE_ERR_IRQ_ENABLE_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE))
    };

    *address = j204c_irq_map[idx * 2];
    *info = j204c_irq_map[idx * 2 + 1];
}

static void calc_j204c_irq(uint32_t base, int32_t link, uint32_t idx, uint32_t *address, uint32_t *info)
{
    uint32_t j204c_irq_map[ADI_APOLLO_JRX_J204C_IRQ_NUM * 2] = {
        BF_JRX_204C_CRC_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_204C_SH_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_204C_MB_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_204C_EMB_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_DATA_RDY_LOST_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_RM_FIFO_EMPTY_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_RM_FIFO_FULL_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_LANE_FIFO_EMPTY_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_JRX_LANE_FIFO_FULL_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE)),
        BF_INVALID_SAMPLE_ERR_IRQ_INFO(base, (link % ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE))
    };

    *address = j204c_irq_map[idx * 2];
    *info = j204c_irq_map[idx * 2 + 1];
}

static uint32_t calc_jrx_phy_ifx_base(int32_t link)
{
    return ((link / ADI_APOLLO_NUM_JRX_LINKS_PER_SIDE) == 0) ? JRX_PHY_IFX_JRX_TX_DIGITAL0 : JRX_PHY_IFX_JRX_TX_DIGITAL1;
}
/*! @} */