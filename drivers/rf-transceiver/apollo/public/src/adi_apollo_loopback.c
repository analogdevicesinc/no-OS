/*!
 * \brief     Loopback 0 control functions
 *
 * \copyright copyright(c) 2023 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_LB0
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_loopback.h"
#include "adi_apollo_private_device.h"

#include "adi_apollo_bf_rx_loopback.h"
#include "adi_apollo_bf_tx_loopback.h"
#include "adi_apollo_bf_tx_misc.h"
#include "adi_apollo_txmisc_local.h"
#include "adi_apollo_bf_rx_misc.h"
#include "adi_apollo_rxmisc_local.h"
#include "adi_apollo_bf_custom.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_utils.h"

#include "adi_utils.h"


/*==================== D E F I N E S ====================*/

static uint32_t calc_rx_lb_base(int32_t side);
static uint32_t calc_tx_lb_base(int32_t adc);
static uint32_t calc_lb1_blend_base(int32_t cduc);

static int32_t adi_apollo_loopback_lb0_tx_xbar_map(adi_apollo_device_t *device, adi_apollo_adc_idx_e adc, adi_apollo_dac_idx_e dac, uint8_t *xbar);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_loopback_lb0_debug_data_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable)
{
    int32_t err, i, select;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK0_LOCK);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        select = sides & (ADI_APOLLO_SIDE_A << i);
        if (select > 0) {
            regmap_base_addr = calc_rx_lb_base(i);

            // Enable/disable debug data gating
            err = adi_apollo_hal_bf_set(device, BF_DBG_DATA_OFF_EN_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb0_rx_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable)
{
    int32_t err, i, select;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK0_LOCK);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        select = sides & (ADI_APOLLO_SIDE_A << i);
        if (select > 0) {
            regmap_base_addr = calc_rx_lb_base(i);

            // Enable/disable Rx loopback
            err = adi_apollo_hal_bf_set(device, BF_LPBK_WR_EN_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb0_tx_enable_set(adi_apollo_device_t *device, uint16_t adcs, uint8_t enable)
{
    int32_t err, i, j, select;
    uint32_t regmap_base_addr = 0;

    // For 4t4r device need to set two consecutive bitfields to enable, so we can't just loop through like normal. Need to calculate which indeces we need
    uint8_t tr4_offset = 4; // skip A2/A3 (normally indeces 2 and 3) and go straight to 4
    uint8_t tr4_adc_per_side = 2; // number of adcs per side for a 4t4r device

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK0_LOCK);

    // Enable/disable regs for 8t8r
    if (device->dev_info.is_8t8r) {
        for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
            select = adcs & (ADI_APOLLO_ADC_A0 << i);
            if (select > 0) {
                regmap_base_addr = calc_tx_lb_base(i);

                // Enable/disable Tx loopback
                err = adi_apollo_hal_bf_set(device, BF_LB0_EN_INFO(regmap_base_addr), enable);
                ADI_APOLLO_ERROR_RETURN(err);
            }
        }
    } else { // Enable/disable registers for 4t4r device
        for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
            for (j = 0; j < tr4_adc_per_side; j++) {
                select = adcs & (ADI_APOLLO_ADC_A0 << (tr4_offset * i + j));
                if (select > 0) {

                    regmap_base_addr = calc_tx_lb_base(tr4_offset * i + 2 * j);
                    err = adi_apollo_hal_bf_set(device, BF_LB0_EN_INFO(regmap_base_addr), enable);
                    ADI_APOLLO_ERROR_RETURN(err);
                
                    regmap_base_addr = calc_tx_lb_base(tr4_offset * i + 2 * j + 1);
                    err = adi_apollo_hal_bf_set(device, BF_LB0_EN_INFO(regmap_base_addr), enable);
                    ADI_APOLLO_ERROR_RETURN(err);

                }
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb0_tx_xbar_set(adi_apollo_device_t *device, uint16_t sides, uint16_t adcs[], uint32_t adc_map_length)
{
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint8_t i, select,xbar_value = 0xb0;
    adi_apollo_dac_idx_e dacs[ADI_APOLLO_DAC_PER_SIDE_NUM] = {ADI_APOLLO_DAC_0, ADI_APOLLO_DAC_1, ADI_APOLLO_DAC_2, ADI_APOLLO_DAC_3};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK0_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(adcs);

    // Check parameters are valid
    if ((device->dev_info.is_8t8r && adc_map_length != 4) || (!device->dev_info.is_8t8r && adc_map_length != 2)) {
        return API_CMS_ERROR_INVALID_PARAM;
    }
    
    // Calculate xbar value
    for (i = 0; i < adc_map_length; i++) {
        err = adi_apollo_loopback_lb0_tx_xbar_map(device, adcs[i], dacs[i], &xbar_value);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        select = sides & (ADI_APOLLO_SIDE_A << i);
        if (select > 0) {

            // set tx xbar
            regmap_base_addr = calc_tx_misc_base(i);
            err = adi_apollo_hal_bf_set(device, BF_HS_XBAR_CTRL_INFO(regmap_base_addr), xbar_value);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb0_read_ptr_rst_set(adi_apollo_device_t *device, uint16_t adcs, uint8_t value)
{
    int32_t err, i, select;
    uint32_t regmap_base_addr = 0;

    if (value >= 3){
        return API_CMS_ERROR_INVALID_PARAM;
    }

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK0_LOCK);

    for (i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        select = adcs & (ADI_APOLLO_ADC_A0 << i);
        if (select > 0) {
            regmap_base_addr = calc_tx_lb_base(i);

            // Set read pointer
            err = adi_apollo_hal_bf_set(device, BF_LB0_RDPTR_SYNC_RSTVAL_INFO(regmap_base_addr), value);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb0_write_ptr_rst_set(adi_apollo_device_t *device, uint16_t sides, uint8_t value)
{
    int32_t err, i, select;
    uint32_t regmap_base_addr = 0;

    if (value >= 3){
        return API_CMS_ERROR_INVALID_PARAM;
    }

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK0_LOCK);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        select = sides & (ADI_APOLLO_SIDE_A << i);
        if (select > 0) {
            regmap_base_addr = calc_rx_lb_base(i);

            // Set write ptr
            err = adi_apollo_hal_bf_set(device, BF_LPBK_WR_EN_INFO(regmap_base_addr), value);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb0_overflow_status_get(adi_apollo_device_t *device, uint16_t adcs, uint8_t *status)
{
    int32_t err;
    uint16_t sides;
    uint8_t bitmask;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK0_LOCK);
    ADI_APOLLO_NULL_POINTER_RETURN(status);

    err = adi_apollo_utils_side_from_adc_select_get(device, adcs, &sides);
    ADI_APOLLO_ERROR_RETURN(err);

    // Get the bitmask
    switch (adcs)
    {
    case ADI_APOLLO_ADC_A0:
    case ADI_APOLLO_ADC_B0:
        bitmask = 0x1;
        break;

    case ADI_APOLLO_ADC_A1:
    case ADI_APOLLO_ADC_B1:
        bitmask = 0x4;
        break;

    case ADI_APOLLO_ADC_A2:
    case ADI_APOLLO_ADC_B2:
        bitmask = 0x2;
        break;

    case ADI_APOLLO_ADC_A3:
    case ADI_APOLLO_ADC_B3:
        bitmask = 0x8;
        break;

    default:
        return API_CMS_ERROR_INVALID_PARAM;
    }

    // Side A
    if (sides == ADI_APOLLO_SIDE_A) {
        err = adi_apollo_hal_bf_get(device, BF_ADC_DATA_OVR_STATUS_INFO(RX_LOOPBACK_RX_SLICE_0_RX_DIGITAL0), status, 1);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_bf_set(device, BF_ADC_DATA_OVR_CLEAR_INFO(RX_LOOPBACK_RX_SLICE_0_RX_DIGITAL0), bitmask);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    // Side B
    if (sides == ADI_APOLLO_SIDE_B) {
        err = adi_apollo_hal_bf_get(device, BF_ADC_DATA_OVR_STATUS_INFO(RX_LOOPBACK_RX_SLICE_0_RX_DIGITAL1), status, 1);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_bf_set(device, BF_ADC_DATA_OVR_CLEAR_INFO(RX_LOOPBACK_RX_SLICE_0_RX_DIGITAL1), bitmask);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    // Set status
    *status = *status & bitmask ? 1 : 0;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb0_bmem_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable)
{
    int32_t err, i, select;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK0_LOCK);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        select = sides & (ADI_APOLLO_SIDE_A << i);
        if (select > 0) {
            regmap_base_addr = calc_rx_lb_base(i);

            // Enable/disable debug data gating
            err = adi_apollo_hal_bf_set(device, BF_LB0_BMEM_PATH_SEL_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_jesd_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable)
{

    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    uint16_t side;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(enable > 1);
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK123_LOCK);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        side = sides & (ADI_APOLLO_SIDE_A << i);
        if (side > 0) {
            regmap_base_addr = calc_tx_misc_base(i);

            // Ensure data reaches loopback point      
            if (enable) {
                err = adi_apollo_hal_bf_set(device, BF_CDUC_DAC_ENABLES0_INFO(regmap_base_addr), 3);
                ADI_APOLLO_ERROR_RETURN(err);

                err = adi_apollo_hal_bf_set(device, BF_FDUC_ENABLES00_INFO(regmap_base_addr), 0xFF);
                ADI_APOLLO_ERROR_RETURN(err);
            }

            err = adi_apollo_hal_bf_set(device, BF_JESD_LBK_MODE_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb1_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK123_LOCK);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        if ((sides & (1 << i)) > 0) {
            // Enable/disable LB1 write
            regmap_base_addr = calc_rx_misc_base(i);
            err = adi_apollo_hal_bf_set(device, BF_LB1_WR_EN_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb1_cduc_enable_set(adi_apollo_device_t *device, uint16_t cducs, uint8_t enable)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK123_LOCK);

    for (i = 0; i < ADI_APOLLO_CDUC_NUM; i++) {
        if ((cducs & (1 << i)) > 0) {
            regmap_base_addr = calc_tx_misc_base(i/ADI_APOLLO_CDUC_PER_SIDE_NUM);
            err = adi_apollo_hal_bf_set(device, BF_LB1_EN0_INFO(regmap_base_addr) + (i%ADI_APOLLO_CDUC_PER_SIDE_NUM), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb1_blend_set(adi_apollo_device_t *device, uint16_t cducs, adi_apollo_loopback_lb1_blend_mode_e mode)
{
    int32_t err, i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK123_LOCK);

    for (i = 0; i < ADI_APOLLO_CDUC_NUM; i++) {
        if ((cducs & (1 << i)) > 0) {
            err = adi_apollo_hal_bf_set(device, calc_lb1_blend_base(2*i), calc_lb1_blend_base(2*i + 1), mode);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb2_enable_set(adi_apollo_device_t *device, uint16_t sides, uint8_t enable)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK123_LOCK);

    for (i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        if ((sides & (1 << i)) > 0) {
            // Enable/disable LB2 write
            regmap_base_addr = calc_rx_misc_base(i);
            err = adi_apollo_hal_bf_set(device, BF_LB2_WR_EN_INFO(regmap_base_addr), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_loopback_lb2_fduc_enable_set(adi_apollo_device_t *device, uint16_t fducs, uint8_t enable)
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(ADI_APOLLO_TX, ADI_APOLLO_EC_LPBK123_LOCK);

    for (i = 0; i < ADI_APOLLO_FDUC_NUM; i++) {
        if ((fducs & (1 << i)) > 0) {
            regmap_base_addr = calc_tx_misc_base(i/ADI_APOLLO_FDUC_PER_SIDE_NUM);
            err = adi_apollo_hal_bf_set(device, BF_LB2_EN0_INFO(regmap_base_addr) + (i%ADI_APOLLO_FDUC_PER_SIDE_NUM), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}


static uint32_t calc_rx_lb_base(int32_t side) {
    static const uint32_t rx_lb_reg[ADI_APOLLO_NUM_SIDES] = {RX_LOOPBACK_RX_SLICE_0_RX_DIGITAL0, RX_LOOPBACK_RX_SLICE_0_RX_DIGITAL1};
    return rx_lb_reg[side];
}

static uint32_t calc_tx_lb_base(int32_t adc){
    static const uint32_t tx_lb_reg[ADI_APOLLO_ADC_NUM] = {
        TX_LOOPBACK0_TX_SLICE_0_TX_DIGITAL0, TX_LOOPBACK1_TX_SLICE_0_TX_DIGITAL0, 
        TX_LOOPBACK0_TX_SLICE_1_TX_DIGITAL0, TX_LOOPBACK1_TX_SLICE_1_TX_DIGITAL0, 
        TX_LOOPBACK0_TX_SLICE_0_TX_DIGITAL1, TX_LOOPBACK1_TX_SLICE_0_TX_DIGITAL1,
        TX_LOOPBACK0_TX_SLICE_1_TX_DIGITAL1, TX_LOOPBACK1_TX_SLICE_1_TX_DIGITAL1
        };
    return tx_lb_reg[adc];
}

static int32_t adi_apollo_loopback_lb0_tx_xbar_map(adi_apollo_device_t *device, adi_apollo_adc_idx_e adc, adi_apollo_dac_idx_e dac, uint8_t *xbar) {
    uint8_t bit_shift;
    uint8_t map_val;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    // Get bit value for adcs
    switch (adc)
    {
    case ADI_APOLLO_ADC_0:
        map_val = 0;
        break;
    case ADI_APOLLO_ADC_1:
        map_val = 1;
        break;
    case ADI_APOLLO_ADC_2:
        map_val = 2;
        break;
    case ADI_APOLLO_ADC_3:
        map_val = 3;
        break;

    default:
        ADI_APOLLO_ERROR_RETURN(API_CMS_ERROR_INVALID_PARAM);
    }

    // Get bit positions
    switch (dac)
    {
    case ADI_APOLLO_DAC_0:
        bit_shift = 0;
        break;
    case ADI_APOLLO_DAC_1:
        bit_shift = 2;
        break;
    case ADI_APOLLO_DAC_2:
        bit_shift = 4;
        break;
    case ADI_APOLLO_DAC_3:
        bit_shift = 6;
        break;

    default:
        ADI_APOLLO_ERROR_RETURN(API_CMS_ERROR_INVALID_PARAM);
    }

    // Update xbar value
    *xbar = *xbar & ~((uint8_t)3 << bit_shift);
    *xbar = *xbar | (map_val << bit_shift);

    return API_CMS_ERROR_OK;
}

static uint32_t calc_lb1_blend_base(int32_t cduc){
    static const uint32_t lb_reg[ADI_APOLLO_CDUC_NUM * 2] = {
        BF_SL0_LB1_SHIFT_INFO(TX_MISC_TX_TOP_TX_DIGITAL0), BF_SL1_LB1_SHIFT_INFO(TX_MISC_TX_TOP_TX_DIGITAL0), 
        BF_SL0_LB1_SHIFT_8T8R_INFO(TX_MISC_TX_TOP_TX_DIGITAL0), BF_SL1_LB1_SHIFT_8T8R_INFO(TX_MISC_TX_TOP_TX_DIGITAL0),
        BF_SL0_LB1_SHIFT_INFO(TX_MISC_TX_TOP_TX_DIGITAL1), BF_SL1_LB1_SHIFT_INFO(TX_MISC_TX_TOP_TX_DIGITAL1), 
        BF_SL0_LB1_SHIFT_8T8R_INFO(TX_MISC_TX_TOP_TX_DIGITAL1), BF_SL1_LB1_SHIFT_8T8R_INFO(TX_MISC_TX_TOP_TX_DIGITAL1)
        };
    return lb_reg[cduc];
}
/*! @} */