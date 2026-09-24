/*!
 * \brief     Apollo API utilities
 *
 * \copyright copyright(c) 2023 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_UTILS
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_utils.h"

static int32_t converter_to_sides(adi_apollo_device_t* device, uint16_t converter, uint16_t *side_select);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_utils_side_from_adc_select_get(adi_apollo_device_t *device, uint16_t adc_select, uint16_t *side_select) {
    return converter_to_sides(device, adc_select, side_select);
}

int32_t adi_apollo_utils_side_from_dac_select_get(adi_apollo_device_t *device, uint16_t dac_select, uint16_t *side_select) {
    return converter_to_sides(device, dac_select, side_select);
}

/*==================== L O C A L   A P I   C O D E ====================*/

static int32_t converter_to_sides(adi_apollo_device_t* device, uint16_t converter, uint16_t *side_select) {
    adi_apollo_side_select_e sel = 0;
    
    if (converter & ADI_APOLLO_ADC_A_ALL) {
        sel = sel | ADI_APOLLO_SIDE_A;
    }
    if (converter & ADI_APOLLO_ADC_B_ALL) {
        sel = sel | ADI_APOLLO_SIDE_B;
    }
    *side_select = sel;

    return API_CMS_ERROR_OK;
}
/*! @} */