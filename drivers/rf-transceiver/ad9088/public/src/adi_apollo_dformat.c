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
#include "adi_apollo_fddc_types.h"
#include "adi_apollo_bf_jtx_dformat.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_utils.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

#define BF_INVALID_EN_INFO_LINK0 0x00000100     // Link0 invalid bitfield
#define BF_INVALID_EN_INFO_LINK1 0x00000101     // Link1 invalid bitfield

/* Struct for acessing bit fields */
typedef struct {
    uint32_t reg_offset;
    uint32_t bf_info;
} reg_bf_info_map_t;

static uint8_t cbout_to_bf(uint8_t cbout, uint8_t is_8t8r);

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

int32_t adi_apollo_dformat_smon_fd_fddc_set(adi_apollo_device_t *device, adi_apollo_dformat_smon_fd_map_t map[], uint32_t map_len)
{
    int32_t err;
    int32_t i;
    int32_t idx;
    uint8_t i_q_sel;
    uint8_t cbout_bf;
    adi_apollo_blk_sel_t fddc;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(map);
    ADI_APOLLO_INVALID_PARAM_RETURN(map_len > ADI_APOLLO_FDDC_NUM)

    static reg_bf_info_map_t fddc_iq_map[2][ADI_APOLLO_FDDC_PER_SIDE_NUM] = {
        {   /* FDDC-I path*/
            {BF_FINE0_ADC_I_SEL_INFO(0)}, /* A0/B0 */
            {BF_FINE1_ADC_I_SEL_INFO(0)}, /* A1/B1 */
            {BF_FINE2_ADC_I_SEL_INFO(0)}, /* A2/B2 */
            {BF_FINE3_ADC_I_SEL_INFO(0)}, /* A3/B3 */
            {BF_FINE4_ADC_I_SEL_INFO(0)}, /* A4/B4 */
            {BF_FINE5_ADC_I_SEL_INFO(0)}, /* A5/B5 */
            {BF_FINE6_ADC_I_SEL_INFO(0)}, /* A6/B6 */
            {BF_FINE7_ADC_I_SEL_INFO(0)}, /* A7/B7 */
        },
        {   /* FDDC-Q path */
            {BF_FINE0_ADC_Q_SEL_INFO(0)}, /* A0/B0 */
            {BF_FINE1_ADC_Q_SEL_INFO(0)}, /* A1/B1 */
            {BF_FINE2_ADC_Q_SEL_INFO(0)}, /* A2/B2 */
            {BF_FINE3_ADC_Q_SEL_INFO(0)}, /* A3/B3 */
            {BF_FINE4_ADC_Q_SEL_INFO(0)}, /* A4/B4 */
            {BF_FINE5_ADC_Q_SEL_INFO(0)}, /* A5/B5 */
            {BF_FINE6_ADC_Q_SEL_INFO(0)}, /* A6/B6 */
            {BF_FINE7_ADC_Q_SEL_INFO(0)}, /* A7/B7 */
        }
    };

    for (idx = 0; idx < map_len; idx++) {
        for (i = 0; i < ADI_APOLLO_FDDC_NUM; i++) {
            fddc = map[idx].fddc_select & (ADI_APOLLO_FDDC_A0 << i);
            if (fddc > 0) {
                regmap_base_addr = calc_jtx_dformat_base((i / ADI_APOLLO_FDDC_PER_SIDE_NUM) * ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE);

                err = map[idx].i_q_select > 1 ? API_CMS_ERROR_INVALID_PARAM : API_CMS_ERROR_OK;
                ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, "i_q_select out of range %d", map[idx].i_q_select);

                err = (device->dev_info.is_8t8r ? (map[idx].cbout > ADI_APOLLO_RXMUX_CBOUT_3 ? API_CMS_ERROR_INVALID_PARAM : API_CMS_ERROR_OK) :
                                                  (map[idx].cbout > ADI_APOLLO_RXMUX_CBOUT_1 ? API_CMS_ERROR_INVALID_PARAM : API_CMS_ERROR_OK));
                ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, "cbout out of range %d", map[idx].cbout);

                i_q_sel = map[idx].i_q_select;
                cbout_bf = cbout_to_bf((uint8_t) map[idx].cbout, device->dev_info.is_8t8r);
    
                err = adi_apollo_hal_bf_set(device,
                                            fddc_iq_map[i_q_sel][i % ADI_APOLLO_FDDC_PER_SIDE_NUM].reg_offset + regmap_base_addr,
                                            fddc_iq_map[i_q_sel][i % ADI_APOLLO_FDDC_PER_SIDE_NUM].bf_info, cbout_bf);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

static uint8_t cbout_to_bf(uint8_t cbout, uint8_t is_8t8r)
{
    return is_8t8r ? cbout : (cbout == 0) ? cbout : 2;      /* for 4t4r 0=cbout0, 2=cbout1 */
}

uint32_t calc_jtx_dformat_base(int32_t link)
{
    return ((link / ADI_APOLLO_NUM_JTX_LINKS_PER_SIDE) == 0) ? JTX_DFORMAT_JTX_TOP_RX_DIGITAL0 : JTX_DFORMAT_JTX_TOP_RX_DIGITAL1;
}
/*! @} */