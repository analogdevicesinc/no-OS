/*!
 * \brief     APIs for DFORMAT functional block
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_DFORMAT
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_dformat.h"

#include "adi_apollo_dformat_local.h"
#include "adi_apollo_tmode_local.h"
#include "adi_apollo_bf_jtx_dformat.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

#define BF_INVALID_EN_INFO_LINK0 0x00000100     // Link0 invalid bitfield
#define BF_INVALID_EN_INFO_LINK1 0x00000101     // Link1 invalid bitfield

int32_t adi_apollo_dformat_pgm(adi_apollo_device_t *device, const uint16_t links, adi_apollo_dformat_pgm_t *config)
{ 
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dformat_base(i);
            
            if ((i % ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) { /* Link 0 */
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_SEL_0_INFO(regmap_base_addr), config->dfor_sel);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_INV_0_INFO(regmap_base_addr), config->dfor_inv);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_DDC_DITHER_EN_0_INFO(regmap_base_addr), config->dfor_ddc_dither_en);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_RES_0_INFO(regmap_base_addr), config->dfor_res);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_LINK_EN_0_INFO(regmap_base_addr), config->link_en);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_LINK_DDC_DEC_0_INFO(regmap_base_addr), config->dcm_ratio);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_LINK_TOTAL_DEC_0_INFO(regmap_base_addr), config->total_dcm);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, REG_INVALID_EN_ADDR(regmap_base_addr), BF_INVALID_EN_INFO_LINK0, config->invalid_en);
	            ADI_APOLLO_ERROR_RETURN(err);
	            err = adi_apollo_hal_bf_set(device, BF_SAMPLE_REPEAT_EN_0_INFO(regmap_base_addr), config->sample_repeat_en);
	            ADI_APOLLO_ERROR_RETURN(err);
            } else {                                            /* Link 1 */
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_SEL_1_INFO(regmap_base_addr), config->dfor_sel); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_INV_1_INFO(regmap_base_addr), config->dfor_inv); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_DDC_DITHER_EN_1_INFO(regmap_base_addr), config->dfor_ddc_dither_en); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_RES_1_INFO(regmap_base_addr), config->dfor_res); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_LINK_EN_1_INFO(regmap_base_addr), config->link_en);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_LINK_DDC_DEC_1_INFO(regmap_base_addr), config->dcm_ratio);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_LINK_TOTAL_DEC_1_INFO(regmap_base_addr), config->total_dcm);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, REG_INVALID_EN_ADDR(regmap_base_addr), BF_INVALID_EN_INFO_LINK1, config->invalid_en);
	            ADI_APOLLO_ERROR_RETURN(err);
	            err = adi_apollo_hal_bf_set(device, BF_SAMPLE_REPEAT_EN_1_INFO(regmap_base_addr), config->sample_repeat_en);
	            ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dformat_res_sel_set(adi_apollo_device_t *device, 
    const uint16_t links, 
    adi_apollo_chip_output_res_e resolution)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dformat_base(i);
            
            if ((i % ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) {
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_RES_0_INFO(regmap_base_addr), resolution);   /* Link 0 */
            } else {
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_RES_1_INFO(regmap_base_addr), resolution);   /* Link 1*/
            }

            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dformat_format_sel_set(adi_apollo_device_t *device, const uint16_t links,
    uint8_t format)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(format > 2);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dformat_base(i);
            
            if ((i % ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) {
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_SEL_0_INFO(regmap_base_addr), format);   /* Link 0 */
                ADI_APOLLO_ERROR_RETURN(err);
            } else {
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_SEL_1_INFO(regmap_base_addr), format);   /* Link 1 */
                ADI_APOLLO_ERROR_RETURN(err);
            }

            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dformat_ctrl_bit_sel_set(adi_apollo_device_t *device, const uint16_t links,
    uint8_t bit0_sel, uint8_t bit1_sel, uint8_t bit2_sel)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dformat_base(i);
            
            if ((i % ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) {
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_0_SEL_0_INFO(regmap_base_addr), bit0_sel);   /* Link 0 */
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_1_SEL_0_INFO(regmap_base_addr), bit1_sel);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_2_SEL_0_INFO(regmap_base_addr), bit2_sel);
                ADI_APOLLO_ERROR_RETURN(err);
            } else {
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_0_SEL_1_INFO(regmap_base_addr), bit0_sel);   /* Link 1 */
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_1_SEL_1_INFO(regmap_base_addr), bit1_sel);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_CTRL_BIT_2_SEL_1_INFO(regmap_base_addr), bit2_sel);
                ADI_APOLLO_ERROR_RETURN(err);
            }

            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_dformat_conv_test_mode_enable_set(adi_apollo_device_t *device, 
    const uint16_t links, 
    uint8_t enable)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i ++) {
        if ((1 << i) & links) {

            regmap_base_addr = calc_jtx_dformat_base(i);

            if ((i % ADI_APOLLO_TMODE_N) == 0) {
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_TMODE_SEL_0_INFO(regmap_base_addr), enable);     /* Link 0 */
            } else {
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_TMODE_SEL_1_INFO(regmap_base_addr), enable);     /* Link 1 */
            }
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dformat_inspect(adi_apollo_device_t *device, const uint16_t link, adi_apollo_dformat_inspect_t *inspect){
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(inspect);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(link) != 1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & link) {
            regmap_base_addr = calc_jtx_dformat_base(i);
            
            if ((i % ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) {                                              /* Link 0 */
                err = adi_apollo_hal_bf_get(device, BF_DFORMAT_SEL_0_INFO(regmap_base_addr), &(inspect->dp_cfg.sel), sizeof(inspect->dp_cfg.sel));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_DFORMAT_INV_0_INFO(regmap_base_addr), (uint8_t*) &(inspect->dp_cfg.inv), sizeof(inspect->dp_cfg.inv));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_DFORMAT_DDC_DITHER_EN_0_INFO(regmap_base_addr), (uint8_t*) &(inspect->dp_cfg.ddc_dither_en), sizeof(inspect->dp_cfg.ddc_dither_en));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_DFORMAT_RES_0_INFO(regmap_base_addr), &(inspect->dp_cfg.res), sizeof(inspect->dp_cfg.res));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_LINK_EN_0_INFO(regmap_base_addr), &(inspect->link_en), sizeof(inspect->link_en));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_LINK_DDC_DEC_0_INFO(regmap_base_addr), (uint8_t*) &(inspect->dcm_ratio), sizeof(inspect->dcm_ratio));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_LINK_TOTAL_DEC_0_INFO(regmap_base_addr), (uint8_t*) &(inspect->total_dcm), sizeof(inspect->total_dcm));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_SAMPLE_REPEAT_EN_0_INFO(regmap_base_addr), 
                    (uint8_t*) &(inspect->dp_cfg.rm_fifo.sample_repeat_en), sizeof(inspect->dp_cfg.rm_fifo.sample_repeat_en));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, REG_INVALID_EN_ADDR(regmap_base_addr), BF_INVALID_EN_INFO_LINK0,
                    (uint8_t*) &(inspect->dp_cfg.rm_fifo.invalid_en), sizeof(inspect->dp_cfg.rm_fifo.invalid_en));
                ADI_APOLLO_ERROR_RETURN(err);
            } else {                                                                                     /* Link 1 */
                err = adi_apollo_hal_bf_get(device, BF_DFORMAT_SEL_1_INFO(regmap_base_addr), &(inspect->dp_cfg.sel), sizeof(inspect->dp_cfg.sel)); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_DFORMAT_INV_1_INFO(regmap_base_addr), (uint8_t*) &(inspect->dp_cfg.inv), sizeof(inspect->dp_cfg.inv)); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_DFORMAT_DDC_DITHER_EN_1_INFO(regmap_base_addr), (uint8_t*) &(inspect->dp_cfg.ddc_dither_en), sizeof(inspect->dp_cfg.ddc_dither_en)); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_DFORMAT_RES_1_INFO(regmap_base_addr), &(inspect->dp_cfg.res), sizeof(inspect->dp_cfg.res)); 
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_LINK_EN_1_INFO(regmap_base_addr), &(inspect->link_en), sizeof(inspect->link_en));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_LINK_DDC_DEC_1_INFO(regmap_base_addr), (uint8_t*) &(inspect->dcm_ratio), sizeof(inspect->dcm_ratio));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_LINK_TOTAL_DEC_1_INFO(regmap_base_addr), (uint8_t*) &(inspect->total_dcm), sizeof(inspect->total_dcm));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, BF_SAMPLE_REPEAT_EN_1_INFO(regmap_base_addr), 
                    (uint8_t*) &(inspect->dp_cfg.rm_fifo.sample_repeat_en), sizeof(inspect->dp_cfg.rm_fifo.sample_repeat_en));
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_get(device, REG_INVALID_EN_ADDR(regmap_base_addr), BF_INVALID_EN_INFO_LINK1,
                    (uint8_t*) &(inspect->dp_cfg.rm_fifo.invalid_en), sizeof(inspect->dp_cfg.rm_fifo.invalid_en));
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_dformat_overflow_status_get(adi_apollo_device_t *device, const uint16_t link, uint8_t clear, uint8_t *status) {
    int32_t err;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(status);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(link) != 1);

    regmap_base_addr = calc_jtx_dformat_base(link);

    if (link == ADI_APOLLO_LINK_A0 || link == ADI_APOLLO_LINK_B0) { // Link 0
        err = adi_apollo_hal_bf_get(device, BF_DFORMAT_OVR_STATUS_0_INFO(regmap_base_addr), status, sizeof(*status));
        ADI_APOLLO_ERROR_RETURN(err);
    } else { // Link 1
        err = adi_apollo_hal_bf_get(device, BF_DFORMAT_OVR_STATUS_1_INFO(regmap_base_addr), status, sizeof(*status));
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if (clear) {
        err = adi_apollo_dformat_overflow_status_clear(device, link);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_dformat_overflow_status_clear(adi_apollo_device_t *device, const uint16_t link) {
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & link) {
            regmap_base_addr = calc_jtx_dformat_base(i);
            if ((i % ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) { // Link 0
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_OVR_CLR_0_INFO(regmap_base_addr), 0);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_OVR_CLR_0_INFO(regmap_base_addr), 0xFF);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_OVR_CLR_0_INFO(regmap_base_addr), 0);
                ADI_APOLLO_ERROR_RETURN(err);
            } else { // Link 1
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_OVR_CLR_1_INFO(regmap_base_addr), 0);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_OVR_CLR_1_INFO(regmap_base_addr), 0xFF);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_DFORMAT_OVR_CLR_1_INFO(regmap_base_addr), 0);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

uint32_t calc_jtx_dformat_base(int32_t link)
{
    return ((link / ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) ? JTX_DFORMAT_JTX_TOP_RX_DIGITAL0 : JTX_DFORMAT_JTX_TOP_RX_DIGITAL1;
}
/*! @} */