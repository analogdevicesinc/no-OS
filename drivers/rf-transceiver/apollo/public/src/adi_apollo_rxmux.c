/*!
 * \brief     Rx Crossbar Mux functional block API implementation
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXMUX
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_rxmux.h"
#include "adi_apollo_rxmux_local.h"
#include "adi_apollo_rxmisc_local.h"

#include "adi_apollo_jtx.h"

#include "adi_apollo_bf_rx_misc.h"
#include "adi_apollo_bf_rx_datin.h"
#include "adi_apollo_bf_jtx_dual_link.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_rxmux_xbar1_set(adi_apollo_device_t *device,  adi_apollo_side_select_e side_sel, adi_apollo_rx_mux0_sel_e cbout_from_adc_sel[], uint32_t length)
{
    int32_t err;
    uint8_t cb_out, side, side_index, bf_val;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cbout_from_adc_sel);

    if (device->dev_info.is_8t8r) {
        ADI_APOLLO_INVALID_PARAM_RETURN(length != ADI_APOLLO_RX_MUX0_NUM_8T8R);
    } else {
        ADI_APOLLO_INVALID_PARAM_RETURN(length != ADI_APOLLO_RX_MUX0_NUM_4T4R);
    }

    for (side_index = 0; side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {

            for (cb_out = 0; cb_out < length; cb_out++) {
                regmap_base_addr = calc_rx_datin_base((cb_out % 2) + 2*side_index);

                bf_val = cbout_from_adc_sel[cb_out];

                if (cb_out < 2) {
                    err = adi_apollo_hal_bf_set(device, BF_ADC_0_MUX_SEL_INFO(regmap_base_addr), bf_val);
                    ADI_APOLLO_ERROR_RETURN(err);
                } else {
                    err = adi_apollo_hal_bf_set(device, BF_ADC_1_MUX_SEL_INFO(regmap_base_addr), bf_val);
                    ADI_APOLLO_ERROR_RETURN(err);
                }
            }

        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rxmux_xbar2_set(adi_apollo_device_t *device,  adi_apollo_side_select_e side_sel, adi_apollo_rx_mux2_sel_e coarse_to_fine_xbar_sel[], uint32_t length)
{
    int32_t err;
    uint8_t side, side_index;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(coarse_to_fine_xbar_sel);
    ADI_APOLLO_INVALID_PARAM_RETURN(length != ADI_APOLLO_RX_MUX2_NUM);

    for (side_index = 0; side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = side_sel & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {

            regmap_base_addr = calc_rx_misc_base(side_index);

            err = adi_apollo_hal_bf_set(device, BF_CB_SEL_F0_INFO(regmap_base_addr), coarse_to_fine_xbar_sel[0]); 
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CB_SEL_F1_INFO(regmap_base_addr), coarse_to_fine_xbar_sel[1]); 
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CB_SEL_F2_INFO(regmap_base_addr), coarse_to_fine_xbar_sel[2]); 
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_set(device, BF_CB_SEL_F3_INFO(regmap_base_addr), coarse_to_fine_xbar_sel[3]); 
            ADI_APOLLO_ERROR_RETURN(err);

        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_rxmux_sample_xbar_set(adi_apollo_device_t *device, uint16_t link_sel, uint8_t sample_offset, adi_apollo_rxmux_sample_sel_e sample_sel[], uint32_t length)
{
    int32_t err;
    uint32_t regmap_base_addr = 0;
    uint8_t i, j;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(sample_sel);
    ADI_APOLLO_INVALID_PARAM_RETURN((sample_offset + length) > ADI_APOLLO_JESD_MAX_FRM_SAMPLE_XBAR_IDX);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & link_sel) {
            regmap_base_addr = calc_jtx_dual_link_base(i);
            for (j = 0; j < length; j++)
            err = adi_apollo_hal_bf_set(device, BF_JTX_CONV_SEL_INFO(regmap_base_addr, sample_offset + j), sample_sel[j]);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

uint32_t calc_rx_datin_base(int32_t adc_mux_index)
{
    static uint32_t rx_datin_regmap[] = {
        RX_DATIN_RX_SLICE_0_RX_DIGITAL0, RX_DATIN_RX_SLICE_1_RX_DIGITAL0,
        RX_DATIN_RX_SLICE_0_RX_DIGITAL1, RX_DATIN_RX_SLICE_1_RX_DIGITAL1
    };
    return rx_datin_regmap[adc_mux_index];
}
/*! @} */