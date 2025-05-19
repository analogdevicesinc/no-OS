/*!
 * \brief     APIs for TRIGGER & TIMESTAMP
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TRIGGER
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_utils.h"
#include "adi_apollo_trigts.h"
#include "adi_apollo_trigts_local.h"
#include "adi_apollo_reconfig_local.h"
#include "adi_apollo_cnco_types.h"
#include "adi_apollo_fnco_types.h"
#include "adi_apollo_cddc_types.h"
#include "adi_apollo_fddc_types.h"
#include "adi_apollo_pfilt_types.h"
#include "adi_apollo_cfir_types.h"
#include "adi_apollo_bmem_types.h"
#include "adi_apollo_clk_mcs.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_txrx_trigger_ts.h"
#include "adi_apollo_bf_txrx_prefsrc_reconf.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/* Struct used for addressing functional block trig sel muxes */ 
typedef struct {
    uint32_t reg_base_addr;
    uint32_t bf_info;
} reg_bf_info_map_t;

/* Maps CNCO functional blocks to associated trig master select */
static uint32_t calc_cnco_trig_mst_num(uint8_t cnco_index);

/* Maps FNCO functional blocks to associated trig master select */
static uint32_t calc_fnco_trig_mst_num(uint8_t fnco_index);

/* Maps PFILT functional blocks to associated trig master select */
static uint32_t calc_pfilt_trig_mst_num(uint8_t pfilt_index);

/* Maps CFIR functional blocks to associated trig master select */
static uint32_t calc_cfir_trig_mst_num(uint8_t cfir_index);

/* Trigger sel mux register and bf info */
static reg_bf_info_map_t *calc_cdrc_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t cnco_idx);
static reg_bf_info_map_t *calc_fdrc_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t fnco_idx);
static reg_bf_info_map_t *calc_bmem_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t bmem_idx);
static reg_bf_info_map_t *calc_pfilt_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t pfilt_idx);
static reg_bf_info_map_t *calc_cfir_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t cfir_idx);

/* Utility for converting a functional block select index to its associated trig mst sel*/
static uint32_t fb_sel_to_tm_sel(uint32_t(*fb2tm_fp)(uint8_t), uint32_t fb_sel, uint8_t fb_sel_per_side, uint16_t fb_sel_side_offset);

/*==================== P U B L I C   A P I   C O D E ====================*/
int32_t adi_apollo_trigts_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                         uint32_t trig_msts, adi_apollo_trig_mst_config_t *config) 
{
    int32_t err;
    uint8_t i, side, side_index, trig_index;
    uint32_t trig_mst;
    uint8_t byte_array[8];
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    //TODO : Invalid param check for both Tx and Rx side trigger masters  
    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);
            
            /* Set reg_map base address for subsequent paged writes */
            adi_apollo_hal_paged_base_addr_set(device, regmap_base_addr);

            for (trig_index = 0; trig_index < ADI_APOLLO_NUM_TRIG_MASTER_PER_SIDE; trig_index++) {
                trig_mst = trig_msts & (ADI_APOLLO_TRIG_MST_CNCO_0 << trig_index); 
                
                if (trig_mst > 0) {
                    
                    /* Trigger enable */
                    // err = adi_apollo_hal_bf_set(device, BF_TRIG_EN_INFO(regmap_base_addr, trig_index), config->trig_enable);
                    err = adi_apollo_hal_paged_bf_set(device, BF_TRIG_EN_INFO(regmap_base_addr, trig_index), config->trig_enable);
                    ADI_APOLLO_ERROR_RETURN(err);
                    
                    /* Trigger offset (registers have a stride of 32) */
                    adi_uint64_to_byte_array(byte_array, config->trig_offset);
                    for (i=0; i<8; i++) {
                        // adi_apollo_hal_reg_set(device, REG_TRIG_OFFSET0_ADDR(regmap_base_addr, trig_index) + i*32, byte_array[i]); /* not paged */
                        adi_apollo_hal_paged_reg_set(device, REG_TRIG_OFFSET0_ADDR(regmap_base_addr, trig_index) + i*32, byte_array[i]);
                        ADI_APOLLO_ERROR_RETURN(err);
                    }
                    
                    /* Trigger period  (registers have a stride of 32) */
                    adi_uint64_to_byte_array(byte_array, config->trig_period);
                    for (i=0; i<8; i++) {
                        // adi_apollo_hal_reg_set(device, REG_TRIG_PERIOD0_ADDR(regmap_base_addr, trig_index) + i*32, byte_array[i]); /* not paged */
                        adi_apollo_hal_paged_reg_set(device, REG_TRIG_PERIOD0_ADDR(regmap_base_addr, trig_index) + i*32, byte_array[i]);
                        ADI_APOLLO_ERROR_RETURN(err);
                    }

                }
            }
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_mst_inspect(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                   uint32_t trig_msts, adi_apollo_trig_mst_config_t *config)
{
    int32_t err;
    uint8_t i, side, side_index, trig_index;
    uint32_t trig_mst;
    uint8_t val_u64[8];
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(side_sel) != 1);

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            for (trig_index = 0; trig_index < ADI_APOLLO_NUM_TRIG_MASTER_PER_SIDE; trig_index++) {
                trig_mst = trig_msts & (ADI_APOLLO_TRIG_MST_CNCO_0 << trig_index); 
                
                if (trig_mst > 0) {
                    
                    /* Trigger enable */
                    err = adi_apollo_hal_bf_get(device, BF_TRIG_EN_INFO(regmap_base_addr, trig_index), (uint8_t *) &config->trig_enable, sizeof(config->trig_enable));
                    ADI_APOLLO_ERROR_RETURN(err);

                    /* Trigger offset (registers have a stride of 32) */
                    for (i=0; i<8; i++) {
                        adi_apollo_hal_reg_get(device, REG_TRIG_PERIOD0_ADDR(regmap_base_addr, trig_index) + i*32, &val_u64[i]);
                    }
                    adi_byte_array_to_uint64(val_u64, &config->trig_period);
                    
                    /* Trigger period  (registers have a stride of 32) */
                    for (i=0; i<8; i++) {
                        adi_apollo_hal_reg_get(device, REG_TRIG_OFFSET0_ADDR(regmap_base_addr, trig_index) + i*32, &val_u64[i]);
                    }
                    adi_byte_array_to_uint64(val_u64, &config->trig_offset);

                    // Can only return one trig mst config at time
                    return API_CMS_ERROR_OK;
                }
            }
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_mst_mute(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                 uint32_t trig_msts, adi_apollo_trig_mute_sel_e trigger_mute)
{
    int32_t err;
    uint8_t side, side_index, trig_index;
    uint32_t trig_mst;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            for (trig_index = 0; trig_index < ADI_APOLLO_NUM_TRIG_MASTER_PER_SIDE; trig_index++) {
                trig_mst = trig_msts & (ADI_APOLLO_TRIG_MST_CNCO_0 << trig_index); 
                if (trig_mst > 0) {
                    err = adi_apollo_hal_bf_set(device, BF_MUTE_TRIG_INFO(regmap_base_addr, trig_index), trigger_mute);
                    ADI_APOLLO_ERROR_RETURN(err);
                }
            }
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_mst_mute_mask_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                 uint32_t trig_msts, adi_apollo_trig_mute_mask_e trigger_mute)
{
    int32_t err;
    uint8_t side, side_index, trig_index;
    uint32_t trig_mst;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(trigger_mute > ADI_APOLLO_TRIG_MUTE_MASK_1);

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            for (trig_index = 0; trig_index < ADI_APOLLO_NUM_TRIG_MASTER_PER_SIDE; trig_index++) {
                trig_mst = trig_msts & (ADI_APOLLO_TRIG_MST_CNCO_0 << trig_index); 
                if (trig_mst > 0) {
                    err = adi_apollo_hal_bf_set(device, BF_TRIG_MASK_COUNT_INFO(regmap_base_addr, trig_index), trigger_mute);
                    ADI_APOLLO_ERROR_RETURN(err);
                }
            }
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_mst_mute_mask_count_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                adi_apollo_trig_mute_mask_e trigger_mute, adi_apollo_trig_mst_sel_e trig_mst, uint16_t pulse_count)
{
    int32_t err;
    uint8_t side, side_index;
    uint32_t regmap_base_addr = 0;
    uint32_t trig = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(trigger_mute == ADI_APOLLO_TRIG_MUTE_MASK_DISABLE);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(trig_mst) != 1);

    trig = adi_api_utils_select_lsb_get(trig_mst);

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);
            if (trigger_mute == ADI_APOLLO_TRIG_MUTE_MASK_0) {
                err = adi_apollo_hal_bf_set(device, BF_COUNT0_REG0_INFO(regmap_base_addr), pulse_count);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_COUNT0_REG1_INFO(regmap_base_addr), pulse_count >> 8);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_TM_SEL0_INFO(regmap_base_addr), trig);
                ADI_APOLLO_ERROR_RETURN(err);
            } else {
                err = adi_apollo_hal_bf_set(device, BF_COUNT1_REG0_INFO(regmap_base_addr), pulse_count);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_COUNT1_REG1_INFO(regmap_base_addr), pulse_count >> 8);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_TM_SEL1_INFO(regmap_base_addr), trig);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_reconfig_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel,
                                               adi_apollo_trigger_sel_mux_e trig_sel)
{
    int32_t err;
    uint8_t side, side_index;
    uint32_t regmap_base_addr = 0;


    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            err = adi_apollo_hal_bf_set(device, BF_TRIG_SEL_MUX_RECONFIG_INFO(regmap_base_addr), trig_sel);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_cdrc_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal,  adi_apollo_blk_sel_t cdrcs, adi_apollo_trigger_sel_mux_e trig_sel)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t cdrc;
    reg_bf_info_map_t *reg_bfinfo;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CNCO_BLK_SEL_MASK(cdrcs);

    for(i = 0; i < ADI_APOLLO_CNCO_NUM; i ++) {
        cdrc = cdrcs & (ADI_APOLLO_CNCO_A0 << i);
        if (cdrc > 0) {
            reg_bfinfo = calc_cdrc_trig_sel_mux(terminal, i);

            err = adi_apollo_hal_bf_set(device, reg_bfinfo->reg_base_addr, reg_bfinfo->bf_info, trig_sel);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    
    return API_CMS_ERROR_OK;   
}

int32_t adi_apollo_trigts_fdrc_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fdrcs, adi_apollo_trigger_sel_mux_e trig_sel)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t fdrc;
    reg_bf_info_map_t *reg_bfinfo;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_FNCO_BLK_SEL_MASK(fdrcs);

    for(i = 0; i < ADI_APOLLO_FNCO_NUM; i ++) {
        fdrc = fdrcs & (ADI_APOLLO_FNCO_A0 << i);
        if (fdrc > 0) {
            reg_bfinfo = calc_fdrc_trig_sel_mux(terminal, i);

            err = adi_apollo_hal_bf_set(device, reg_bfinfo->reg_base_addr, reg_bfinfo->bf_info, trig_sel);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    
    return API_CMS_ERROR_OK;   
}

int32_t adi_apollo_trigts_bmem_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t bmems, adi_apollo_trigger_sel_mux_e trig_sel)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t bmem;
    reg_bf_info_map_t *reg_bfinfo;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for(i = 0; i < ADI_APOLLO_BMEM_NUM; i ++) {
        bmem = bmems & (ADI_APOLLO_BMEM_A0 << i);
        if (bmem > 0) {
            reg_bfinfo = calc_bmem_trig_sel_mux(terminal, i);

            err = adi_apollo_hal_bf_set(device, reg_bfinfo->reg_base_addr, reg_bfinfo->bf_info, trig_sel);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    
    return API_CMS_ERROR_OK;   
}

int32_t adi_apollo_trigts_pfilt_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, adi_apollo_trigger_sel_mux_e trig_sel)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t pfilt;
    reg_bf_info_map_t *reg_bfinfo;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_PFILT_BLK_SEL_MASK(pfilts);

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i ++) {
        pfilt = pfilts & (ADI_APOLLO_PFILT_A0 << i);
        if (pfilt > 0) {
            reg_bfinfo = calc_pfilt_trig_sel_mux(terminal, i);

            err = adi_apollo_hal_bf_set(device, reg_bfinfo->reg_base_addr, reg_bfinfo->bf_info, trig_sel);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    
    return API_CMS_ERROR_OK;   
}

int32_t adi_apollo_trigts_cfir_trig_sel_mux_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, adi_apollo_trigger_sel_mux_e trig_sel)
{
    int32_t err;
    uint8_t i;
    adi_apollo_blk_sel_t cfir;
    reg_bf_info_map_t *reg_bfinfo;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_CFIR_BLK_SEL_MASK(cfirs);

    for(i = 0; i < ADI_APOLLO_PFILT_NUM; i ++) {
        cfir = cfirs & (ADI_APOLLO_PFILT_A0 << i);
        if (cfir > 0) {
            reg_bfinfo = calc_cfir_trig_sel_mux(terminal, i);

            err = adi_apollo_hal_bf_set(device, reg_bfinfo->reg_base_addr, reg_bfinfo->bf_info, trig_sel);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    
    return API_CMS_ERROR_OK;   
}

int32_t adi_apollo_trigts_trig_now(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel)
{
    int32_t err;
    uint8_t side, side_index;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            // A low-to-high transition spi_trig 
            err = adi_apollo_hal_reg_set(device, REG_SPI_TRIG_ADDR(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_reg_set(device, REG_SPI_TRIG_ADDR(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_counter_get(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel, uint64_t *val)
{
    int32_t err;
    uint8_t side, side_index;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(val);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(side_sel) != 1);

    for (side_index = 0; side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            /* Latch 64-bit timestamp counter*/
            err = adi_apollo_hal_bf_set(device, BF_TIMESTAMP_READ_EN_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_TIMESTAMP_READ_EN_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_TIMESTAMP_READ_EN_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);

            /* Read the 64-bit counter value */
            err = adi_apollo_hal_bf_get(device, BF_TIMESTAMP_STATUS_TXRX_TRIGGER_TS_INFO(regmap_base_addr), (uint8_t *)val, sizeof(*val));
            ADI_APOLLO_ERROR_RETURN(err);

            /* Only single value returned */
            break;
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_cnco_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cncos, adi_apollo_trig_mst_config_t *config)
{
    int32_t err;
    uint32_t trig_msts;
    
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();    
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    /* Configure cnco trigger masters for Side A */
    trig_msts = fb_sel_to_tm_sel(calc_cnco_trig_mst_num, cncos, ADI_APOLLO_CDDC_PER_SIDE_NUM, ADI_APOLLO_CDDC_A0);
    if (trig_msts > 0){
        err = adi_apollo_trigts_mst_config(device, terminal, ADI_APOLLO_SIDE_A, trig_msts, config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Configure cnco trigger masters for Side B */
    trig_msts = fb_sel_to_tm_sel(calc_cnco_trig_mst_num, cncos, ADI_APOLLO_CDDC_PER_SIDE_NUM, ADI_APOLLO_CDDC_B0);
    if (trig_msts > 0){
        err = adi_apollo_trigts_mst_config(device, terminal, ADI_APOLLO_SIDE_B, trig_msts, config);
        ADI_APOLLO_ERROR_RETURN(err);        
    }

    return API_CMS_ERROR_OK;   
}

int32_t adi_apollo_trigts_fnco_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t fncos, adi_apollo_trig_mst_config_t *config)
{
    int32_t err;
    uint32_t trig_msts = 0;
    
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    /* Configure fnco trigger masters for Side A */
    trig_msts = fb_sel_to_tm_sel(calc_fnco_trig_mst_num, fncos, ADI_APOLLO_FDDC_PER_SIDE_NUM, ADI_APOLLO_FDDC_A0);
    if (trig_msts > 0){
        err = adi_apollo_trigts_mst_config(device, terminal, ADI_APOLLO_SIDE_A, trig_msts, config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Configure fnco trigger masters for Side B */
    trig_msts = fb_sel_to_tm_sel(calc_fnco_trig_mst_num, fncos, ADI_APOLLO_FDDC_PER_SIDE_NUM, ADI_APOLLO_FDDC_B0);
    if (trig_msts > 0){
        err = adi_apollo_trigts_mst_config(device, terminal, ADI_APOLLO_SIDE_B, trig_msts, config);
        ADI_APOLLO_ERROR_RETURN(err);        
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_pfilt_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t pfilts, adi_apollo_trig_mst_config_t *config)
{
    int32_t err;
    uint32_t trig_msts;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    /* Configure pfilt trigger masters for Side A */
    trig_msts = fb_sel_to_tm_sel(calc_pfilt_trig_mst_num, pfilts, ADI_APOLLO_PFILT_PER_SIDE, ADI_APOLLO_PFILT_A0);
    if (trig_msts > 0) {
        err = adi_apollo_trigts_mst_config(device, terminal, ADI_APOLLO_SIDE_A, trig_msts, config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Configure PFILT trigger masters for Side B */
    trig_msts = fb_sel_to_tm_sel(calc_pfilt_trig_mst_num, pfilts, ADI_APOLLO_PFILT_PER_SIDE, ADI_APOLLO_PFILT_B0);
    if (trig_msts > 0) {
        err = adi_apollo_trigts_mst_config(device, terminal, ADI_APOLLO_SIDE_B, trig_msts, config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_cfir_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_blk_sel_t cfirs, adi_apollo_trig_mst_config_t *config)
{
    int32_t err;
    uint32_t trig_msts;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    /* Configure cfir trigger masters for Side A */
    trig_msts = fb_sel_to_tm_sel(calc_cfir_trig_mst_num, cfirs, ADI_APOLLO_CFIRS_PER_SIDE, ADI_APOLLO_CFIR_A0);
    if (trig_msts > 0) {
        err = adi_apollo_trigts_mst_config(device, terminal, ADI_APOLLO_SIDE_A, trig_msts, config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Configure CFIR trigger masters for Side B */
    trig_msts = fb_sel_to_tm_sel(calc_cfir_trig_mst_num, cfirs, ADI_APOLLO_CFIRS_PER_SIDE, ADI_APOLLO_CFIR_B0);
    if (trig_msts > 0) {
        err = adi_apollo_trigts_mst_config(device, terminal, ADI_APOLLO_SIDE_B, trig_msts, config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_reconfig_trig_mst_config(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, adi_apollo_side_select_e side_sel, 
                                                 adi_apollo_trig_mst_config_t *config)
{    
    return adi_apollo_trigts_mst_config(device, terminal, side_sel, ADI_APOLLO_TRIG_MST_RECONFIG, config);    
}

int32_t adi_apollo_trigts_ts_reset_mode_set(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint16_t side_sel, adi_apollo_trig_ts_reset_mode_e mode)
{
    int32_t err;
    uint8_t side_index;
    uint16_t side;
    uint32_t regmap_base_addr = 0;
    uint32_t prefsrc_regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0; side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            // Set the timestamp reset mode: SYSREF or SPI based
            err = adi_apollo_hal_bf_set(device, BF_TS_RST_MODE_TXRX_TRIGGER_TS_INFO(regmap_base_addr), mode);
            ADI_APOLLO_ERROR_RETURN(err);

            // Clear reset done status for selected timestamp instance
            err = adi_apollo_hal_bf_set(device, BF_RST_DONE_CLR_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_RST_DONE_CLR_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            if (mode == ADI_APOLLO_TRIG_TS_RESET_MODE_SYSREF) {
                prefsrc_regmap_base_addr = calc_prefsrc_reconfig_base(terminal, side_index);

                // Enable timestamp reset on sync event (timestamp_reset_en requires 0->1 transition to set)
                err = adi_apollo_hal_bf_set(device, BF_TIMESTAMP_RESET_EN_INFO(prefsrc_regmap_base_addr), 0);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_TIMESTAMP_RESET_EN_INFO(prefsrc_regmap_base_addr), 1);
                ADI_APOLLO_ERROR_RETURN(err);

                err = adi_apollo_hal_bf_set(device, BF_NONRESYNC_SYSREF_EN_INFO(prefsrc_regmap_base_addr), 1);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_ts_reset(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint16_t side_sel, uint8_t sync)
{
    int32_t err;
    uint8_t side_index;
    uint16_t side;
    uint8_t mode, sync_pending = 0;
    uint32_t regmap_base_addr = 0;
    uint32_t prefsrc_regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0; side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            // Clear reset done status
            err = adi_apollo_hal_bf_set(device, BF_RST_DONE_CLR_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_RST_DONE_CLR_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);

            // Determine current timestamp reset mode
            err = adi_apollo_hal_bf_get(device, BF_TS_RST_MODE_TXRX_TRIGGER_TS_INFO(regmap_base_addr), &mode, sizeof(mode));
            ADI_APOLLO_ERROR_RETURN(err);

            if (mode == ADI_APOLLO_TRIG_TS_RESET_MODE_SYSREF) {
                // Reset timestamp counter via SYSREF/SYNC
                prefsrc_regmap_base_addr = calc_prefsrc_reconfig_base(terminal, side_index);

                // Enable timestamp reset on sync event (timestamp_reset_en requires 0->1 transition to set)
                err = adi_apollo_hal_bf_set(device, BF_TIMESTAMP_RESET_EN_INFO(prefsrc_regmap_base_addr), 0);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_TIMESTAMP_RESET_EN_INFO(prefsrc_regmap_base_addr), 1);
                ADI_APOLLO_ERROR_RETURN(err);

                // Defer sync request until all timestamp block instances are configure (i.e. one trig does all)
                sync_pending = sync;
            } else {
                // Reset timestamp counter via SPI
                err = adi_apollo_hal_bf_set(device, BF_SPI_TS_RST_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 0);
                ADI_APOLLO_ERROR_RETURN(err);
                err = adi_apollo_hal_bf_set(device, BF_SPI_TS_RST_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 1);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    }

    if (sync_pending) {
        err = adi_apollo_clk_mcs_sync_only_set(device); // dynamic sync (no SYSREF realignment)
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_reset_done_clear(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint16_t side_sel) {
    int32_t err;
    uint8_t side_index;
    uint16_t side;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0; side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            // Clear reset done
            err = adi_apollo_hal_bf_set(device, BF_RST_DONE_CLR_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 0);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_RST_DONE_CLR_TXRX_TRIGGER_TS_INFO(regmap_base_addr), 1);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_trigts_reset_done_get(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint16_t side_sel, uint8_t *reset_done_status)
{
    int32_t err;
    uint8_t side_index;
    uint16_t side;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(side_sel) != 1);

    for (side_index = 0; side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            regmap_base_addr = calc_trigts_base(terminal, side_index);

            err = adi_apollo_hal_bf_get(device, BF_RST_DONE_TXRX_TRIGGER_TS_INFO(regmap_base_addr), reset_done_status, sizeof(*reset_done_status));
            ADI_APOLLO_ERROR_RETURN(err);

            /* Only single value returned */
            break;
        }
    }

    return API_CMS_ERROR_OK;
}

static uint32_t calc_cnco_trig_mst_num(uint8_t cnco_index)
{   
    static uint32_t cnco_trig_mst_map[ADI_APOLLO_CDDC_PER_SIDE_NUM] = {
        ADI_APOLLO_TRIG_MST_CNCO_0, ADI_APOLLO_TRIG_MST_CNCO_1, 
        ADI_APOLLO_TRIG_MST_CNCO_2, ADI_APOLLO_TRIG_MST_CNCO_3        
    };
    return cnco_trig_mst_map[cnco_index];
}

static uint32_t calc_fnco_trig_mst_num(uint8_t fnco_index)
{
    static uint32_t fnco_trig_mst_map[ADI_APOLLO_FDDC_PER_SIDE_NUM] = {
        ADI_APOLLO_TRIG_MST_FNCO_0, ADI_APOLLO_TRIG_MST_FNCO_1, 
        ADI_APOLLO_TRIG_MST_FNCO_2, ADI_APOLLO_TRIG_MST_FNCO_3,
        ADI_APOLLO_TRIG_MST_FNCO_4, ADI_APOLLO_TRIG_MST_FNCO_5, 
        ADI_APOLLO_TRIG_MST_FNCO_6, ADI_APOLLO_TRIG_MST_FNCO_7
    };
    return fnco_trig_mst_map[fnco_index];
}

static uint32_t calc_pfilt_trig_mst_num(uint8_t pfilt_index)
{
    static uint32_t pfilt_trig_mst_map[ADI_APOLLO_PFILT_PER_SIDE] = {
        ADI_APOLLO_TRIG_MST_PFILT0, ADI_APOLLO_TRIG_MST_PFILT1};
    return pfilt_trig_mst_map[pfilt_index];
}

static uint32_t calc_cfir_trig_mst_num(uint8_t cfir_index)
{
    static uint32_t cfir_trig_mst_map[ADI_APOLLO_CFIR_PER_SIDE_NUM] = {
        ADI_APOLLO_TRIG_MST_CFIR0, ADI_APOLLO_TRIG_MST_CFIR1};
    return cfir_trig_mst_map[cfir_index];
}

static uint32_t fb_sel_to_tm_sel(uint32_t(*fb2tm_fp)(uint8_t), uint32_t fb_sel, uint8_t fb_sel_per_side, uint16_t fb_sel_side_offset)
{
    uint8_t i;
    uint32_t fb, trig_msts = 0;
    
    // Determine the corresponding trigger master(tm) selects from a functional block(fb) select
    for (i = 0; i < fb_sel_per_side; i ++) {
        fb = fb_sel & (fb_sel_side_offset << i);
        if (fb > 0) {
            trig_msts |= fb2tm_fp(i);   /* func block idx to trig mst bit sel */
        }
    }
    return trig_msts;
}

static reg_bf_info_map_t *calc_cdrc_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t cnco_idx)
{
    static reg_bf_info_map_t cdrc_rx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_CDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 0)},   /* A0 */
        {BF_TRIG_SEL_MUX_CDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 0)},   /* A1 */
        {BF_TRIG_SEL_MUX_CDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 1)},   /* A2 */
        {BF_TRIG_SEL_MUX_CDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 1)},   /* A3 */

        {BF_TRIG_SEL_MUX_CDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 0)},   /* B0 */
        {BF_TRIG_SEL_MUX_CDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 0)},   /* B1 */
        {BF_TRIG_SEL_MUX_CDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 1)},   /* B2 */
        {BF_TRIG_SEL_MUX_CDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 1)}    /* B3 */
    };

    static reg_bf_info_map_t cdrc_tx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_CDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 0)},    /* A0 */
        {BF_TRIG_SEL_MUX_CDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 0)},    /* A1 */
        {BF_TRIG_SEL_MUX_CDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 1)},    /* A2 */
        {BF_TRIG_SEL_MUX_CDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 1)},    /* A3 */

        {BF_TRIG_SEL_MUX_CDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 0)},    /* B0 */
        {BF_TRIG_SEL_MUX_CDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 0)},    /* B1 */
        {BF_TRIG_SEL_MUX_CDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 1)},    /* B2 */
        {BF_TRIG_SEL_MUX_CDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 1)}     /* B3 */
    };

    return (terminal == ADI_APOLLO_RX) ? &cdrc_rx_trig_sel_mux[cnco_idx] : &cdrc_tx_trig_sel_mux[cnco_idx];
}

static reg_bf_info_map_t *calc_fdrc_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t fnco_idx)
{
    static reg_bf_info_map_t fdrc_rx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 0)},   /* A0 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 1)},   /* A1 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 2)},   /* A2 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 3)},   /* A3 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 0)},   /* A4 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 1)},   /* A5 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 2)},   /* A6 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 3)},   /* A7 */

        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 0)},   /* B0 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 1)},   /* B1 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 2)},   /* B2 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 3)},   /* B3 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 0)},   /* B4 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 1)},   /* B5 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 2)},   /* B6 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 3)},   /* B7 */
    };

    static reg_bf_info_map_t fdrc_tx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 0)},   /* A0 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 1)},   /* A1 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 2)},   /* A2 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 3)},   /* A3 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 0)},   /* A4 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 1)},   /* A5 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 2)},   /* A6 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 3)},   /* A7 */

        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 0)},   /* B0 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 1)},   /* B1 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 2)},   /* B2 */
        {BF_TRIG_SEL_MUX_FDRC0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 3)},   /* B3 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 0)},   /* B4 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 1)},   /* B5 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 2)},   /* B6 */
        {BF_TRIG_SEL_MUX_FDRC1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 3)},   /* B7 */
    };

    return (terminal == ADI_APOLLO_RX) ? &fdrc_rx_trig_sel_mux[fnco_idx] : &fdrc_tx_trig_sel_mux[fnco_idx];
}

static reg_bf_info_map_t *calc_bmem_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t bmem_idx)
{
    static reg_bf_info_map_t bmem_rx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 0)},   /* A0 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 1)},   /* A1 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 0)},   /* A2 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 1)},   /* A3 */

        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 0)},   /* B0 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 1)},   /* B1 */   
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 0)},   /* B2 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 1)},   /* B3 */   
    };

    static reg_bf_info_map_t bmem_tx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 0)},   /* A0 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 1)},   /* A1 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 0)},   /* A2 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 1)},   /* A3 */

        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 0)},   /* B0 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 1)},   /* B1 */   
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 0)},   /* B2 */
        {BF_TRIG_SEL_MUX_BMEM_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 1)},   /* B3 */   
    };

    return (terminal == ADI_APOLLO_RX) ? &bmem_rx_trig_sel_mux[bmem_idx] : &bmem_tx_trig_sel_mux[bmem_idx];
}

static reg_bf_info_map_t *calc_pfilt_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t pfilt_idx)
{
    static reg_bf_info_map_t pfilt_rx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_PFILT0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0)},   /* A0 */
        {BF_TRIG_SEL_MUX_PFILT1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0)},   /* A1 */

        {BF_TRIG_SEL_MUX_PFILT0_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1)},   /* B0 */
        {BF_TRIG_SEL_MUX_PFILT1_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1)},   /* B1 */   
    };

    static reg_bf_info_map_t pfilt_tx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_PFILT0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0)},   /* A0 */
        {BF_TRIG_SEL_MUX_PFILT1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0)},   /* A1 */

        {BF_TRIG_SEL_MUX_PFILT0_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1)},   /* B0 */
        {BF_TRIG_SEL_MUX_PFILT1_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1)},   /* B1 */    
    };

    return (terminal == ADI_APOLLO_RX) ? &pfilt_rx_trig_sel_mux[pfilt_idx] : &pfilt_tx_trig_sel_mux[pfilt_idx];
}

static reg_bf_info_map_t *calc_cfir_trig_sel_mux(adi_apollo_terminal_e terminal, uint8_t cfir_idx)
{
    static reg_bf_info_map_t cfir_rx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_CFIR_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 0)},   /* A0 */
        {BF_TRIG_SEL_MUX_CFIR_INFO(TXRX_TRIGGER_TS_RX_DIGITAL0, 1)},   /* A1 */

        {BF_TRIG_SEL_MUX_CFIR_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 0)},   /* B0 */
        {BF_TRIG_SEL_MUX_CFIR_INFO(TXRX_TRIGGER_TS_RX_DIGITAL1, 1)},   /* B1 */   
    };
 
    static reg_bf_info_map_t cfir_tx_trig_sel_mux[] = {
        {BF_TRIG_SEL_MUX_CFIR_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 0)},   /* A0 */
        {BF_TRIG_SEL_MUX_CFIR_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, 1)},   /* A1 */

        {BF_TRIG_SEL_MUX_CFIR_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 0)},   /* B0 */
        {BF_TRIG_SEL_MUX_CFIR_INFO(TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1, 1)},   /* B1 */   
    };

    return (terminal == ADI_APOLLO_RX) ? &cfir_rx_trig_sel_mux[cfir_idx] : &cfir_tx_trig_sel_mux[cfir_idx];
}

 uint32_t calc_trigts_base(adi_apollo_terminal_e terminal, int32_t side_index)
 {
     static uint32_t rx_trigts_regmap[ADI_APOLLO_NUM_SIDES] = {
         TXRX_TRIGGER_TS_RX_DIGITAL0, TXRX_TRIGGER_TS_RX_DIGITAL1
     };
     static uint32_t tx_trigts_regmap[ADI_APOLLO_NUM_SIDES] = {
         TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL0, TXRX_TRIGGER_TS_TX_TOP_TX_DIGITAL1
     };
     if (terminal == ADI_APOLLO_RX)
         return rx_trigts_regmap[side_index];
     else
         return tx_trigts_regmap[side_index];
 }



/*! @} */
